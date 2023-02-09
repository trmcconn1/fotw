##############################################################################
# Makefile for lsh (A small shell based on monitor 1.3)
# Edit or create the appropriate OS section
#              By Terry McConnell
##############################################################################
VERSION = 1.3
PROG_NAME = lsh
#
# Edit other settings in macros.a
############## Linux #####################
CC = gcc
CFLAGS = -DUNIX 
LIBS = 
LD = ld
EXTCMD = strip lsh 
##########################################
INSTALL_DIR = $(HOME)/bin

############# One should not have to edit below this line #####################

OBJECTS = main.o monitor.o internal.o external.o  
.c.o:
	$(CC) -c -DVERSION=\"$(VERSION)\" $(CFLAGS) $<

lsh: $(OBJECTS) macros.h internal.h 
	$(CC) $(CFLAGS) -o $(PROG_NAME) $(OBJECTS) $(LIBS) -lc    
	$(EXTCMD)

all: lsh

install:
	cp lsh $(INSTALL_DIR)

clean:
	rm -f *.o

distclean:
	rm -f *.o
	rm -f lsh

distribution: distclean
	cd .. && tar zcvf lsh$(VERSION).tgz lsh$(VERSION)
