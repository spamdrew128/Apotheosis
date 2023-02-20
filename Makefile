CC=gcc

SRC=src
BITBOARDS=$(SRC)\bitboards
ENDINGS=$(SRC)\endings
ENGINE=$(SRC)\engine
FEN=$(SRC)\fen
LOOKUP=$(SRC)\lookup
MOVEGEN=$(SRC)\movegen
PLAY=$(SRC)\play
RNG=$(SRC)\RNG
STATE=$(SRC)\state
TIMER=$(SRC)\timer
UCI=$(SRC)\UCI
ZOBRIST=$(SRC)\zobrist

TDD_ROOT=tdd
TDD=$(TDD_ROOT)\tests
ENGINE_TDD=$(TDD_ROOT)\engine_tests

INCDIRS:= \
-I . \
-I $(SRC)\. \
-I $(BITBOARDS)\. \
-I $(ENDINGS)\. \
-I $(ENGINE)\. \
-I $(FEN)\. \
-I $(LOOKUP)\. \
-I $(MOVEGEN)\. \
-I $(PLAY)\. \
-I $(RNG)\. \
-I $(STATE)\. \
-I $(TIMER)\. \
-I $(UCI)\. \
-I $(ZOBRIST)\. \
\
-I $(TDD_ROOT)\. \
-I $(TDD)\. \
-I $(ENGINE_TDD)\. 

DEBUGFLAGS=-g
OPTFLAGS=-O3 -flto
CFLAGS=-Wall -std=c17 -march=native $(OPTFLAGS) $(INCDIRS) 

COMMON_CFILES= \
$(BITBOARDS)\bitboards.c \
$(BITBOARDS)\magic.c \
$(ENDINGS)\endings.c \
$(ENGINE)\chess_search.c \
$(ENGINE)\evaluation.c \
$(ENGINE)\PV_table.c \
$(FEN)\FEN.c \
$(LOOKUP)\lookup.c \
$(PLAY)\move.c \
$(PLAY)\make_and_unmake.c \
$(RNG)\RNG.c \
$(STATE)\board_info.c \
$(STATE)\game_state.c \
$(TIMER)\timer.c \
$(MOVEGEN)\legals.c \
$(MOVEGEN)\movegen.c \
$(MOVEGEN)\pieces.c \
$(UCI)\UCI.c \
$(ZOBRIST)\zobrist.c

COMMON_OBJECTS= \
$(BITBOARDS)\bitboards.o \
$(BITBOARDS)\magic.o \
$(ENDINGS)\endings.o \
$(ENGINE)\chess_search.o \
$(ENGINE)\evaluation.o \
$(ENGINE)\PV_table.o \
$(FEN)\FEN.o \
$(LOOKUP)\lookup.o \
$(PLAY)\move.o \
$(PLAY)\make_and_unmake.o \
$(RNG)\RNG.o \
$(STATE)\board_info.o \
$(STATE)\game_state.o \
$(TIMER)\timer.o \
$(MOVEGEN)\legals.o \
$(MOVEGEN)\movegen.o \
$(MOVEGEN)\pieces.o \
$(UCI)\UCI.o \
$(ZOBRIST)\zobrist.o

MAIN=main
CFILES=$(MAIN).c $(COMMON_CFILES)
OBJECTS=$(MAIN).o $(COMMON_OBJECTS)

TDD_MAIN=$(TDD_ROOT)\main_tdd

D_CFILES= \
$(TDD_MAIN).c \
$(COMMON_CFILES) \
$(TDD_ROOT)\debug.c \
$(TDD)\bitboards_tdd.c \
$(TDD)\board_info_tdd.c \
$(TDD)\lookup_tdd.c \
$(TDD)\FEN_tdd.c \
$(TDD)\pieces_tdd.c \
$(TDD)\magic_tdd.c \
$(TDD)\legals_tdd.c \
$(TDD)\movegen_tdd.c \
$(TDD)\game_state_tdd.c \
$(TDD)\make_and_unmake_tdd.c \
$(TDD)\recursive_testing.c \
$(TDD)\perft_table.c \
$(TDD)\zobrist_tdd.c \
$(TDD)\endings_tdd.c \
\
$(ENGINE_TDD)\basic_tests.c \
$(ENGINE_TDD)\PV_table_tdd.c

D_OBJECTS= \
$(TDD_MAIN).o \
$(COMMON_OBJECTS) \
$(TDD_ROOT)\debug.o \
$(TDD)\bitboards_tdd.o \
$(TDD)\board_info_tdd.o \
$(TDD)\lookup_tdd.o \
$(TDD)\FEN_tdd.o \
$(TDD)\pieces_tdd.o \
$(TDD)\magic_tdd.o \
$(TDD)\legals_tdd.o \
$(TDD)\movegen_tdd.o \
$(TDD)\game_state_tdd.o \
$(TDD)\make_and_unmake_tdd.o \
$(TDD)\recursive_testing.o \
$(TDD)\perft_table.o \
$(TDD)\zobrist_tdd.o \
$(TDD)\endings_tdd.o \
\
$(ENGINE_TDD)\basic_tests.o \
$(ENGINE_TDD)\PV_table_tdd.o

BINARY=bin
DEBUG_BINARY=debug

all: $(BINARY) $(DEBUG_BINARY)

test: $(DEBUG_BINARY)
	$(DEBUG_BINARY).exe

$(BINARY): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(DEBUG_BINARY): $(D_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	del /S *.exe *.o && cls