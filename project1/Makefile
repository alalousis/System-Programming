OBJS = main.o input.o hash.o date.o bst.o commands.o heap.o queue.o exit.o
SOURCE = main.c input.c hash.c date.c bst.c commands.c heap.c queue.c exit.c
HEADER = structs.h functions.h
OUT = diseaseMonitor
CC = gcc
FLAG = -c

all: $(OBJS)
	$(CC) $(OBJS) -o $(OUT)

main.o: main.c
	$(CC) $(FLAG) main.c

input.o: input.c
	$(CC) $(FLAG) input.c

hash.o: hash.c
	$(CC) $(FLAG) hash.c

date.o: date.c
	$(CC) $(FLAG) date.c

bst.o: bst.c
	$(CC) $(FLAG) bst.c

commands.o: commands.c
	$(CC) $(FLAG) commands.c

heap.o: heap.c
	$(CC) $(FLAG) heap.c

queue.o: queue.c
	$(CC) $(FLAG) queue.c

exit.o: exit.c
	$(CC) $(FLAG) exit.c

clean:
	rm -f $(OBJS) $(OUT)
