##############################################################################
# Makefile for recman
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
#EXTCMD = chmem +200000 recman
##########################################
############## Linux #####################
#CFLAGS = -O2 -D_POSIX_SOURCE -D_LINUX -D_POSIX
#LIBS = 
#CC = gcc
#EXTCMD = strip recman
##########################################
############## SunOs, UnixWare  ##########
CFLAGS = -O2 -D_POSIX
LIBS = 
CC = gcc
EXTCMD = strip recman
##########################################


############# Do not Edit Below this Line ########################

OBJECTS = main.o parse.o db.o
.c.o:
	$(CC) -c $(CFLAGS) $<

recman: $(OBJECTS)
	$(CC) -o recman $(OBJECTS) $(LIBS) 
	$(EXTCMD)

clean:
	rm -f *.o

backup: 
	backup.sh
