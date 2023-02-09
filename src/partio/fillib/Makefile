##############################################################################
# Makefile for library of filter tests for partition program
#              By Terry McConnell
##############################################################################
############## Linux #####################
CFLAGS = -O2 -D_POSIX_SOURCE -D_LINUX -D_POSIX -DDEBUG
CC = gcc
AR=ar
##########################################

############# Do not Edit Below this Line ########################

OBJECTS = distinct.o odd.o super.o fewer.o lessthan.o bressoud.o sqmod5.o
.c.o:
	$(CC) -c $(CFLAGS) $<

filterlib.a: $(OBJECTS)
	$(AR) rc fillib.a $(OBJECTS)  
	ranlib fillib.a
	cp fillib.a ../fillib.a

all: fillib.a

clean:
	rm -f *.o

