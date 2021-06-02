OBJS = whoServer.o threads.o circularBuffer.o sockets.o receive.o
SOURCE = whoServer.c threads.c circularBuffer.c sockets.c receive.c
HEADER = structs.h functions.h variables.h
OUT = whoServer
CC = gcc
FLAG = -c

all: $(OBJS)
	$(CC) $(OBJS) -o $(OUT) -lpthread

whoServer.o: whoServer.c
	$(CC) $(FLAG) whoServer.c

threads.o: threads.c
	$(CC) $(FLAG) threads.c

circularBuffer.o: circularBuffer.c
	$(CC) $(FLAG) circularBuffer.c

sockets.o: sockets.c
	$(CC) $(FLAG) sockets.c

receive.o: receive.c
	$(CC) $(FLAG) receive.c

clean:
	rm -f $(OBJS) $(OUT)
