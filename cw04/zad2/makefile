CC: gcc
CFLAGS: -g -Wall -Wextra -std=c99

all: error_handler

error_handler: error_handler.c
	mkdir -p bin
	$(CC) $(CFLAGS) -o bin/$@ $<

clean:
	rm -rf bin test_dir 
test: error_handler
	mkdir -p test_dir
	touch test_dir/test_file1
	touch test_dir/test_file2
	./bin/error_handler test_dir/test_file1 
	@echo "Return code: $$?"
	./bin/error_handler test_dir/sfsdfs
	@echo "Return code: $$?"