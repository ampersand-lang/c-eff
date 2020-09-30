#!/bin/sh
nasm -felf64 -gdwarf yield.asm
gcc -c -ggdb -std=c99 -o eff.o eff.c
gcc -std=c99 -ggdb -o eff eff.o yield.o
