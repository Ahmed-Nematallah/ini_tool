
CC=gcc
CFLAGS=-O3 -Wall -Wextra -Wfloat-equal -Wshadow -Wformat=2 \
       -Wuninitialized -Wpointer-arith -Wcast-align \
	   -Wstrict-prototypes -Wwrite-strings -Waggregate-return \
	   -Wcast-qual -Wswitch-default -Wswitch-enum \
	   -Wunreachable-code -std=c99 -pedantic \
	   -Wunused -Winvalid-pch -Wlogical-op -Wno-overlength-strings \
	   -Wno-pedantic-ms-format
SRCDIR=src
OBJDIR=obj
BINDIR=bin
DEPS=$(SRCDIR)/*.h

SOURCES=$(shell find $(SRCDIR)/ -name "*.c")
OBJECTS=$(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

all: $(BINDIR)/ini_tool

$(BINDIR)/ini_tool: $(SOURCES) $(RESOURCE_OBJECTS)
	$(CC) $(SOURCES) $(RESOURCE_OBJECTS) -o $(BINDIR)/ini_tool $(CFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -rf $(BINDIR)/* $(OBJDIR)/*
