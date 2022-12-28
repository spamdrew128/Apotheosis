CC=gcc
INCDIRS=-I.
CFLAGS=-Wall -g $(INCDIRS)

COMMON_CFILES=bitboards.c lookup.c
COMMON_OBJECTS=bitboards.o lookup.o

MAIN=main
CFILES=$(MAIN).c $(COMMON_CFILES)
OBJECTS=$(MAIN).o $(COMMON_OBJECTS)

TDD=tdd
TDD_MAIN=$(TDD)/main_tdd
D_CFILES=$(TDD_MAIN).c $(COMMON_CFILES) $(TDD)/bitboards_tdd.c $(TDD)/lookup_tdd.c
D_OBJECTS=$(TDD_MAIN).o $(COMMON_OBJECTS) $(TDD)/bitboards_tdd.o $(TDD)/lookup_tdd.o

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
	-del -fR $(BINARY) $(DEBUG_BINARY) $(OBJECTS) $(D_OBJECTS)