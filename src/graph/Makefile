##############################################################################
# Makefile for graph (graph theory programs)
# Uncomment the appropriate section
#              By Terry McConnell
##############################################################################
############## NeXT (historical interest only) ######################
#CFLAGS = -O2 -D_POSIX_SOURCE  -D_BSD -D_NEXT
#LIBS = 
#CC=gcc
#EXTCMD =
##########################################
############## Minix #####################
#CFLAGS = -O -D_POSIX_SOURCE -D_MINIX -D_POSIX
#LIBS =
#CC = cc
#EXTCMD = chmem +200000 graph
##########################################
############## Linux #####################
#CFLAGS = -O2 -D_POSIX_SOURCE -D_LINUX -D_POSIX
#LIBS = 
#CC = gcc
#EXTCMD = strip graph
##########################################
############## SunOs, UnixWare  ##########
CFLAGS = -O2 -D_POSIX
LIBS = 
CC = gcc
EXTCMD = strip graph
##########################################


############# Do not Edit Below this Line ########################

OBJECTS = main.o parse.o dijkstra.o util.o euler.o walk.o

all: graph

.c.o:
	$(CC) -c $(CFLAGS) $<

graph: $(OBJECTS)
	$(CC) -o graph $(OBJECTS) $(LIBS) 
	$(EXTCMD)

clean:
	rm -f *.o

archive:
	rm -f *.o
	rm graph
	cd .. && tar zcvf graph.tgz graph

backup: 
	backup.sh
