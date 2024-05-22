# /*
# 1) otworz biblioteke
# 2) przypisz wskaznikom f1 i f2 adresy funkcji z biblioteki sumuj i srednia
# 3) stworz Makefile kompilujacy biblioteke 'bibl1' ladowana dynamicznie
#    oraz kompilujacy ten program
# biblioteka jest w pliku bibl1.c i bibl1.h
# program jest w pliku zad1.c
# */

CC = gcc
CFLAGS = -Wall

all: test_dynamic

lib_shared:
	$(CC) $(CFLAGS) -fPIC -c bibl1.c
	$(CC) $(CFLAGS) -shared bibl1.o -o libbibl1.so

zad1_dynamic: lib_shared
	$(CC) $(CFLAGS) zad1.c -o zad1_dynamic -ldl -DDYNAMIC

test_dynamic: zad1_dynamic
	./zad1_dynamic

clean:
	rm -f *.o
	rm -f *.so
	rm -f zad1_dynamic