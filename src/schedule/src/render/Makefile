############################################################################
# UNIX Makefile for Rendering Programs by Terry McConnell                    #
############################################################################
.SUFFIXES : .c .o

OBJS = rch2txt.o qsort.o


CFLAGS = -I../../include  -Wall

.c.o: 
	 ${CC} $(CFLAGS) -c $<

all: rch2txt
	
rch2txt: $(OBJS)
	gcc -o rch2txt $(OBJS) 

clean:
	rm -f *.o



