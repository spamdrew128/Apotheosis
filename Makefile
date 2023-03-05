CC=gcc

DEBUGFLAGS=-g
OPTFLAGS=-O3 -flto
CFLAGS=-Wall -std=c17 -march=native $(DEBUGFLAGS)
CPPFLAGS=$(INCDIRS)

RELEASE=false

ifeq ($(RELEASE), y)
	CFLAGS += -DNDEBUG
endif

ifeq ($(OS),Windows_NT)
	include windows.mk
else
	include linux.mk
endif