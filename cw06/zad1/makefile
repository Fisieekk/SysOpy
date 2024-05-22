CC = gcc
CCFLAGS = -Wall -std=c99 -g
BUILDDIR = build
TIME = /usr/bin/time -f "%e"
OUTPUT_FILE = results.txt

all: riemman

riemman: Riemann.c
	mkdir -p $(BUILDDIR)
	$(CC) $(CCFLAGS) -o $(BUILDDIR)/$@ $< -lm

test: riemman
	rm -f $(OUTPUT_FILE)
	@echo "30 processes   " `$(TIME) -o $(OUTPUT_FILE) -a  ./$(BUILDDIR)/riemman 0.0000000001 30 2>&1 | tee -a $(OUTPUT_FILE)` "[s]"
	@echo "20 processes   " `$(TIME) -o $(OUTPUT_FILE) -a ./$(BUILDDIR)/riemman 0.0000000001 20 2>&1 | tee -a $(OUTPUT_FILE)` "[s]"
	@echo "10 processes   " `$(TIME) -o $(OUTPUT_FILE) -a ./$(BUILDDIR)/riemman 0.0000000001 10 2>&1 | tee -a $(OUTPUT_FILE)` "[s]"
	@echo "0.0001 accuracy " `$(TIME) -o $(OUTPUT_FILE) -a ./$(BUILDDIR)/riemman 0.0001 9 2>&1 | tee -a $(OUTPUT_FILE)` "[s]"


example: riemman
	./$(BUILDDIR)/riemman 0.0001 9

clean:
	rm -rf $(BUILDDIR)
