##############################################################################
# Makefile for flib: My library of mathematical functions
# Uncomment the appropriate section
#              By Terry McConnell
##############################################################################
INSTALL_DIR = /home/tmc/src/flib
############## Linux(gcc) #####################
# I learned the hard way always to use -Wall with gcc
#CFLAGS =  -O2 -Wall -I$(INSTALL_DIR)/include -DDEBUG -D_GNU_SOURCE -D_LINUX 
CFLAGS =  -O2 -Wall -I$(INSTALL_DIR)/include -D__GNU_C -D_UNIX -D_LINUX 
LFLAGS = -Llib   
LIBS =  -lm -lf
CC = gcc
EXTCMD = 
##########################################


############# You should not edit below this line ########################

PROGRAMS = hyper confluent gamma bessel
HYPER = hyper.o user.o format.o exception.o 
CONFLUENT = confluent.o user.o format.o exception.o 
GAMMA = gamma.o user.o format.o exception.o 
BESSEL = bessel.o user.o format.o exception.o

.c.o:
	$(CC) -c $(CFLAGS) $<

libf.a: 
	cd lib; make

hyper: $(HYPER) libf.a
	$(CC) -o hyper $(LFLAGS) $(HYPER) $(LIBS) 

confluent: $(CONFLUENT) libf.a
	$(CC) -o confluent $(LFLAGS) $(CONFLUENT) $(LIBS) 

gamma: $(GAMMA) libf.a
	$(CC) -o gamma $(LFLAGS) $(GAMMA) $(LIBS) 

bessel: $(BESSEL) libf.a
	$(CC) -o bessel $(LFLAGS) $(BESSEL) $(LIBS) 

all:  $(PROGRAMS)

clean:
	rm -f *.o
	cd lib; make clean

real_clean:
	rm -f *.o
	rm -f $(PROGRAMS)
	cd lib; make real_clean

backup: 
	backup.sh
