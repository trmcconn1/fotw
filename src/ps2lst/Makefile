##############################################################################
# Makefile for ps2lst
# Uncomment the appropriate section
#              By Terry McConnell
##############################################################################
############## NeXT (historical interest only) ######################
#CFLAGS = -O2 -D_POSIX_SOURCE  -D_BSD -D_NEXT
#LIBS = 
#CC=gcc
#EXTCMD =
##########################################
############## Minix #####################
#CFLAGS = -O -D_POSIX_SOURCE -D_MINIX -D_POSIX
#LIBS =
#CC = cc
#EXTCMD = chmem +200000 ps2lst
##########################################
############## Linux #####################
#CFLAGS = -O2 -D_POSIX_SOURCE -D_LINUX -D_POSIX
#LIBS = 
#CC = gcc
#EXTCMD = strip ps2lst
##########################################
############## SunOs, UnixWare  ##########
CFLAGS = -O2 -D_POSIX
LIBS = 
CC = gcc
EXTCMD = strip ps2lst
##########################################


############# Do not Edit Below this Line ########################

OBJECTS = main.o parse.o 
.c.o:
	$(CC) -c $(CFLAGS) $<

ps2lst: $(OBJECTS)
	$(CC) -o ps2lst $(OBJECTS) $(LIBS) 
	$(EXTCMD)

clean:
	rm -f *.o

backup: 
	backup.sh
