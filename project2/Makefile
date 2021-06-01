OBJS = main.o input.o hash.o date.o bst.o commands.o heap.o queue.o exit.o pipes.o receive.o signals.o statistics.o
SOURCE = main.c input.c hash.c date.c bst.c commands.c heap.c queue.c exit.c pipes.c receive.c signals.c statistics.c
HEADER = structs.h functions.h
OUT = diseaseAggregator
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

pipes.o: pipes.c
	$(CC) $(FLAG) pipes.c

receive.o: receive.c
	$(CC) $(FLAG) receive.c

signals.o: signals.c
	$(CC) $(FLAG) signals.c

statistics.o: statistics.c
	$(CC) $(FLAG) statistics.c

exit.o: exit.c
	$(CC) $(FLAG) exit.c

clean:
	rm -f $(OBJS) $(OUT)
