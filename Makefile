CC = gcc
CFLAGS = -g -Wall -Wuninitialized -O1 -std=c11 -lcurl
ifdef SANATIZE_ADDRESS
	CFLAGS += -fsanitize=address -fno-omit-frame-pointer
endif
OBJECTS=$(patsubst src/%.h,src/%.o, $(wildcard src/*.h))
BINARY = do-run

all: $(BINARY)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BINARY): src/main.c $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean test

test: $(BINARY)
	./$(BINARY) test/*.do

syntax:
	cp do.sublime-syntax ~/.config/sublime-text-3/Packages/User/

clean:
	$(RM) */*.o *.o $(BINARY)
