##############################################################################
# Makefile for sorts: My humble library of sorting algorithms
#              By Terry McConnell
##############################################################################
INSTALL_DIR = /home/tmc/src/sorts
VERSION = 1.1
############## Linux gcc  #####################
# I learned the hard way always to use -Wall with gcc
CFLAGS =  -O2  -Wall 
CC = gcc
EXTCMD = 
##########################################

COMMON = util.o compare.o data.o
SORTS = quicksort.o insertsort.o mergesort.o bubblesort.o heapsort.o shellsort.o selectsort.o countingsort.o
.c.o:
	$(CC) -c -DVERSION=\"$(VERSION)\" $(CFLAGS) $<


testsort: $(COMMON) testsort.o $(SORTS)
	$(CC) -o  testsort $(LFLAGS) $(COMMON) testsort.o $(SORTS)  

clean:
	rm -f *.o

dist_clean:
	rm -f *.o
	rm testsort

distribution: 
	cd .. && tar zcf sorts1.1.tgz sorts
	
