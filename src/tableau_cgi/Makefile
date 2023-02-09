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

# These are for the cgi interface. Our boilerplate code also unfortunately
# uses a lex scanner and parser, as does the tableau program itself. To
# prevent name conflicts between the two different scanners we use the
# -P (prefix) option of flex and the corresponding -p option of bison.

cgi.tab.o: cgi.tab.c
lex.cgi.o: lex.cgi.c

lex.cgi.c: cgi.l
	$(LEX) -I -Pcgi cgi.l

cgi.tab.c: cgi.y
	bison -d -p cgi cgi.y

cgi: cgi.tab.o lex.cgi.o tableau.o parse.o lex.yy.o memory.o walk.o node.o qsort.o truth.o tableau_cgi.o
	$(CC) -o tableau_cgi tableau_cgi.o cgi.tab.o lex.cgi.o tableau.o parse.o lex.yy.o memory.o walk.o node.o qsort.o truth.o -lfl 
	sudo cp tableau_cgi /home/tmc/cgi-bin
	sudo chown apache.apache /home/tmc/cgi-bin/tableau_cgi
	sudo chmod 500 /home/tmc/cgi-bin/tableau_cgi

all: tableau cgi

clean:
	rm -f *.o
	rm lex.yy.c

backup: 
	backup.sh
