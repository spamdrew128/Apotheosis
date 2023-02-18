#include <stdint.h>

#include "search.h"
#include "movegen.h"
#include "make_and_unmake.h"
#include "RNG.h"

typedef uint8_t Depth_t;

static SearchResults_t NegamaxRoot(
    MoveList_t* moveList,     
    BoardInfo_t* boardInfo,
    GameStack_t* gameStack,
    ZobristStack_t* zobristStack,
    Depth_t depth
)
{
    Move_t bestMove;
    Centipawns_t bestScore = -EVAL_MAX;

    CompleteMovegen(moveList, boardInfo, gameStack);
    for(int i = 0; i <= moveList->maxIndex; i++) {
        Move_t move = moveList->moves[i];
        MakeMove(boardInfo, gameStack, move);
    }
}

SearchResults_t Search(
    PlayerTimeInfo_t uciTimeInfo,
    BoardInfo_t* boardInfo,
    GameStack_t* gameStack,
    ZobristStack_t* zobristStack
)
{
    MoveList_t moveList;
    CompleteMovegen(&moveList, boardInfo, gameStack);

    SearchResults_t results = NegamaxRoot(&moveList, boardInfo, gameStack, zobristStack, 3);

    return results;
}
