CC=gcc

DEBUGFLAGS=-g
OPTFLAGS=-O3 -flto
SAN=N
CFLAGS=-Wall -std=c17 -march=native $(OPTFLAGS)
CPPFLAGS=$(INCDIRS)

RELEASE=N

ifneq (,$(findstring clang,$(shell $(CXX) --version)))
	CFLAGS += -fuse-ld=lld
endif

ifeq ($(RELEASE), Y)
	CFLAGS += -DNDEBUG
endif

ifeq ($(SAN), Y)
	CFLAGS += -fsanitize=address
endif

ifeq ($(OS),Windows_NT)
	include windows.mk
else
	include linux.mk
endif