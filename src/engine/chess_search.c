#include <stdint.h>
#include <time.h>

#include "chess_search.h"
#include "movegen.h"
#include "make_and_unmake.h"
#include "move.h"
#include "RNG.h"
#include "endings.h"
#include "UCI.h"
#include "timer.h"
#include "PV_table.h"
#include "move_ordering.h"
#include "transposition_table.h"
#include "killers.h"
#include "history.h"
#include "util_macros.h"
#include "FEN.h"

enum {
    time_fraction = 25,
    timer_check_freq = 1024,

    MIN_TIME_PER_MOVE = 5,
    DEPTH_MAX = PLY_MAX - 30, // leaving room for qsearch to expand
};

#define MATING "mate "
#define MATED "mate -"
#define NO_MATE "cp "

typedef struct {
    bool outOfTime;
    NodeCount_t nodeCount;
    Depth_t seldepth;
    Killers_t killers;
    History_t history;
    PvTable_t pvTable;
    TranspositionTable_t* tt;
} ChessSearchInfo_t;

static Timer_t globalTimer;

static EvalScore_t Negamax(
    BoardInfo_t* boardInfo,
    GameStack_t* gameStack,
    ZobristStack_t* zobristStack,
    ChessSearchInfo_t* searchInfo,
    EvalScore_t alpha,
    EvalScore_t beta,
    Depth_t depth,
    Ply_t ply
);

bool IsQuiet(Move_t move, BoardInfo_t* boardInfo) {
    return 
        PieceOnSquare(boardInfo, ReadToSquare(move)) == none_type &&
        ReadSpecialFlag(move) != en_passant_flag;
}

static void ResetSeldepth(ChessSearchInfo_t* chessSearchInfo) {
    chessSearchInfo->seldepth = 0;
}

static void InitSearchInfo(ChessSearchInfo_t* chessSearchInfo, UciSearchInfo_t* uciSearchInfo) {
    chessSearchInfo->outOfTime = false;
    chessSearchInfo->nodeCount = 0;
    ResetSeldepth(chessSearchInfo);
    InitKillers(&chessSearchInfo->killers);
    InitHistory(&chessSearchInfo->history);
    chessSearchInfo->tt = &uciSearchInfo->tt;
}

static bool ShouldCheckTimer(NodeCount_t nodeCount) {
    return nodeCount % timer_check_freq == 0;
}

static void MakeAndAddHash(BoardInfo_t* boardInfo, GameStack_t* gameStack, Move_t move, ZobristStack_t* zobristStack) {
    MakeMove(boardInfo, gameStack, move);
    AddZobristHashToStack(zobristStack, HashPosition(boardInfo, gameStack));
}

static void UnmakeAndRemoveHash(BoardInfo_t* boardInfo, GameStack_t* gameStack, ZobristStack_t* zobristStack) {
    UnmakeMove(boardInfo, gameStack);
    RemoveZobristHashFromStack(zobristStack);
}

static EvalScore_t QSearch(
    BoardInfo_t* boardInfo,
    GameStack_t* gameStack,
    ZobristStack_t* zobristStack,
    ChessSearchInfo_t* searchInfo,
    EvalScore_t alpha,
    EvalScore_t beta,
    Ply_t ply
)
{
    if(ShouldCheckTimer(searchInfo->nodeCount) && TimerExpired(&globalTimer)) {
        searchInfo->outOfTime = true;
        return 0;
    }

    searchInfo->seldepth = MAX(searchInfo->seldepth, ply);

    MoveEntryList_t moveList;
    CompleteMovegen(&moveList, boardInfo, gameStack);

    GameEndStatus_t gameEndStatus = CheckForMates(boardInfo, moveList.maxIndex);
    switch (gameEndStatus) {
        case checkmate:
            return -EVAL_MAX + ply;
        case draw:
            return 0;
    }

    EvalScore_t standPat = ScoreOfPosition(boardInfo);
    if(standPat >= beta) {
        return standPat;
    }

    if(standPat > alpha) {
        alpha = standPat;
    }

    MovePicker_t movePicker;
    InitCaptureMovePicker(&movePicker, &moveList, boardInfo);

    EvalScore_t bestScore = standPat;
    for(int i = 0; i <= moveList.maxCapturesIndex; i++) {
        searchInfo->nodeCount++;
        Move_t move = PickMove(&movePicker);
        MakeAndAddHash(boardInfo, gameStack, move, zobristStack);

        EvalScore_t score = -QSearch(boardInfo, gameStack, zobristStack, searchInfo, -beta, -alpha, ply+1);

        UnmakeAndRemoveHash(boardInfo, gameStack, zobristStack);

        if(searchInfo->outOfTime) {
            return 0;
        }

        if(score > bestScore) {
            bestScore = score;

            if(score >= beta) {
                break;
            }
            
            if(score > alpha) {
                alpha = score;
            }
        }
    }

    return bestScore;
}

