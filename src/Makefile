CC = gcc
CFLAGS = -O3 -I. -g -Wall -std=c99
all: vparser

vparser: vparser.o
	$(CC) -o vparser vparser.o

vparser.o: vparser.c
	$(CC) $(CFLAGS) -c vparser.c

clean:
	rm -f vparser vparser.o
