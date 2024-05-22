CC: gcc
CFLAGS: -g -Wall -Wextra -std=c99

all: creator

creator: creator.c
	$(CC) $(CFLAGS) -o creator creator.c
clean:
	rm -f creator

test: creator
	./creator 5
