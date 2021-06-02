OBJS = master.o input.o hash.o date.o bst.o commands.o heap.o queue.o exit.o pipes.o sockets.o
SOURCE = master.c input.c hash.c date.c bst.c commands.c heap.c queue.c exit.c pipes.c sockets.c
HEADER = structs.h functions.h
OUT = master
CC = gcc
FLAG = -c

all: $(OBJS)
	$(CC) $(OBJS) -o $(OUT)

master.o: master.c
	$(CC) $(FLAG) master.c

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

pipes.o: pipes.c
	$(CC) $(FLAG) pipes.c

sockets.o: sockets.c
	$(CC) $(FLAG) sockets.c

exit.o: exit.c
	$(CC) $(FLAG) exit.c

clean:
	rm -f $(OBJS) $(OUT) parent worker

worker:
	gcc -o worker worker.c commands.c pipes.c date.c bst.c exit.c hash.c heap.c input.c queue.c sockets.c

parent:
	gcc -o parent parent.c commands.c pipes.c date.c bst.c exit.c hash.c heap.c input.c queue.c sockets.c
