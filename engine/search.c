#include "search.h"
#include "movegen.h"
#include "make_and_unmake.h"
#include "RNG.h"

static SearchResults_t DummySearch(
    MoveList_t* moveList,     
    BoardInfo_t* boardInfo,
    GameStack_t* gameStack,
    ZobristStack_t* zobristStack
)
{
    Color_t color = boardInfo->colorToMove;

    SearchResults_t results;
    results.score = color == white ? -EVAL_MAX : EVAL_MAX;

    for(int i = 0; i <= moveList->maxIndex; i++) {
        Move_t move = moveList->moves[i];
        MakeMove(boardInfo, gameStack, move);
        EvalScore_t score = ScoreOfPosition(boardInfo);

        if(color == white && score > results.score) {
            results.score = score;
            results.bestMove = move;
        } else if(color == black && score < results.score) {
            results.score = score;
            results.bestMove = move;
        }

        UnmakeMove(boardInfo, gameStack);
    }

    return results;
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

    SearchResults_t results = DummySearch(&moveList, boardInfo, gameStack, zobristStack);

    return results;
}
