#include "killers.h"

void InitKillers(Killers_t* killers) {
    for(int i = 0; i < KILLERS_LENGTH; i++) {
        for(int j = 0; j < KILLERS_WIDTH; j++) {
            InitMove(&(killers->moves[i][j]));
        }
    }
}