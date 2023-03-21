CC=gcc

DEBUGFLAGS=-g -fsanitize=address
OPTFLAGS=-O3 -flto
CFLAGS=-Wall -std=c17 -march=native $(DEBUGFLAGS)
CPPFLAGS=$(INCDIRS)

RELEASE=false

ifeq ($(RELEASE), Y)
	CFLAGS += -DNDEBUG
endif

ifeq ($(OS),Windows_NT)
	include windows.mk
else
	include linux.mk
endif