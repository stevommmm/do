# UNAMEM=$(shell uname -m | sed -e s/arm*/arm/)
CC = gcc
CFLAGS = -g -Wall -Wuninitialized -O1 -std=c11 -lcurl
ifdef SANATIZE_ADDRESS
	CFLAGS += -fsanitize=address -fno-omit-frame-pointer
endif
# ifeq ($(UNAMEM), arm)
# 	CFLAGS += -fsigned-char
# endif

BINARY = ok

all: $(BINARY)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BINARY): $(wildcard src/*.c)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean test

test: $(BINARY)
	@test/run.sh

clean:
	$(RM) */*.o *.o $(BINARY)
