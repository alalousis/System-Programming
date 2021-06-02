OBJS = whoClient.o queries.o threads.o
SOURCE = whoClient.c queries.c threads.c
HEADER = functions.h structs.h variables.h
OUT = whoClient
CC = gcc
FLAG = -c

all: $(OBJS)
	$(CC) $(OBJS) -o $(OUT) -lpthread

whoServer.o: whoClient.c
	$(CC) $(FLAG) whoClient.c

queries.o: queries.c
	$(CC) $(FLAG) queries.c

threads.o: threads.c
	$(CC) $(FLAG) threads.c

clean:
	rm -f $(OBJS) $(OUT)
