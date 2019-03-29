CC=gcc
VPATH = obj:src:include
DEPS = my_string.h dbase.h connection.h vector.h node.h
OBJ = my_string.o dbase.o connection.o vector.o node.o main.o

%.o: %.c $(DEPS)
	$(CC) -pthread -c -o $@ $< $(CFLAGS)

node: $(OBJ)
	$(CC) -pthread -o bin/$@ $^ $(CFLAGS)
