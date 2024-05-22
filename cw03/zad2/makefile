CC := gcc
CFLAGS := -Wall -Wextra -std=c99 -g

.PHONY: all clean test

all: size

size: size.o
	$(CC) $(CFLAGS) -o size size.o

size.o: size.c
	$(CC) $(CFLAGS) -c -o size.o size.c

clean:
	rm -f size size.o  result.txt

test: size
	./size
