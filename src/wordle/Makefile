##############################################################################
# Makefile for wordle
# Edit or create the appropriate OS section
#              By Terry McConnell
##############################################################################
VERSION = 1.0
PROG_NAME = wordle 
#
# Edit other settings in macros.a
############## Freebsd  #####################
#CC = cc
#CFLAGS = -DUNIX 
#LIBS = 
#LD = ld
#LDFLAGS = 
#EXTCMD = strip monitor 
##########################################
############## Linux  #####################
CC = cc
CFLAGS = -DUNIX 
LIBS = 
LD = ld
#LDFLAGS = 
EXTCMD = strip wordle 
##########################################
INSTALL_DIR = $(HOME)/bin

############# One should not have to edit below this line #####################

OBJECTS = main.o monitor.o internal.o external.o  
.c.o:
	$(CC) -c -DVERSION=\"$(VERSION)\" $(CFLAGS) $<

wordle: $(OBJECTS) macros.h internal.h
	$(CC) $(CFLAGS) -o $(PROG_NAME) $(OBJECTS) $(LIBS) -lc    
	$(EXTCMD)

all: wordle

install:
	cp wordle $(INSTALL_DIR)

clean:
	rm -f *.o

distclean:
	rm -f *.o
	rm -f wordle

distribution: distclean
	cd .. && tar zcvf wordle$(VERSION).tgz wordle$(VERSION)
