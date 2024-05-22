CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -O0


.PHONY: all lib_static lib_shared client_static client_shared  client_dynamic 

all: lib_static lib_shared objdump_static objdump_shared objdump_dynamic clean

lib_static:
	$(CC) $(CFLAGS) -c collatz.c
	ar rcs libcollatz.a collatz.o

lib_shared:
	$(CC) $(CFLAGS) -fPIC -c collatz.c
	$(CC) $(CFLAGS) -shared collatz.o -o libcollatz.so

client_static: lib_static
	$(CC) $(CFLAGS) client.c libcollatz.a -o client_static 

client_shared: lib_shared
	$(CC) $(CFLAGS) client.c -L libcollatz -Wl,-rpath=. -o client_shared

client_dynamic: lib_shared
	$(CC) $(CFLAGS) client.c -o client_dynamic -ldl -DDYNAMIC

test_static: client_static
	./client_static
test_shared: client_shared
	./client_shared
test_dynamic: client_dynamic
	./client_dynamic
objdump_static: client_static
	objdump -d client_static > client_static_objdump.txt

objdump_shared: client_shared
	objdump -d client_shared > client_shared_objdump.txt

objdump_dynamic: client_dynamic
	objdump -d client_dynamic > client_dynamic_objdump.txt



clean:
	rm -f *.a *.o *.so  client client_static client_shared client_dynamic


clean_all:
	rm -f *.a *.o *.so client client_static client_shared client_dynamic *.txt