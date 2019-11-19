CC = gcc
CFLAGS = -g -Wall -Wuninitialized -O1
ifdef SANATIZE_ADDRESS
	CFLAGS += -fsanitize=address -fno-omit-frame-pointer
endif
BINARY = ok

all: $(BINARY)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BINARY): $(wildcard *.c)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean

clean:
	$(RM) */*.o *.o $(BINARY)
