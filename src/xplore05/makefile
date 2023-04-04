# Makefile for xplore

# Use these settings with NCURSES
CFLAGS =  -DNCURSES  -DXPLORE 
# Use these settings for Minix
#CFLAGS =  -DXPLORE  -D_MINIX -DNO_STRDUP -DLPR=\"lp\" -wo
# if your more program is not in /usr/bin then also include
#-DMORE=\"path\" where path is a path to more
LIBS = -lncurses
#LIBS = -lcurses
CC=cc
SRC = xplore.c browse.c edit.c history.c

xplore: $(SRC) browse.h history.h
	$(CC) $(CFLAGS) -o $@ $(SRC) $(LIBS) 

all: xplore

clean: 
	rm *.o

distclean:
	rm *.o || rm xplore

distribution: 
	cd .. && tar zcvf xplore.05.tgz xplore
	cd xplore
