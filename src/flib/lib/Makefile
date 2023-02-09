##############################################################################
# Makefile for flib: My library of mathematical functions
# Uncomment the appropriate section
#              By Terry McConnell
##############################################################################
INSTALL_DIRECTORY = /home/tmc/src/flib
############## Linux #####################
CFLAGS = -O2 -Wall -I$(INSTALL_DIRECTORY)/include -D__GNU_C -D_LINUX 
AR = ar
ARFLAGS = rcus
EXTCMD = 
##########################################


############# You should not edit below this line ########################

LIB = libf.a
MEMBERS = hyper.o confluent.o gamma.o version.o integrate.o bessel.o roundoff.o

.c.o:
	$(CC) -c $(CFLAGS) $<

flib.a: $(MEMBERS)
	$(AR) $(ARFLAGS) $(LIB) $(MEMBERS) 

all: $(LIB)

clean:
	rm -f *.o

real_clean:
	rm -f *.o
	rm -f libf.a

backup: 
	backup.sh
