CC = gcc
CFLAGS = -Wall
BUILD_DIR = build

all: life
	
life: grid.c life.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) grid.c life.c -o $(BUILD_DIR)life -lncursesw -lpthread -lm
	
run: life
	./$(BUILD_DIR)life


clean:
	rm -rf $(BUILD_DIR)
