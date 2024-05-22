CC: gcc
CFLAGS: -g -Wall -Wextra -std=c99

BUILD_DIR=build

.PHONY: all clean


all: reactioner

reactioner: reactioner.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ $<

clean:
	rm -rf $(BUILD_DIR)
	rm -f reactioner

test: reactioner
	./$(BUILD_DIR)/reactioner ignore
	./$(BUILD_DIR)/reactioner handler
	./$(BUILD_DIR)/reactioner mask
	./$(BUILD_DIR)/reactioner none