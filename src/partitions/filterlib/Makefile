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

OBJECTS = distinct_parts.o odd_parts.o superdistinct.o fewerthan.o lessthan.o bressoud.o 1or4mod5.o
.c.o:
	$(CC) -c $(CFLAGS) $<

filterlib.a: $(OBJECTS)
	$(AR) rc filterlib.a $(OBJECTS)  
	ranlib filterlib.a
	cp filterlib.a ../filterlib.a

all: filterlib.a

clean:
	rm -f *.o

