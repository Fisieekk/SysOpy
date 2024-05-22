mCC = gcc
CFLAGS = -Wall -std=c99 -g
BUILDDIR = build

.PHONY: all clean runner_integral runner_sender

all: integral sender


integral: integral.c
	mkdir -p $(BUILDDIR)
	rm -rf *_fifo
	$(CC) $(CFLAGS) -o $(BUILDDIR)/$@ $<

sender: sender.c
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -o $(BUILDDIR)/$@ $<

runner_integral: integral
	$(BUILDDIR)/integral

runner_sender: sender
	$(BUILDDIR)/sender

clean:
	rm -rf $(BUILDDIR)
	rm -rf *_fifo