static EvalScore_t NullWindowSearch(
    BoardInfo_t* boardInfo,
    GameStack_t* gameStack,
    ZobristStack_t* zobristStack,
    ChessSearchInfo_t* searchInfo,
    EvalScore_t alpha,
    Depth_t depth,
    Ply_t ply
)
{
    EvalScore_t nullWindowBeta = alpha + 1;
    EvalScore_t score = -Negamax(boardInfo, gameStack, zobristStack, searchInfo, -nullWindowBeta, -alpha, depth-1, ply+1);

    return score;
}

static EvalScore_t Negamax(
    BoardInfo_t* boardInfo,
    GameStack_t* gameStack,
    ZobristStack_t* zobristStack,
    ChessSearchInfo_t* searchInfo,
    EvalScore_t alpha,
    EvalScore_t beta,
    Depth_t depth,
    Ply_t ply
)
{
    searchInfo->nodeCount++;
    PvLengthInit(&searchInfo->pvTable, ply);

    if(depth == 0) {
        return QSearch(boardInfo, gameStack, zobristStack, searchInfo, alpha, beta, ply);
    }

    const bool isRoot = ply == 0;
    const bool isPVNode = beta - alpha != 1;

    if(ShouldCheckTimer(searchInfo->nodeCount) && TimerExpired(&globalTimer)) {
        searchInfo->outOfTime = true;
        return 0;
    }

    searchInfo->seldepth = MAX(searchInfo->seldepth, ply);

    MoveEntryList_t moveList;
    CompleteMovegen(&moveList, boardInfo, gameStack);

    if(!isRoot) {
        GameEndStatus_t gameEndStatus = CurrentGameEndStatus(boardInfo, gameStack, zobristStack, moveList.maxIndex);
        switch (gameEndStatus) {
            case checkmate:
                return -EVAL_MAX + ply;
            case draw:
                return 0;
        }
    }

    ZobristHash_t hash = ZobristStackTop(zobristStack);
    TTIndex_t ttIndex = GetTTIndex(searchInfo->tt, hash);
    TTEntry_t entry = GetTTEntry(searchInfo->tt, ttIndex);
    Move_t ttMove = NullMove();
    if(TTHit(entry, hash)) {
        if(!isPVNode && TTCutoffIsPossible(entry, alpha, beta, depth)) {
            return ScoreFromTT(entry.bestScore, ply);
        }
        
        ttMove = entry.bestMove;
    }

    MovePicker_t movePicker;
    InitAllMovePicker(
        &movePicker,
        &moveList,
        boardInfo,
        ttMove,
        &searchInfo->killers,
        &searchInfo->history,
        ply
    );

    EvalScore_t oldAlpha = alpha;
    EvalScore_t bestScore = -EVAL_MAX;
    Move_t bestMove;
    for(int i = 0; i <= moveList.maxIndex; i++) {
        EvalScore_t score;
        Move_t move = PickMove(&movePicker);
        MakeAndAddHash(boardInfo, gameStack, move, zobristStack);
    
        if(i == 0) {
            score = -Negamax(boardInfo, gameStack, zobristStack, searchInfo, -beta, -alpha, depth-1, ply+1);
        } else {
            score = NullWindowSearch(boardInfo, gameStack, zobristStack, searchInfo, alpha, depth, ply);
            // if our NWS beat alpha without failing high, that means we might have a better move and need to re search
            if(score > alpha && score < beta) {
                score = -Negamax(boardInfo, gameStack, zobristStack, searchInfo, -beta, -alpha, depth-1, ply+1);
            }
        }

        UnmakeAndRemoveHash(boardInfo, gameStack, zobristStack);

        if(searchInfo->outOfTime) {
            return 0;
        }

        if(score > bestScore) {
            bestScore = score;
            bestMove = move;
            if(score >= beta) {
                if(IsQuiet(move, boardInfo)) {
                    AddKiller(&searchInfo->killers, move, ply);
                    UpdateHistory(&searchInfo->history, boardInfo, move, depth);
                }
                break;
            }

            if(score > alpha) {
                alpha = score;
                UpdatePvTable(&searchInfo->pvTable, move, ply);
            }
        }
    }

    TTFlag_t flag = DetermineTTFlag(bestScore, oldAlpha, alpha, beta);
    StoreTTEntry(searchInfo->tt, ttIndex, flag, depth, ply, bestMove, bestScore, hash);

    return bestScore;
}

