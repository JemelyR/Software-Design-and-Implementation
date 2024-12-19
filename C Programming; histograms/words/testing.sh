#!/bin/bash
#
# tester file for words.c
#
# executes commands to make sure words.c works properly
# 
# JemelyR, January 2023, cs50



./words < words.c | head #should print head of itself

./words emptyTest #read an empty file, should print out can't read error message


./words < testing #read a test file, should only print words, one word per line


./words < test2 #a test file, should only print words, one words per line


#read what i type until eof, should only print words, one word per line
./words <<EOF 
this is third test#$% hello67 8 9 done
EOF





