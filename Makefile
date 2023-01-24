CC=gcc
INCDIRS=-I.
CFLAGS=-Wall -g -std=c99 $(INCDIRS)

COMMON_CFILES= \
bitboards.c \
lookup.c \
FEN.c \
pieces.c \
magic.c \
RNG.c \
legals.c \
movegen.c \
game_state.c

COMMON_OBJECTS= \
bitboards.o \
lookup.o \
FEN.o \
pieces.o \
magic.o \
RNG.o \
legals.o \
movegen.o \
game_state.o

MAIN=main
CFILES=$(MAIN).c $(COMMON_CFILES)
OBJECTS=$(MAIN).o $(COMMON_OBJECTS)

TDD=tdd
TDD_MAIN=$(TDD)\main_tdd

D_CFILES= \
$(TDD_MAIN).c \
$(COMMON_CFILES) \
$(TDD)\bitboards_tdd.c \
$(TDD)\lookup_tdd.c \
$(TDD)\debug.c \
$(TDD)\FEN_tdd.c \
$(TDD)\pieces_tdd.c \
$(TDD)\magic_tdd.c \
$(TDD)\legals_tdd.c \
$(TDD)\movegen_tdd.c \
$(TDD)\game_state_tdd.c

D_OBJECTS= \
$(TDD_MAIN).o \
$(COMMON_OBJECTS) \
$(TDD)\bitboards_tdd.o \
$(TDD)\lookup_tdd.o \
$(TDD)\debug.o \
$(TDD)\FEN_tdd.o \
$(TDD)\pieces_tdd.o \
$(TDD)\magic_tdd.o \
$(TDD)\legals_tdd.o \
$(TDD)\movegen_tdd.o \
$(TDD)\game_state_tdd.o

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