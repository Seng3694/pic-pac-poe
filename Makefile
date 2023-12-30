CC=gcc
MKDIR=mkdir
RM=rm -f
CP=cp -r

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

install: $(BIN)/ppp
	$(CP) $(BIN)/ppp /usr/local/bin

uninstall:
	$(RM) /usr/local/bin/ppp

clean:
	$(RM) $(BIN)/*

.PHONY: clean
