#include "PV_table_tdd.h"
#include "debug.h"

// assumes white plays first
static EvalScore_t NegamaxFakeout(
    PvTable_t* pvTable,
    Depth_t depth,
    Ply_t ply,
    int branchingFactor,
    int* leafCount,
    int* evaluations
)
{
    if(depth == 0) {
        int eval = evaluations[*leafCount];
        (*leafCount)++;

        bool isBlack = ply % 2;
        return isBlack ? -eval : eval;
    }

    EvalScore_t bestScore = -EVAL_MAX;
    for(uint16_t i = 0; i < branchingFactor; i++) {
        Move_t move;
        move.data = i;
    
        EvalScore_t score =
            -NegamaxFakeout(
                pvTable,
                depth-1,
                ply+1,
                branchingFactor,
                leafCount,
                evaluations
            );

        if(score > bestScore) {
            bestScore = score;
            UpdatePvTable(pvTable, move, ply);
        }
    }

    return bestScore;
}

static bool PvMatchesExpectedPath(PvTable_t* pvTable, uint16_t* expectedPathFromRoot) {
    for(int i = 0; i < pvTable->maxPly; i++) {
        if(pvTable->moveMatrix[0][i].data != expectedPathFromRoot[i]) {
            return false;
        }
    }

    return true;
}

static bool FindsCorrectPv(
    Depth_t depth,
    int branchingFactor,
    int* evaluations,
    uint16_t* expectedPathFromRoot
)
{
    PvTable_t pvTable;
    PvTableInit(&pvTable, depth);
    int leafCount = 0;

    NegamaxFakeout(
        &pvTable,
        depth,
        0,
        branchingFactor,
        &leafCount,
        evaluations
    );

    bool success = PvMatchesExpectedPath(&pvTable, expectedPathFromRoot);

    PvTableTeardown(&pvTable);

    return success;
}

static void ShouldFindCorrectPv() {
    int evaluations[] = { -2, 2, 1, 3, -3, 1, -4, -2 };
    uint16_t expectedPathFromRoot[] = { 0, 0, 1 };
    bool success = 
        FindsCorrectPv(
            3,
            2,
            evaluations,
            expectedPathFromRoot
        );

    PrintResults(success);
}

void PvTableTDDRunner() {
    ShouldFindCorrectPv();
}