############# Makefile for dally program        ###############
#                     By Terry R. McConnell                            #
########################################################################

#########################################################################
VERSION = 1.0
CC = gcc
# Add -DDEBUG in individual cc lines below if needed, not here.
CFLAGS = -DPOSIX  
#CFLAGS =  -D_POSIX_SOURCE
EXECUTABLES = dally
#########################################################################

.c.o: 
	$(CC) -c $(CFLAGS) $<


#                    Targets for individual executables 

#############
# dally     #
#############
dally: dally.o mtwist.o randistrs.o 
	$(CC) $(CFLAGS) -DMAIN dally.o mtwist.o randistrs.o -o  dally -lm

all: $(EXECUTABLES)

clean:
	rm *.o || true

