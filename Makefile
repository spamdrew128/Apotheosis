CC=gcc

SRC=src
BITBOARDS=$(SRC)\bitboards
ENGINE=$(SRC)\engine
PLAY=$(SRC)\play
STATE=$(SRC)\state

TDD_ROOT=tdd
TDD=$(TDD_ROOT)\tests

INCDIRS:= \
-I . \
-I $(SRC)\. \
-I $(BITBOARDS)\. \
-I $(ENGINE)\. \
-I $(PLAY)\. \
-I $(STATE)\. \
\
-I $(TDD)\. 

DEBUGFLAGS=-g
OPTFLAGS=-O3 -flto
CFLAGS=-Wall -std=c17 -march=native $(DEBUGFLAGS) $(INCDIRS) 

COMMON_CFILES= \
$(BITBOARDS)\bitboards.c \
$(BITBOARDS)\magic.c \
$(ENGINE)\search.c \
$(ENGINE)\evaluation.c \
$(PLAY)\move.c \
$(PLAY)\make_and_unmake.c \
$(STATE)\board_info.c \
$(STATE)\game_state.c \
$(SRC)\lookup.c \
$(SRC)\FEN.c \
$(SRC)\pieces.c \
$(SRC)\RNG.c \
$(SRC)\legals.c \
$(SRC)\movegen.c \
$(SRC)\zobrist.c \
$(SRC)\UCI.c \
$(SRC)\endings.c 

COMMON_OBJECTS= \
$(BITBOARDS)\bitboards.o \
$(BITBOARDS)\magic.o \
$(ENGINE)\search.o \
$(ENGINE)\evaluation.o \
$(PLAY)\move.o \
$(PLAY)\make_and_unmake.o \
$(STATE)\board_info.o \
$(STATE)\game_state.o \
$(SRC)\lookup.o \
$(SRC)\FEN.o \
$(SRC)\pieces.o \
$(SRC)\RNG.o \
$(SRC)\legals.o \
$(SRC)\movegen.o \
$(SRC)\zobrist.o \
$(SRC)\UCI.o \
$(SRC)\endings.o

MAIN=main
CFILES=$(MAIN).c $(COMMON_CFILES)
OBJECTS=$(MAIN).o $(COMMON_OBJECTS)

TDD_MAIN=$(TDD_ROOT)\main_tdd

D_CFILES= \
$(TDD_MAIN).c \
$(COMMON_CFILES) \
$(TDD)\bitboards_tdd.c \
$(TDD)\board_info_tdd.c \
$(TDD)\lookup_tdd.c \
$(TDD)\debug.c \
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
$(TDD)\UCI_tdd.c \
$(TDD)\endings_tdd.c

D_OBJECTS= \
$(TDD_MAIN).o \
$(COMMON_OBJECTS) \
$(TDD)\bitboards_tdd.o \
$(TDD)\board_info_tdd.o \
$(TDD)\lookup_tdd.o \
$(TDD)\debug.o \
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
$(TDD)\UCI_tdd.o \
$(TDD)\endings_tdd.o

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