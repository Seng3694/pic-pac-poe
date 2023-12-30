CC=gcc
MKDIR=mkdir
RM=rm -f

CFLAGS=-g -O0 -Wall -std=c99 -fsanitize=undefined -fsanitize=address

ifdef release
CFLAGS=-O2 -Wall -std=c99 -DNDEBUG
endif

BIN=bin
SRC=src

SOURCES=$(wildcard $(SRC)/*.c)
OBJS=$(patsubst $(SRC)/%.c, $(BIN)/%.o, $(SOURCES))

$(BIN)/ppp: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ -llua -lm

$(BIN)/%.o: $(SRC)/%.c | $(BIN) 
	$(CC) -c $(CFLAGS) $< -o $@

$(BIN):
	$(MKDIR) $(BIN)

clean:
	$(RM) $(BIN)/*

.PHONY: clean
