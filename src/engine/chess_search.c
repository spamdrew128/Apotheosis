#include <stdint.h>
#include <time.h>

#include "chess_search.h"
#include "movegen.h"
#include "make_and_unmake.h"
#include "RNG.h"
#include "endings.h"
#include "UCI.h"
#include "timer.h"

enum {
    overhead_msec = 10,
    time_fraction = 25
};

typedef struct {
    bool outOfTime;
} SearchInfo_t;

static Timer_t globalTimer;

static void InitSearchInfo(SearchInfo_t* searchInfo) {
    searchInfo->outOfTime = false;
}

static void MakeAndAddHash(BoardInfo_t* boardInfo, GameStack_t* gameStack, Move_t move, ZobristStack_t* zobristStack) {
    MakeMove(boardInfo, gameStack, move);
    AddZobristHashToStack(zobristStack, HashPosition(boardInfo, gameStack));
}

static void UnmakeAndAddHash(BoardInfo_t* boardInfo, GameStack_t* gameStack, ZobristStack_t* zobristStack) {
    UnmakeMove(boardInfo, gameStack);
    RemoveZobristHashFromStack(zobristStack);
}

static EvalScore_t NegamaxHelper(
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
    MoveList_t moveList;
    CompleteMovegen(&moveList, boardInfo, gameStack);

    GameEndStatus_t gameEndStatus = CurrentGameEndStatus(boardInfo, gameStack, zobristStack, moveList.maxIndex);
    switch (gameEndStatus)
    {
        case checkmate:
            return -EVAL_MAX + ply;
        case draw:
            return 0;
    }

    if(depth == 0) {
        return ScoreOfPosition(boardInfo);
    }

    EvalScore_t bestScore = -EVAL_MAX;
    
    for(int i = 0; i <= moveList.maxIndex; i++) {
        Move_t move = moveList.moves[i];
        MakeAndAddHash(boardInfo, gameStack, move, zobristStack);

        EvalScore_t score = -NegamaxHelper(boardInfo, gameStack, zobristStack, searchInfo, -beta, -alpha, depth-1, ply+1);

        UnmakeAndAddHash(boardInfo, gameStack, zobristStack);

        if(TimerExpired(&globalTimer)) {
            searchInfo->outOfTime = true;
            return score;
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

static SearchResults_t NegamaxRoot(
    BoardInfo_t* boardInfo,
    GameStack_t* gameStack,
    ZobristStack_t* zobristStack,
    SearchInfo_t* searchInfo,
    Depth_t depth
)
{
    Move_t bestMove;
    EvalScore_t bestScore = -EVAL_MAX;
    
    MoveList_t moveList;
    CompleteMovegen(&moveList, boardInfo, gameStack);
    for(int i = 0; i <= moveList.maxIndex; i++) {
        Move_t move = moveList.moves[i];
        MakeAndAddHash(boardInfo, gameStack, move, zobristStack);

        EvalScore_t score = -NegamaxHelper(boardInfo, gameStack, zobristStack, searchInfo, -INFINITY, INFINITY, depth-1, 1);

        UnmakeAndAddHash(boardInfo, gameStack, zobristStack);

        if(searchInfo->outOfTime) {
            break;
        }

        if(score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
    }

    SearchResults_t results;
    results.score = bestScore;
    results.bestMove = bestMove;

    return results;
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
        timeToUse = (totalTime + increment) / time_fraction;
    }

    TimerInit(&globalTimer, timeToUse - overhead_msec);
}

SearchResults_t Search(
    UciSearchInfo_t uciSearchInfo,
    BoardInfo_t* boardInfo,
    GameStack_t* gameStack,
    ZobristStack_t* zobristStack
)
{
    SetupGlobalTimer(uciSearchInfo, boardInfo);

    SearchInfo_t searchInfo;
    InitSearchInfo(&searchInfo);

    SearchResults_t searchResults;
    Depth_t currentDepth = 0;
    do {
        currentDepth++;
        SearchResults_t newResults
            = NegamaxRoot(boardInfo, gameStack, zobristStack, &searchInfo, currentDepth);

        if(!searchInfo.outOfTime) {
            searchResults = newResults;
            SendUciInfoString("score cp %d depth %d", searchResults.score, currentDepth);
        }
    } while(!searchInfo.outOfTime && currentDepth != uciSearchInfo.depthLimit);

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