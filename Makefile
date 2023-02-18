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

TDD_ROOT=tdd
TDD=$(TDD_ROOT)\tests

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
\
-I $(TDD)\. 

DEBUGFLAGS=-g
OPTFLAGS=-O3 -flto
CFLAGS=-Wall -std=c17 -march=native $(DEBUGFLAGS) $(INCDIRS) 

COMMON_CFILES= \
$(BITBOARDS)\bitboards.c \
$(BITBOARDS)\magic.c \
$(ENDINGS)\endings.c \
$(ENGINE)\evaluation.c \
$(ENGINE)\search.c \
$(FEN)\FEN.c \
$(LOOKUP)\lookup.c \
$(PLAY)\move.c \
$(PLAY)\make_and_unmake.c \
$(RNG)\RNG.c \
$(STATE)\board_info.c \
$(STATE)\game_state.c \
$(MOVEGEN)\legals.c \
$(MOVEGEN)\movegen.c \
$(MOVEGEN)\pieces.c \
$(SRC)\zobrist.c \
$(SRC)\UCI.c

COMMON_OBJECTS= \
$(BITBOARDS)\bitboards.o \
$(BITBOARDS)\magic.o \
$(ENDINGS)\endings.o \
$(ENGINE)\evaluation.o \
$(ENGINE)\search.o \
$(FEN)\FEN.o \
$(LOOKUP)\lookup.o \
$(PLAY)\move.o \
$(PLAY)\make_and_unmake.o \
$(RNG)\RNG.o \
$(STATE)\board_info.o \
$(STATE)\game_state.o \
$(MOVEGEN)\legals.o \
$(MOVEGEN)\movegen.o \
$(MOVEGEN)\pieces.o \
$(SRC)\zobrist.o \
$(SRC)\UCI.o 

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