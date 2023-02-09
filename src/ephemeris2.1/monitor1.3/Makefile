##############################################################################
# Makefile for monitor
# Edit or create the appropriate OS section
#              By Terry McConnell
##############################################################################
VERSION = 1.3
PROG_NAME = monitor
#
# Edit other settings in macros.a
############## Freebsd  #####################
CC = cc
CFLAGS = -DUNIX 
LIBS = 
LD = ld
#LDFLAGS = 
EXTCMD = strip monitor 
##########################################
INSTALL_DIR = $(HOME)/bin

############# One should not have to edit below this line #####################

OBJECTS = main.o monitor.o internal.o external.o  
.c.o:
	$(CC) -c -DVERSION=\"$(VERSION)\" $(CFLAGS) $<

monitor: $(OBJECTS) macros.h internal.h
	$(CC) $(CFLAGS) -o $(PROG_NAME) $(OBJECTS) $(LIBS) -lc    
	$(EXTCMD)

all: monitor

install:
	cp monitor $(INSTALL_DIR)

clean:
	rm -f *.o

distclean:
	rm -f *.o
	rm -f monitor

distribution: distclean
	cd .. && tar zcvf monitor$(VERSION).tgz monitor$(VERSION)
