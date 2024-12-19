#
# Makefile for 'client'
# CS50 project 'Nuggets'
#
# Daniel Ha, March 2023
#

CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../support
CC = gcc
MAKE = make

LIBS = ../support/support.a

.PHONY: clean

############# default rule ###########
client: client.o
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@ -lcurses

client.o: client.h client.c

############# clean ###########
clean:
	rm -f client
	rm -rf *~ *.o *.gch *.dSYM
	rm -f *.log
