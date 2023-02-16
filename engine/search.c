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
    
    SearchResults_t results;
    results.bestMove = moveList.moves[moveList.maxIndex];
    results.score = 0;

    return results;
}