static void SetupGlobalTimer(UciSearchInfo_t* uciSearchInfo, BoardInfo_t* boardInfo) {
    Milliseconds_t totalTime;
    Milliseconds_t increment;
    if(boardInfo->colorToMove == white) {
        totalTime = uciSearchInfo->wTime;
        increment = uciSearchInfo->wInc;
    } else {
        totalTime = uciSearchInfo->bTime;
        increment = uciSearchInfo->bInc;
    }

    Milliseconds_t timeToUse;
    if(uciSearchInfo->forceTime) {
        timeToUse = uciSearchInfo->forceTime;
    } else {
        timeToUse = (totalTime + increment/2) / time_fraction;
    }

    timeToUse = MAX(timeToUse - uciSearchInfo->overhead, MIN_TIME_PER_MOVE);

    TimerInit(&globalTimer, timeToUse);
}

static void PrintUciInformation(
    ChessSearchInfo_t searchInfo,
    SearchResults_t searchResults,
    Depth_t currentDepth,
    Stopwatch_t* stopwatch
)
{
    char* scoreType = NO_MATE;
    EvalScore_t scoreValue = searchResults.score;

    if(searchResults.score > MATE_THRESHOLD) {
        scoreType = MATING;
        Ply_t ply = EVAL_MAX - searchResults.score;
        scoreValue = (ply + 1)/2;

    } else if(searchResults.score < -MATE_THRESHOLD) {
        scoreType = MATED;
        Ply_t ply = EVAL_MAX + searchResults.score;
        scoreValue = (ply + 1)/2;
    }

    Milliseconds_t time = ElapsedTime(stopwatch) + 1;
    long long nps =((searchInfo.nodeCount * msec_per_sec) / time);
    SendUciInfoString(
        "score %s%d depth %d seldepth %d nodes %lld time %lld nps %lld hashfull %d",
        &searchInfo.pvTable,
        scoreType,
        scoreValue,
        currentDepth,
        searchInfo.seldepth,
        (long long)searchInfo.nodeCount,
        (long long)time,
        nps,
        HashFull(searchInfo.tt)
    );
}

Move_t FirstLegalMove(BoardInfo_t* boardinfo, GameStack_t* gameStack) {
    MoveEntryList_t list;
    CompleteMovegen(&list, boardinfo, gameStack);
    return list.moves[0].move;
}

