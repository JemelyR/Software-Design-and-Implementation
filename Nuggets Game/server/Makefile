#
# Makefile for 'server'
# CS50 project 'Nuggets'
#
# Rachel Matthew, March 2023
#

CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../support
CC = gcc
MAKE = make

LIBS = ../support/support.a
OBJS = grid.o gold.o players.o

.PHONY: clean

server: server.c $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@ -lm

goldtest: gold.c grid.o players.o
	$(CC) $(CFLAGS) $^ -DUNIT_TEST $(LIBS) -o $@ -lm

playerstest: players.c grid.o gold.o
	$(CC)	$(CFLAGS) $^ -DUNIT_TEST $(LIBS) -o $@ -lm

gridtest: gridtest.c $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@ -lm

grid.o: grid.c grid.h
gold.o: gold.c gold.h
players.o: players.c players.h

clean:
	if [[ -e server ]]; then rm -f server; fi	
	if [[ -e goldtest ]]; then rm -f goldtest; fi
	if [[ -e playerstest ]]; then rm -f playerstest; fi
	if [[ -e gridtest ]]; then rm -f gridtest; fi
	rm -f *.o vgcore.*
	

