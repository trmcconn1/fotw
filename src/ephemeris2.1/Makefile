##############################################################################
# Makefile for Ephemeris 2.0 
# Edit or create the appropriate OS section
#              By Terry McConnell
##############################################################################
VERSION = 2.1
PROG_NAME = ephemeris
#
# Edit other settings in macros.h
############## Linux  #####################
CC = gcc
CFLAGS = -DUNIX
LIBS = -lm
LD = ld
LDFLAGS = 
EXTCMD = strip ephemeris 
##########################################
INSTALL_DIR = $(HOME)/bin

############# One should not have to edit below this line #####################

OBJECTS = main.o monitor.o internal.o external.o convert.o elements.o ephemeris.o kepler.o parse.o radial_kepler.o secs.o dialog.o

ephemeris: $(OBJECTS) macros.h internal.h
	$(CC) $(CFLAGS) -o $(PROG_NAME) $(OBJECTS) $(LIBS) -lc    
	$(EXTCMD)

# kepler is incorporated from a stand-alone program of the same name
# and needs a special build line to define NO_MAIN
 
kepler.o: kepler.c
	$(CC) -c kepler.c $(CFLAGS) -DNO_MAIN      
.c.o:
	$(CC) -c -DVERSION=\"$(VERSION)\" $(CFLAGS) $<

all: ephemeris 

install:
	cp ephemeris $(INSTALL_DIR)

clean:
	rm -f *.o

distclean:
	rm -f *.o
	rm -f ephemeris

distribution: distclean
	cd .. && tar zcvf ephemeris$(VERSION).tgz ephemeris$(VERSION)
