CC=gcc
CFLAGS=-Wall -Wextra  -std=gnu99 -g

BUILD_DIR = build


all: client server

client: client.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ $<

server: server.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ $<

runner_client: client 
	./$(BUILD_DIR)/client

runner_server: server
	./$(BUILD_DIR)/server


clean:
	rm -rf $(BUILD_DIR)