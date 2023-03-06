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

enum {
    time_fraction = 25,
    timer_check_freq = 1024,

    MATE_THRESHOLD = EVAL_MAX - 100,

    DEPTH_MAX = PLY_MAX
};

#define MATING "mate "
#define MATED "mate -"
#define NO_MATE "cp "

typedef struct {
    bool outOfTime;
    NodeCount_t nodeCount;
    PvTable_t pvTable;
    TranspositionTable_t* tt;
} ChessSearchInfo_t;

static Timer_t globalTimer;

static void InitSearchInfo(ChessSearchInfo_t* chessSearchInfo, UciSearchInfo_t* uciSearchInfo) {
    chessSearchInfo->outOfTime = false;
    chessSearchInfo->nodeCount = 0;
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

    MoveList_t moveList;
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

    SortCaptures(&moveList, boardInfo);

    EvalScore_t bestScore = standPat;
    for(int i = 0; i <= moveList.maxCapturesIndex; i++) {
        searchInfo->nodeCount++;
        Move_t move = moveList.moves[i];
        MakeAndAddHash(boardInfo, gameStack, move, zobristStack);

        EvalScore_t score = -QSearch(boardInfo, gameStack, zobristStack, searchInfo, -beta, -alpha, ply+1);

        UnmakeAndRemoveHash(boardInfo, gameStack, zobristStack);

        if(searchInfo->outOfTime) {
            return 0;
        }

        if(score >= beta) {
            return score;
        }

        if(score > bestScore) {
            bestScore = score;
            if(score > alpha) {
                alpha = score;
            }
        }
    }

    return bestScore;
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
    if(ShouldCheckTimer(searchInfo->nodeCount) && TimerExpired(&globalTimer)) {
        searchInfo->outOfTime = true;
        return 0;
    }

    PvLengthInit(&searchInfo->pvTable, ply);

    if(depth == 0) {
        return QSearch(boardInfo, gameStack, zobristStack, searchInfo, alpha, beta, ply);
    }

    MoveList_t moveList;
    CompleteMovegen(&moveList, boardInfo, gameStack);

    GameEndStatus_t gameEndStatus = CurrentGameEndStatus(boardInfo, gameStack, zobristStack, moveList.maxIndex);
    switch (gameEndStatus) {
        case checkmate:
            return -EVAL_MAX + ply;
        case draw:
            return 0;
    }

    SortCaptures(&moveList, boardInfo);

    ZobristHash_t hash = ZobristStackTop(zobristStack);
    TTEntry_t* entry = GetTTEntry(searchInfo->tt, hash);
    if(TTHit(entry, hash)) {
        SortTTMove(&moveList, entry->move, moveList.maxIndex);
    }

    TTFlag_t flag = upper_bound; // start at upper bound and update if we know we won't fail low

    EvalScore_t bestScore = -EVAL_MAX;
    Move_t bestMove;
    for(int i = 0; i <= moveList.maxIndex; i++) {
        Move_t move = moveList.moves[i];
        MakeAndAddHash(boardInfo, gameStack, move, zobristStack);

        EvalScore_t score = -Negamax(boardInfo, gameStack, zobristStack, searchInfo, -beta, -alpha, depth-1, ply+1);

        UnmakeAndRemoveHash(boardInfo, gameStack, zobristStack);

        searchInfo->nodeCount++;

        if(searchInfo->outOfTime) {
            return 0;
        }

        if(score > bestScore) {
            bestScore = score;
            bestMove = move;
            if(score >= beta) {
                flag = lower_bound;
                break;
            }

            if(score > alpha) {
                alpha = score;
                flag = exact;
                UpdatePvTable(&searchInfo->pvTable, move, ply);
            }
        }
    }

    ReplaceTTEntry(entry, flag, depth, bestMove, bestScore, hash);

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

    TimerInit(&globalTimer, timeToUse - uciSearchInfo->overhead);
}

static void PrintUciInformation(
    ChessSearchInfo_t searchInfo,
    SearchResults_t searchResults,
    Depth_t currentDepth,
    Stopwatch_t* stopwatch
)
{
    const char* scoreType = NO_MATE;
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

    SendUciInfoString(
        "score %s%d depth %d nodes %lld time %lld",
        scoreType,
        scoreValue,
        currentDepth,
        (long long)searchInfo.nodeCount,
        (long long)ElapsedTime(stopwatch)
    );

    SendPvInfo(&searchInfo.pvTable, currentDepth);
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
    Depth_t currentDepth = 0;
    do {
        currentDepth++;

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