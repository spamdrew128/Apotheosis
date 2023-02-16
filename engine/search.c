#include "search.h"
#include "movegen.h"
#include "make_and_unmake.h"
#include "RNG.h"

static SearchResults_t DummySearch(
    MoveList_t* moveList,     
    BoardInfo_t* boardInfo,
    GameStack_t* gameStack,
    ZobristStack_t* zobristStack,
    Color_t colorToMove
)
{
    SearchResults_t results;
    results.score = colorToMove == white ? -EVAL_MAX : EVAL_MAX;

    for(int i = 0; i <= moveList->maxIndex; i++) {
        Move_t move = moveList->moves[i];
        MakeMove(boardInfo, gameStack, move, colorToMove);
        EvalScore_t score = ScoreOfPosition(boardInfo);

        if(colorToMove == white && score > results.score) {
            results.score = score;
            results.bestMove = move;
        } else if(colorToMove == black && score < results.score) {
            results.score = score;
            results.bestMove = move;
        }
    }

    return results;
}

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

    SearchResults_t results = DummySearch(&moveList, boardInfo, gameStack, zobristStack, color);

    return results;
}
