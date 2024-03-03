CC = gcc -Wall
static:
	make clean
	$(CC) -c Zad1.c
	ar rcs Zad1.a Zad1.o

shared:
	make clean
	$(CC) -shared -o libcount.so -fPIC Zad1.c

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so