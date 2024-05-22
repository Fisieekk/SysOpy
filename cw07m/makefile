CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -g

BUILD_DIR = build

all: client server

client: client.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/client client.c

server: server.c
	rm -rf /dev/mqueue/*
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/server server.c


runner_server: server
	./$(BUILD_DIR)/server

runner_client: client
	./$(BUILD_DIR)/client

clean:
	rm -rf $(BUILD_DIR)
	rm -rf /dev/mqueue/*