CC=gcc
INCDIRS=-I.
CFLAGS=-Wall -g $(INCDIRS)

COMMON_CFILES=bitboards.c lookup.c
COMMON_OBJECTS=bitboards.o lookup.o

MAIN=main
CFILES=$(MAIN).c $(COMMON_CFILES)
OBJECTS=$(MAIN).o $(COMMON_OBJECTS)

TDD_MAIN=main_tdd
D_CFILES=$(TDD_MAIN).c $(COMMON_CFILES) bitboards_tdd.c lookup_tdd.c
D_OBJECTS=$(TDD_MAIN).o $(COMMON_OBJECTS) bitboards_tdd.o lookup_tdd.o

BINARY=bin
DEBUG_BINARY=debug_bin

all: $(BINARY) $(DEBUG_BINARY)

$(BINARY): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(DEBUG_BINARY): $(D_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -rf $(BINARY) $(DEBUG_BINARY) $(OBJECTS)