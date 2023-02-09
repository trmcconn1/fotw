##############################################################################
# Makefile for deadlin
# Uncomment the appropriate section
#              By Terry McConnell
##############################################################################
#
# I totally give up trying to figure out how to properly choose macros that
# distinguish among sgetty (BSD? V7??) and Termio/Termios. _POSIX does
# not seem to be a standard macro, but for us it means: don't use sgetty - 
# use termio(s). We want to use the "XSI extensions" of the POSIX standard
# when it is available since it includes useful features of the terminal
# driver. _POSIX_SOURCE seems to turn this off, but _GNU_SOURCE includes
# all POSIX extenions ever heard of.
#
############## FREEBSD  ######################
#CFLAGS = -O2 -D_POSIX -D_BSD 
#LIBS = 
#CC=cc
#EXTCMD = strip deadlin 
##########################################
############## Minix #####################
#CFLAGS = -O -D_POSIX_SOURCE -D_MINIX -D_POSIX
#LIBS =
#CC = cc
#EXTCMD = chmem +200000 deadlin
##########################################
############## Linux #####################
CFLAGS = -O2  -D_GNU_SOURCE -D_LINUX -D_POSIX
LIBS = 
CC = gcc
EXTCMD = strip deadlin
##########################################
############## SunOs, UnixWare  ##########
#CFLAGS = -O2  -D_POSIX
#LIBS = 
#CC = gcc
#EXTCMD = strip deadlin
##########################################


############# Do not Edit Below this Line ########################

OBJECTS = main.o unix.o readline.o parse.o
.c.o:
	$(CC) -c $(CFLAGS) $<

deadlin: $(OBJECTS)
	$(CC) -o deadlin $(OBJECTS) $(LIBS) 
	$(EXTCMD)

clean:
	rm -f *.o

backup: 
	backup.sh
