##############################################################################
# Makefile for gpxpoly
# Uncomment the appropriate section
#              By Terry McConnell
##############################################################################
############## NeXT ######################
#CFLAGS = -O2 -D_POSIX_SOURCE  -D_BSD -D_NEXT
#LIBS = 
#CC=gcc
#EXTCMD =
##########################################
############## Minix #####################
#CFLAGS = -O -D_POSIX_SOURCE -D_MINIX -D_POSIX
#LIBS =
#CC = cc
#EXTCMD = chmem +200000 gpxpoly
##########################################
############## Linux #####################
CFLAGS = -O2 -D_POSIX_SOURCE -D_LINUX -D_POSIX
LIBS =  -lm
CC = gcc
#EXTCMD = strip gpxpoly
##########################################
############## MAC  ##########
#CFLAGS = -O2  -D_POSIX
#LIBS = 
#CC = cc
EXTCMD = strip gpxpoly
##########################################


############# Do not Edit Below this Line ########################

OBJECTS = main.o gpxpoly.o convert.o secs.o parse.o dialog.o 
.c.o:
	$(CC) -c $(CFLAGS) $<

deadlin: $(OBJECTS)
	$(CC) -o gpxpoly $(OBJECTS) $(LIBS) 
	$(EXTCMD)

clean:
	rm -f *.o
	rm -f gpxpoly

backup: clean 
	backup.sh
