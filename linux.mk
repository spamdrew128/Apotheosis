SRC=src
BENCH=bench
TDD_ROOT=tdd

BITBOARDS=$(SRC)/bitboards
ENDINGS=$(SRC)/endings
ENGINE=$(SRC)/engine
EVAL=$(ENGINE)/evaluation
FEN=$(SRC)/FEN
LOOKUP=$(SRC)/lookup
MOVEGEN=$(SRC)/movegen
PLAY=$(SRC)/play
RNG=$(SRC)/RNG
STATE=$(SRC)/state
TIMER=$(SRC)/timer
UCI=$(SRC)/UCI
ZOBRIST=$(SRC)/zobrist

TDD=$(TDD_ROOT)/tests
ENGINE_TDD=$(TDD_ROOT)/engine_tests

TUNING=tuning

MAIN=main
TDD_MAIN=$(TDD_ROOT)/main_tdd

INCDIRS:= \
-I . \
-I $(SRC)/. \
-I $(BITBOARDS)/. \
-I $(ENDINGS)/. \
-I $(ENGINE)/. \
-I $(EVAL)/. \
-I $(FEN)/. \
-I $(LOOKUP)/. \
-I $(MOVEGEN)/. \
-I $(PLAY)/. \
-I $(RNG)/. \
-I $(STATE)/. \
-I $(TIMER)/. \
-I $(UCI)/. \
-I $(ZOBRIST)/. \
\
-I $(TDD_ROOT)/. \
-I $(TDD)/. \
-I $(ENGINE_TDD)/. \
\
-I $(TUNING)/.

COMMON_CFILES= \
$(SRC)/string_utils.c \
$(BITBOARDS)/bitboards.c \
$(BITBOARDS)/magic.c \
$(ENDINGS)/endings.c \
$(ENGINE)/chess_search.c \
$(ENGINE)/killers.c \
$(ENGINE)/PV_table.c \
$(ENGINE)/move_ordering.c \
$(ENGINE)/transposition_table.c \
$(ENGINE)/history.c \
$(EVAL)/eval_helpers.c \
$(EVAL)/eval_constants_tools.c \
$(EVAL)/evaluation.c \
$(EVAL)/pawn_structure.c \
$(EVAL)/board_control.c \
$(FEN)/FEN.c \
$(LOOKUP)/lookup.c \
$(PLAY)/move.c \
$(PLAY)/make_and_unmake.c \
$(RNG)/RNG.c \
$(STATE)/board_info.c \
$(STATE)/game_state.c \
$(TIMER)/timer.c \
$(MOVEGEN)/legals.c \
$(MOVEGEN)/movegen.c \
$(MOVEGEN)/pieces.c \
$(UCI)/UCI.c \
$(ZOBRIST)/zobrist.c \
\
$(TUNING)/tuner.c \
$(TUNING)/datagen.c

COMMON_OBJECTS=$(COMMON_CFILES:%.c=%.o)

CFILES=$(MAIN).c $(BENCH).c $(COMMON_CFILES)
OBJECTS=$(MAIN).o $(BENCH).o $(COMMON_OBJECTS)

D_CFILES= \
$(TDD_MAIN).c \
$(COMMON_CFILES) \
$(TDD_ROOT)/debug.c \
$(TDD)/bitboards_tdd.c \
$(TDD)/board_info_tdd.c \
$(TDD)/lookup_tdd.c \
$(TDD)/FEN_tdd.c \
$(TDD)/pieces_tdd.c \
$(TDD)/magic_tdd.c \
$(TDD)/legals_tdd.c \
$(TDD)/movegen_tdd.c \
$(TDD)/game_state_tdd.c \
$(TDD)/make_and_unmake_tdd.c \
$(TDD)/recursive_testing.c \
$(TDD)/perft_table.c \
$(TDD)/zobrist_tdd.c \
$(TDD)/endings_tdd.c \
\
$(ENGINE_TDD)/basic_tests.c \
$(ENGINE_TDD)/PV_table_tdd.c \
$(ENGINE_TDD)/random_crashes.c \
$(ENGINE_TDD)/move_ordering_tdd.c \
$(ENGINE_TDD)/TT_tdd.c \
$(ENGINE_TDD)/killers_tdd.c \
$(ENGINE_TDD)/history_tdd.c \
$(ENGINE_TDD)/pawn_structure_tdd.c \
$(ENGINE_TDD)/board_control_tdd.c

D_OBJECTS=$(D_CFILES:%.c=%.o)

EXE=bin
DEBUG_EXE=debug

all: $(EXE) $(DEBUG_EXE)

test: $(DEBUG_EXE)
	$(DEBUG_EXE)

$(EXE): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ -lm	

$(DEBUG_EXE): $(D_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ -lm

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $^

clean:
	rm $(EXE) $(DEBUG_EXE) $(OBJECTS) $(D_OBJECTS) *bin *txt