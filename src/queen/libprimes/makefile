#################################################################
#   Libprimes  Program Makefile (Linux Version)                 #
#        By Terry McConnell                                     #
#        October 2005                                           #
#################################################################

VERSION = 1
ENDRANGE = 100000
CFLAGS =  -I. -I.. 
TXT = 
CC = gcc
LIBNAME = libprimes.a
AR = ar
RANLIB = ranlib


.c.o:
	${CC} ${FLAGS} -c $<
	 
libprimes: ../eratosthenes
	echo '#include "global.h"' > libprimes.c	
	../eratosthenes -c $(ENDRANGE) >> libprimes.c 
	$(CC) $(CFLAGS) -c libprimes.c	
	${AR} rv ${LIBNAME} libprimes.o
	${RANLIB} ${LIBNAME}
	echo -n '#define N_PRIMES ' > libprimes.h
	grep primes libprimes.c | sed 's/.*\[\(.*\)\].*/\1/' >> libprimes.h

clean:
	rm -f libprimes.o

realclean: clean
	rm -f libprimes.c libprimes.a libprimes.h
