##############################################################################
# Makefile for tableau
# Uncomment the appropriate section
#              By Terry McConnell
##############################################################################
############## Linux #####################
# _GNU_SOURCE seems to be needed to prevent compiler warnings with strdup
CFLAGS = -O2 -DDEBUG=1 -D_POSIX_SOURCE -D_GNU_SOURCE -D_LINUX -D_POSIX
LEX = flex
LEX_ARGS = 
LIBS = -lfl
CC = gcc
EXTCMD = strip tableau
##########################################

############# You should not edit below this line ########################

OBJECTS = main.o tableau.o parse.o lex.yy.o memory.o walk.o node.o qsort.o truth.o

.c.o:
	$(CC) -c $(CFLAGS) $<

lex.yy.c: lex.l parse.h
	$(LEX) $(LEX_ARGS) lex.l

tableau: $(OBJECTS)
	$(CC) -o tableau $(OBJECTS) $(LIBS) 
	$(EXTCMD)

all: tableau 

clean:
	rm -f *.o
	rm lex.yy.c

backup: 
	backup.sh
