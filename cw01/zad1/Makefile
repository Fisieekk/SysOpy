CXX=gcc
CFLAGS = -Wall -std=c17 -Wextra -g

.PHONY: all countdown clean

all: countdown

countdown: countdown.c
	$(CXX) $(CFLAGS) -o countdown countdown.c

clean:
	rm -f countdown