#include <stdint.h>
#include <time.h>

#include "chess_search.h"
#include "movegen.h"
#include "make_and_unmake.h"
#include "RNG.h"
#include "endings.h"
#include "UCI.h"
#include "timer.h"

typedef struct {
    bool timeDepleted;
} SearchInfo_t;

static Timer_t globalTimer;

static void InitSearchInfo(SearchInfo_t* searchInfo) {
    searchInfo->timeDepleted = false;
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

static void SetupGlobalTimer(PlayerTimeInfo_t uciTimeInfo, BoardInfo_t* boardInfo) {
    Milliseconds_t totalTime;
    Milliseconds_t increment;
    if(boardInfo->colorToMove == white) {
        totalTime = uciTimeInfo.wTime;
        increment = uciTimeInfo.wInc;
    } else {
        totalTime = uciTimeInfo.bTime;
        increment = uciTimeInfo.bInc;
    }

    Milliseconds_t timeToUse = (totalTime + increment) / 20;
    TimerInit(&globalTimer, timeToUse);
}

SearchResults_t Search(
    PlayerTimeInfo_t uciTimeInfo,
    BoardInfo_t* boardInfo,
    GameStack_t* gameStack,
    ZobristStack_t* zobristStack,
    Depth_t maxDepth
)
{
    SetupGlobalTimer(uciTimeInfo, boardInfo);

    SearchInfo_t searchInfo;
    InitSearchInfo(&searchInfo);

    SearchResults_t searchResults = NegamaxRoot(boardInfo, gameStack, zobristStack, &searchInfo, maxDepth);

    SendNumericalUciCommand("score cp", searchResults.score);

    return searchResults;
}