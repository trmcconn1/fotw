############################################################################
# UNIX Makefile for Newsmstr Program by Terry McConnell                    #
############################################################################
.SUFFIXES : .c .o

OBJS = main.o shell.o ui.o compare.o qsort.o dialogs.o


CFLAGS = -I../../include  -Wall

.c.o: 
	 ${CC} $(CFLAGS) -c $<

	
newsmstr: $(OBJS)
	gcc -o newsmstr $(OBJS) 

clean:
	rm -f *.o

dos:
	cp *.c /dosc/src/schedule/src/newsmstr;



