CC=gcc
CFLAGS=-I -pthread
DEPS = node.h my_string.h dbase.h
OBJ = node.o my_string.o dbase.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

node: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
