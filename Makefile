CC	   = gcc
CFLAGS = -g -Wall
RM	   = rm -f
SRCDIR = ./src
BINDIR = ./bin

TARGETS = $(basename $(notdir $(wildcard src/*.c)))


all: $(TARGETS)

bin:
	mkdir -p $(BINDIR)

run_day_%: day_%
	%(BINDIR)/$@

day_%: $(SRCDIR)/day_%.c bin
	@$(CC) $(CFLAGS) -o $(BINDIR)/$@ $(word 1, $^)
	@echo "$@ compiled"

clean:
	$(RM) $(BINDIR)/*
