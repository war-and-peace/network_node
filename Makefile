CC=gcc
CFLAGS=-I -pthread
DEPS = node.h my_string.h dbase.h main.c
OBJ = node.o my_string.o dbase.o main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

node: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
