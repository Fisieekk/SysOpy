CC=gcc
CFLAGS=-Wall -g -pthread
BUILD_DIR=build

all: santa

santa: santa.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ $^

run: santa
	./$(BUILD_DIR)/santa

clean:
	rm -rf $(BUILD_DIR)