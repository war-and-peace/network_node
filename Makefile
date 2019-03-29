CC=gcc
CFLAGS=-I include -I obj
VPATH = include:obj:src
DEPS = my_string.h dbase.h connection.h vector.h node.h
OBJ = my_string.o dbase.o connection.o vector.o node.o main.o

%.o: %.c
	$(CC) -pthread -c -o obj/$@ $< $(CFLAGS)

node: $(OBJ)
	$(CC) -pthread -o bin/$@ $^ $(CFLAGS)

new:
	mkdir -p bin &
	mkdir -p obj