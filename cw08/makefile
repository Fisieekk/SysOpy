CC: gcc
CFLAGS: -std=c11 -Wextra -Wall -g -O0

BUILD_DIR = build

.PHONY: all clean

all: printer,user

printer: printer.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/printer printer.c -lrt -pthread

user: user.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/user user.c -lrt -pthread

runner_printer: printer
	./$(BUILD_DIR)/printer 2

runner_user: user
	./$(BUILD_DIR)/user 3


clean:
	rm -rf $(BUILD_DIR)