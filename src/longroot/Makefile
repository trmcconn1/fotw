##############################################################################
# Makefile for longroot
# Uncomment the appropriate section
#              By Terry McConnell
##############################################################################
############## Linux #####################
CFLAGS = -Wall -static-libgcc -static -DHAVE_STRDUP -D_GNU_SOURCE -D_LINUX -D_POSIX
LIBS =
CC = gcc
EXTCMD = 
##########################################


############# You should not edit below this line ########################

OBJECTS = libarbp.o longsqrt.o longcurt.o longmul.o mystrdup.o longsqrt_standalone.o longcurt_standalone.o

.c.o:
	$(CC) -c $(CFLAGS) $<

longsqrt: longsqrt.o libarbp.o mystrdup.o longsqrt_standalone.o

longmul:  longmul.o libarbp.o mystrdup.o

longcurt: longcurt.o libarbp.o mystrdup.o longcurt_standalone.o

cgi: longsqrt.o longcurt.o libarbp.o mystrdup.o cgi.tab.o lex.yy.o
	gcc -o longroot.cgi cgi.tab.o lex.yy.o longsqrt.o libarbp.o mystrdup.o longcurt.o -lfl

cgi.tab.o: cgi.tab.c 
lex.yy.o: lex.yy.c

lex.yy.c: cgi.l
	flex -I cgi.l
cgi.tab.c: cgi.y
	bison -d cgi.y

all: longsqrt longmul longcurt cgi

clean:
	rm -f lex.yy.c cgi.tab.c cgi.tab.h
	rm -f *.o

dist_clean: clean
	rm -f longsqrt longmul longcurt

backup: 
	backup.sh
