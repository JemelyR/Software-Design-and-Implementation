#!/bin/bash
# testing.sh
# testing file for chill.c
#
# executes commands to test that chill.c is working properly
#
#
# JemelyR, January 2023, cs50 23W


./chill # no arguments, prints auto table

./chill 44 #one argument, prints table of just those temps

./chill 86 #out of temp range, error

./chill 30 0.2 #temp okay but speed out of range, error

./chill 16.7 9 #decimal temp and two arguments

./chill -100 0.3 #both temp and speed out of range, error

./chill 7 8 9 10 #too many argument, error





