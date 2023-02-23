#include <stdint.h>
#include <time.h>

#include "chess_search.h"
#include "movegen.h"
#include "make_and_unmake.h"
#include "RNG.h"
#include "endings.h"
#include "UCI.h"
#include "timer.h"
#include "PV_table.h"

enum {
    overhead_msec = 10,
    time_fraction = 25,
    timer_check_freq = 1024,

    DEPTH_MAX = PLY_MAX
};

typedef uint64_t NodeCount_t;
typedef struct {
    bool outOfTime;
    NodeCount_t nodeCount;
    PvTable_t pvTable;
} SearchInfo_t;

static Timer_t globalTimer;

static void InitSearchInfo(SearchInfo_t* searchInfo) {
    searchInfo->outOfTime = false;
    searchInfo->nodeCount = 0;
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
    SearchInfo_t* searchInfo,
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

    GameEndStatus_t gameEndStatus = CurrentGameEndStatus(boardInfo, gameStack, zobristStack, moveList.maxIndex);
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
    SearchInfo_t* searchInfo,
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

    MoveList_t moveList;
    CompleteMovegen(&moveList, boardInfo, gameStack);

    GameEndStatus_t gameEndStatus = CurrentGameEndStatus(boardInfo, gameStack, zobristStack, moveList.maxIndex);
    switch (gameEndStatus) {
        case checkmate:
            return -EVAL_MAX + ply;
        case draw:
            return 0;
    }

    if(depth == 0) {
        return QSearch(boardInfo, gameStack, zobristStack, searchInfo, alpha, beta, ply+1);
    }

    EvalScore_t bestScore = -EVAL_MAX;
    
    for(int i = 0; i <= moveList.maxIndex; i++) {
        Move_t move = moveList.moves[i];
        MakeAndAddHash(boardInfo, gameStack, move, zobristStack);

        EvalScore_t score = -Negamax(boardInfo, gameStack, zobristStack, searchInfo, -beta, -alpha, depth-1, ply+1);

        UnmakeAndRemoveHash(boardInfo, gameStack, zobristStack);

        searchInfo->nodeCount++;

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
                UpdatePvTable(&searchInfo->pvTable, move, ply);
            }
        }
    }

    return bestScore;
}

static void SetupGlobalTimer(UciSearchInfo_t uciSearchInfo, BoardInfo_t* boardInfo) {
    Milliseconds_t totalTime;
    Milliseconds_t increment;
    if(boardInfo->colorToMove == white) {
        totalTime = uciSearchInfo.wTime;
        increment = uciSearchInfo.wInc;
    } else {
        totalTime = uciSearchInfo.bTime;
        increment = uciSearchInfo.bInc;
    }

    Milliseconds_t timeToUse;
    if(uciSearchInfo.forceTime) {
        timeToUse = uciSearchInfo.forceTime;
    } else {
        timeToUse = (totalTime + increment/2) / time_fraction;
    }

    TimerInit(&globalTimer, timeToUse - overhead_msec);
}

SearchResults_t Search(
    UciSearchInfo_t uciSearchInfo,
    BoardInfo_t* boardInfo,
    GameStack_t* gameStack,
    ZobristStack_t* zobristStack,
    bool printUciInfo
)
{
    Stopwatch_t stopwatch;
    StopwatchInit(&stopwatch);
    SetupGlobalTimer(uciSearchInfo, boardInfo);

    SearchInfo_t searchInfo;
    InitSearchInfo(&searchInfo);

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
                SendUciInfoString(
                    "score cp %d depth %d nodes %lld time %lld",
                    searchResults.score,
                    currentDepth,
                    (long long)searchInfo.nodeCount,
                    (long long)ElapsedTime(&stopwatch)
                );
                SendPvInfo(&searchInfo.pvTable, currentDepth);
                // removed NPS uci because it breaks cutechess for some reason
            }
        }

    } while(!searchInfo.outOfTime && currentDepth != uciSearchInfo.depthLimit && currentDepth < DEPTH_MAX);

    return searchResults;
}

void UciSearchInfoInit(UciSearchInfo_t* uciSearchInfo) {
    uciSearchInfo->wTime = 0;
    uciSearchInfo->bTime = 0;
    uciSearchInfo->wInc = 0;
    uciSearchInfo->bInc = 0;
    uciSearchInfo->forceTime = 0;
    uciSearchInfo->depthLimit = 0;
}