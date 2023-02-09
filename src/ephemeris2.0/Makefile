##############################################################################
# Makefile for Ephemeris 2.0 
# Edit or create the appropriate OS section
# Use target standalone to create version that does not link with libc
# This means external commands cannot be implemented in c - use _external.a
# This is a 32 bit program so appropriate CFLAGS and LDFLAGS must be used
# on 64 bit platforms to generate 32 bit code
#              By Terry McConnell
##############################################################################
VERSION = 2.0
PROG_NAME = ephemeris
#
# Edit other settings in macros.a
############## Linux 32 bit #####################
CC = gcc
#CFLAGS = 
AS_FLAGS = -f elf  
LIBS = -lm
AS = nasm
LD = ld
#LDFLAGS = 
EXTCMD = strip ephemeris 
##########################################
############## Linux 64 bit #####################
CFLAGS = -m32 -march=i386
LDFLAGS = -m elf_i386
##########################################
INSTALL_DIR = $(HOME)/bin

############# One should not have to edit below this line #####################

OBJECTS = main.o monitor.o lib.o internal.o external.o convert.o elements.o ephemeris.o kepler.o parse.o radial_kepler.o secs.o dialog.o

kepler.o: kepler.c
	$(CC) -c kepler.c $(CFLAGS) -DNO_MAIN      
.c.o:
	$(CC) -c -DVERSION=\"$(VERSION)\" $(CFLAGS) $<
.a.o:
	$(AS) $(AS_FLAGS) $<

ephemeris: $(OBJECTS) macros.a 
	$(CC) $(CFLAGS) -o $(PROG_NAME) $(OBJECTS) $(LIBS) -lc    
	$(EXTCMD)

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
