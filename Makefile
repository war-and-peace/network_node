CC=gcc
CFLAGS=-I -pthread
DEPS = node.h my_string.h
OBJ = node.o my_string.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

node: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
