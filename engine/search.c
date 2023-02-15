#include "search.h"
#include "movegen.h"
#include "make_and_unmake.h"
#include "RNG.h"

SearchResults_t Search(
    PlayerTimeInfo_t uciTimeInfo,
    BoardInfo_t* boardInfo,
    GameStack_t* gameStack,
    ZobristStack_t* zobristStack,
    Color_t color
)
{
    MoveList_t moveList;
    CompleteMovegen(&moveList, boardInfo, gameStack, color);
    int randIndex = RandUnsigned64() % (moveList.maxIndex + 1);
    
    SearchResults_t results;
    results.bestMove = moveList.moves[randIndex];
    results.score = 0;

    return results;
}
