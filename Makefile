CC=gcc
CFLAGS=-I.
LIBS=parson.o
OBJ=client.o requests.o helpers.o buffer.o

all: client

client: $(OBJ) $(LIBS)
	$(CC) -o $@ $^ -Wall

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

parson.o: parson.c
	$(CC) -c -o $@ $< $(CFLAGS)

run: client
	./client

clean:
	rm -f *.o client
