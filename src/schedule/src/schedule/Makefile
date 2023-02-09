############################################################################
# UNIX Makefile for Schedule Program by Terry McConnell                    #
############################################################################
.SUFFIXES : .c .o

OBJS = main.o shell.o ui.o calendar.o load.o  compare.o qsort.o \
schedcmd.o shelldb.o audit.o roomchrt.o

CFLAGS =   -I../../include   -Wall

.c.o: 
	 ${CC} $(CFLAGS) -c $<

	
schedule: $(OBJS)
	gcc -o schedule $(OBJS) 

clean:
	rm -f *.o

dos:
	cp *.c /dosc/src/schedule/src;\
	cp ../../scripts/sched.f97 /dosc/src/schedule;\
	cp ../../data/* /dosc/src/schedule/data;\
	cp ../../data/f97/* /dosc/src/schedule/data/f97;\
	cp ../../include/* /dosc/src/schedule/include;\
	cp ../../examples/* /dosc/src/schedule/examples;\
	cp ../../doc/* /dosc/src/schedule/doc



