CC	   = gcc
CFLAGS = -g -Wall
RM	   = rm -f
SRCDIR = ./src
BINDIR = ./bin

TARGETS = $(basename $(notdir $(wildcard src/day_*.c)))

all: $(TARGETS)

bin:
	mkdir -p $(BINDIR)

day_%: $(SRCDIR)/day_%.c bin
	@$(CC) $(CFLAGS) -o $(BINDIR)/$@ $(word 1, $^)
	@echo "$@ compiled"

clean:
	$(RM) $(BINDIR)/*
