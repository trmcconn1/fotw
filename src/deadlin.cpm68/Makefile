##############################################################################
# Makefile for deadlin
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
#EXTCMD = chmem +200000 deadlin
##########################################
############## Linux #####################
#CFLAGS = -O2 -D_POSIX_SOURCE -D_LINUX -D_POSIX
#LIBS = 
#CC = gcc
#EXTCMD = strip deadlin
##########################################
############## SunOs, UnixWare  ##########
#CFLAGS = -O2  -D_POSIX
#LIBS = 
#CC = gcc
#EXTCMD = strip deadlin
##########################################
############## SunOs, UnixWare  ##########
CFLAGS = -D_CPM
LIBS = 
CC = gcc
EXTCMD = 
##########################################


############# Do not Edit Below this Line ########################

OBJECTS = main.o cpm.o readline.o parse.o
.c.o:
	$(CC) -c $(CFLAGS) $<

deadlin: $(OBJECTS)
	$(CC) -o deadlin $(OBJECTS) $(LIBS) 
	$(EXTCMD)

clean:
	rm -f *.o

backup: 
	backup.sh
