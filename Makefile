CC = gcc
CFLAGS = -g -Wall -O3
HASHDIR = ./uthash/include
CFLAGS += -I$(HASHDIR)

main: main.o reassemble.o
	$(CC) main.o reassemble.o  -l curl -o main

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

reassemble.o: reassemble.c reassemble.h $(HASHDIR)/uthash.h
	$(CC) $(CFLAGS) -c reassemble.c

clean:
	rm *.o main