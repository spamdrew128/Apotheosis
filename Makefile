CC=gcc

SRC=src
BENCH=bench

DEBUGFLAGS=-g
OPTFLAGS=-O3 -flto
CFLAGS=-Wall -std=c17 -march=native $(OPTFLAGS)
CPPFLAGS=$(INCDIRS)

ifeq ($(OS),Windows_NT)
include windows.mk
else
include linux.mk
endif