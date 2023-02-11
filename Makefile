CC=gcc
INCDIRS=-I.

DEBUGFLAGS=-g
OPTFLAGS=-O3 -flto
CFLAGS=-Wall -std=c17 -march=native $(DEBUGFLAGS) $(INCDIRS) 

COMMON_CFILES= \
bitboards.c \
board_info.c \
lookup.c \
FEN.c \
pieces.c \
magic.c \
RNG.c \
legals.c \
movegen.c \
game_state.c \
move.c \
make_and_unmake.c \
castling.c

COMMON_OBJECTS= \
bitboards.o \
board_info.o \
lookup.o \
FEN.o \
pieces.o \
magic.o \
RNG.o \
legals.o \
movegen.o \
game_state.o \
move.o \
make_and_unmake.o \
castling.o

MAIN=main
CFILES=$(MAIN).c $(COMMON_CFILES)
OBJECTS=$(MAIN).o $(COMMON_OBJECTS)

TDD=tdd
TDD_MAIN=$(TDD)\main_tdd

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
$(TDD)\perft_table.c

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
$(TDD)\perft_table.o

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
	del $(BINARY).exe $(DEBUG_BINARY).exe $(OBJECTS) $(D_OBJECTS)