# Makefile for tree-filling sequence analyzing program 
CC=gcc
INSTALLDIR=${HOME}/bin

tf: tree_fill.c issquare.c 
	$(CC) -o tf tree_fill.c issquare.c

install: tf
	mv tf $(INSTALLDIR)