SearchResults_t Search(
    UciSearchInfo_t* uciSearchInfo,
    BoardInfo_t* boardInfo,
    GameStack_t* gameStack,
    ZobristStack_t* zobristStack,
    bool printUciInfo
)
{
    Stopwatch_t stopwatch;
    StopwatchInit(&stopwatch);
    SetupGlobalTimer(uciSearchInfo, boardInfo);

    ChessSearchInfo_t searchInfo;
    InitSearchInfo(&searchInfo, uciSearchInfo);

    SearchResults_t searchResults;
     // so if we time out during depth 1 search we have something to return
    searchResults.bestMove = FirstLegalMove(boardInfo, gameStack);
    Depth_t currentDepth = 0;
    do {
        currentDepth++;
        ResetSeldepth(&searchInfo);

        EvalScore_t score = Negamax(
            boardInfo,
            gameStack,
            zobristStack,
            &searchInfo,
            -INFINITY,
            INFINITY,
            currentDepth,
            0
        );

        if(!searchInfo.outOfTime) {
            searchResults.bestMove = PvTableBestMove(&searchInfo.pvTable);
            searchResults.score = score;

            if(printUciInfo) {
                PrintUciInformation(searchInfo, searchResults, currentDepth, &stopwatch);
            }
        }

    } while(!searchInfo.outOfTime && currentDepth != uciSearchInfo->depthLimit && currentDepth < DEPTH_MAX);

    if(printUciInfo) {
        printf("bestmove ");
        PrintMove(searchResults.bestMove, true);
    }

    return searchResults;
}

NodeCount_t BenchSearch(
    UciSearchInfo_t* uciSearchInfo,
    BoardInfo_t* boardInfo,
    GameStack_t* gameStack,
    ZobristStack_t* zobristStack
)
{
    SetupGlobalTimer(uciSearchInfo, boardInfo);
    
    ChessSearchInfo_t searchInfo;
    InitSearchInfo(&searchInfo, uciSearchInfo);

    Depth_t currentDepth = 0;
    do {
        currentDepth++;
        ResetSeldepth(&searchInfo);

        Negamax(
            boardInfo,
            gameStack,
            zobristStack,
            &searchInfo,
            -INFINITY,
            INFINITY,
            currentDepth,
            0
        );
    } while(currentDepth != uciSearchInfo->depthLimit && currentDepth < DEPTH_MAX);

    return searchInfo.nodeCount;
}

void UciSearchInfoTimeInfoReset(UciSearchInfo_t* uciSearchInfo) {
    uciSearchInfo->wTime = 0;
    uciSearchInfo->bTime = 0;
    uciSearchInfo->wInc = 0;
    uciSearchInfo->bInc = 0;
    uciSearchInfo->forceTime = 0;
}

void UciSearchInfoInit(UciSearchInfo_t* uciSearchInfo) {
    uciSearchInfo->wTime = 0;
    uciSearchInfo->bTime = 0;
    uciSearchInfo->wInc = 0;
    uciSearchInfo->bInc = 0;
    uciSearchInfo->forceTime = 0;
    uciSearchInfo->overhead = overhead_default_msec;

    uciSearchInfo->depthLimit = 0;

    TranspositionTableInit(&uciSearchInfo->tt, hash_default_mb);
}

EvalScore_t SimpleQsearch(
    BoardInfo_t* boardInfo,
    GameStack_t* gameStack,
    ZobristStack_t* zobristStack,
    EvalScore_t alpha,
    EvalScore_t beta
)
{
    MoveEntryList_t moveList;
    CompleteMovegen(&moveList, boardInfo, gameStack);

    GameEndStatus_t gameEndStatus = CheckForMates(boardInfo, moveList.maxIndex);
    switch (gameEndStatus) {
        case checkmate:
            return -EVAL_MAX;
        case draw:
            return 0;
    }

    EvalScore_t standPat = ScoreOfPosition(boardInfo);
    if(standPat >= beta) {
        return standPat;
    }

    if(standPat > alpha) {
        alpha = standPat;
    }

    MovePicker_t movePicker;
    InitCaptureMovePicker(&movePicker, &moveList, boardInfo);

    EvalScore_t bestScore = standPat;
    for(int i = 0; i <= moveList.maxCapturesIndex; i++) {
        Move_t move = PickMove(&movePicker);
        MakeAndAddHash(boardInfo, gameStack, move, zobristStack);

        EvalScore_t score = -SimpleQsearch(boardInfo, gameStack, zobristStack, -beta, -alpha);

        UnmakeAndRemoveHash(boardInfo, gameStack, zobristStack);

        if(score > bestScore) {
            bestScore = score;

            if(score >= beta) {
                break;
            }
            
            if(score > alpha) {
                alpha = score;
            }
        }
    }

    return bestScore;
}