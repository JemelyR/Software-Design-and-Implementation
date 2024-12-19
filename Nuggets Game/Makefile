#
# Makefile for Nuggets game
# CS50 project 'Nuggets'
#
# Makefile by: Daniel Ha, March 5th, 2023
# Project by: Rose, Rachel, Jemely, Daniel

L = support server client maps
.PHONY: all clean

all:
	make -C support support.a
	make -C server server
	make -C client client

clean:
	make -C server clean
	make -C client clean
	make -C support clean
	
