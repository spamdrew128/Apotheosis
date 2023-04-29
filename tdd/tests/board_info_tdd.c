#include <stdio.h>
#include <stdbool.h>

#include "debug.h"
#include "board_constants.h"
#include "bitboards.h"

void BoardInfoShouldInitCorrectly() {
    BoardInfo_t info;
    InitBoardInfo(&info);

    bool success = true;

    for(int i = 0; i < 2; i++) {
        success = success && info.allPieces[i] == EMPTY_SET;
        success = success && info.pawns[i] == EMPTY_SET;
        success = success && info.knights[i] == EMPTY_SET;
        success = success && info.bishops[i] == EMPTY_SET;
        success = success && info.rooks[i] == EMPTY_SET;
        success = success && info.queens[i] == EMPTY_SET;
        success = success && info.kings[i] == EMPTY_SET;
    }

    success = success && info.empty == EMPTY_SET;

    for(int i = 0; i < NUM_SQUARES; i++) {
        success = success && info.mailbox[i] == none_type;
    }

    PrintResults(success);
}

static void MailboxShouldUpdate() {
    BoardInfo_t info;
    InitBoardInfo(&info);
    info.knights[white] = CreateBitboard(2, e2, g7);
    info.queens[black] = CreateBitboard(1, a3);
    TranslateBitboardsToMailbox(&info);

    bool success = info.mailbox[e2] == knight && info.mailbox[a3] == queen;

    PrintResults(success);
}

void BoardInfoTDDRunner() {
    BoardInfoShouldInitCorrectly();
    MailboxShouldUpdate();
}