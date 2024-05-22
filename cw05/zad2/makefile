CC: gcc
CFLAGS: -g -Wall -Wextra -std=c99

BUILD_DIR=build

all: sender catcher

sender: sender.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ $<

catcher: catcher.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ $<
PID:=0
SIGNAL:=1

test: sender catcher
	./$(BUILD_DIR)/catcher
	./$(BUILD_DIR)/sender $(PID) $(SIGNAL)

clean:
	rm -rf $(BUILD_DIR)
	rm -f sender catcher