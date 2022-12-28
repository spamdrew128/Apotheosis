CC=gcc
INCDIRS=-I.
CFLAGS=-Wall -g $(INCDIRS)

CFILES=bitboards.c lookup.c
OBJECTS=bitboards.o lookup.o

D_CFILES=bitboards_tdd.c lookup_tdd.c
D_OBJECTS=bitboards_tdd.o lookup_tdd.o

BINARY=bin
DEBUG_BINARY=debug_bin

MAIN=main.c
TDD_MAIN=tdd/main_tdd.c

all: $(BINARY) $(DEBUG_BINARY)

$(BINARY): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(DEBUG_BINARY): $(D_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -rf $(BINARY) $(DEBUG_BINARY) $(OBJECTS)