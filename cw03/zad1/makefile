CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O0

.PHONY: all clean test

all: test_single test_thousand

reverse_single:
	$(CC) $(CFLAGS) -o reverse reverse.c -DSINGLE

reverse_thousand: 
	$(CC) $(CFLAGS) -o reverse reverse.c -DTHOUSAND


clean:
	rm -f reverse reverse.o tmp1 tmp2 tmp3 tmp4 time.txt

test_single: reverse_single
	./reverse plik_binarny tmp1
	./reverse tmp1 tmp2
	diff -s tmp2 plik_binarny

test_thousand: reverse_thousand
	./reverse plik_binarny tmp3
	./reverse tmp3 tmp4
	diff -s tmp4 plik_binarny