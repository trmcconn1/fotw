##############################################################################
# Makefile for turing
# Uncomment the appropriate section
#              By Terry McConnell
##############################################################################
############## Linux #####################
CFLAGS = -O2 -D_POSIX_SOURCE -D_LINUX -D_POSIX
YACC = bison
YACC_ARGS = -y -d
LEX = flex
LEX_ARGS = 
LIBS = -lfl
CC = gcc
EXTCMD = strip turing
##########################################


############# You should not edit below this line ########################

OBJECTS = turing.o qsort.o y.tab.o lex.yy.o stack.o macro.o tape.o

.c.o:
	$(CC) -c $(CFLAGS) $<

y.tab.c: parse.y
	$(YACC) $(YACC_ARGS) parse.y

lex.yy.c: lex.l y.tab.h
	$(LEX) $(LEX_ARGS) lex.l

turing: $(OBJECTS)
	$(CC) -o turing $(OBJECTS) $(LIBS) 
	$(EXTCMD)

lst2tape:
	cd tsrc/universal/lst2tape; make

all: turing lst2tape

clean:
	rm -f *.o
	cd tsrc/universal/lst2tape; make clean

backup: 
	backup.sh
