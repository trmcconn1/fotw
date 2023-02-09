##############################################################################
# Makefile for regression
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
#EXTCMD = chmem +200000 regression
##########################################
############## Linux #####################
CFLAGS = -O2 -D_POSIX_SOURCE -D_LINUX -D_POSIX
LIBS = -lm 
CC = gcc
EXTCMD = strip regression
##########################################
############## SunOs, UnixWare  ##########
#CFLAGS = -O2 -D_POSIX
#LIBS = 
#CC = gcc
#EXTCMD = strip regression
##########################################


############# Do not Edit Below this Line ########################

OBJECTS = main.o util.o regression.o 
.c.o:
	$(CC) -c $(CFLAGS) $<

regression: $(OBJECTS)
	$(CC) -o regression $(OBJECTS) $(LIBS) 
	$(EXTCMD)

clean:
	rm -f *.o

backup: 
	backup.sh
