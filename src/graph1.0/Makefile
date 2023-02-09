##############################################################################
# Makefile for Graph 
# Edit or create the appropriate OS section
# Use target standalone to create version that does not link with libc
# This means external commands cannot be implemented in c - use _external.a
# This is a 32 bit program so appropriate CFLAGS and LDFLAGS must be used
# on 64 bit platforms to generate 32 bit code
#              By Terry McConnell
##############################################################################
VERSION = 1.0
PROG_NAME = graph
#
# Edit other settings in macros.a
############## Linux 32 bit #####################
CC = gcc
#CFLAGS = 
AS_FLAGS = -f elf  
LIBS = 
AS = nasm
LD = ld
#LDFLAGS = 
EXTCMD = strip graph 
##########################################
############## Linux 64 bit #####################
#CC = gcc
CFLAGS = -m32 -march=i386
#AS_FLAGS = -f elf  
#LIBS = 
#AS = nasm
#LD = ld
LDFLAGS = -m elf_i386
#EXTCMD = strip monitor 
##########################################
INSTALL_DIR = $(HOME)/bin

############# One should not have to edit below this line #####################

OBJECTS = main.o monitor.o lib.o internal.o external.o graph.o parse.o util.o walk.o dijkstra.o euler.o tree.o

.c.o:
	$(CC) -c -DVERSION=\"$(VERSION)\" $(CFLAGS) $<
.a.o:
	$(AS) $(AS_FLAGS) $<

graph: $(OBJECTS) macros.a 
	$(CC) $(CFLAGS) -o $(PROG_NAME) $(OBJECTS) $(LIBS) -lc    
	$(EXTCMD)

all: graph 

install:
	cp graph $(INSTALL_DIR)

clean:
	rm -f *.o

distclean:
	rm -f *.o
	rm -f graph

distribution: distclean
	cd .. && tar zcvf graph$(VERSION).tgz graph$(VERSION)
