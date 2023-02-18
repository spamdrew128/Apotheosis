#include <stdint.h>

#include "search.h"
#include "movegen.h"
#include "make_and_unmake.h"
#include "RNG.h"
#include "endings.h"

typedef uint8_t Depth_t;

static EvalScore_t NegamaxHelper(
    BoardInfo_t* boardInfo,
    GameStack_t* gameStack,
    ZobristStack_t* zobristStack,
    Depth_t depth
)
{
    if(depth == 0) {
        return ScoreOfPosition(boardInfo);
    }

    EvalScore_t bestScore = -EVAL_MAX;
    
    MoveList_t moveList;
    CompleteMovegen(&moveList, boardInfo, gameStack);
    for(int i = 0; i <= moveList.maxIndex; i++) {
        Move_t move = moveList.moves[i];
        MakeMove(boardInfo, gameStack, move);

        EvalScore_t score = -NegamaxHelper(boardInfo, gameStack, zobristStack, depth-1);

        UnmakeMove(boardInfo, gameStack);

        if(score > bestScore) {
            bestScore = score;
        }
    }

    return bestScore;
}

static SearchResults_t NegamaxRoot(  
    BoardInfo_t* boardInfo,
    GameStack_t* gameStack,
    ZobristStack_t* zobristStack,
    Depth_t depth
)
{
    Move_t bestMove;
    EvalScore_t bestScore = -EVAL_MAX;
    
    MoveList_t moveList;
    CompleteMovegen(&moveList, boardInfo, gameStack);
    for(int i = 0; i <= moveList.maxIndex; i++) {
        Move_t move = moveList.moves[i];
        MakeMove(boardInfo, gameStack, move);

        EvalScore_t score = -NegamaxHelper(boardInfo, gameStack, zobristStack, depth-1);

        UnmakeMove(boardInfo, gameStack);

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

SearchResults_t Search(
    PlayerTimeInfo_t uciTimeInfo,
    BoardInfo_t* boardInfo,
    GameStack_t* gameStack,
    ZobristStack_t* zobristStack
)
{
    SearchResults_t results = NegamaxRoot(boardInfo, gameStack, zobristStack, 3);

    return results;
}
