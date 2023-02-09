##############################################################################
# Makefile for library of filter tests for partition program (DOS version)
#              By Terry McConnell
##############################################################################
############## Borland (16bit large model ) ####################
CFLAGS = -ml -O2 
CC = bcc
AR=lib.exe
##########################################

############# Do not Edit Below this Line ########################

OBJECTS = distinct.obj odd.obj super.obj fewer.obj lessthan.obj bressoud.obj sqmod5.obj
# if this gets too long you will need to use a response file
ACTIONS = +distinct.obj +odd.obj +super.obj +fewer.obj +lessthan.obj +bressoud.obj sqmod5.obj

.c.obj:
	$(CC) -c $(CFLAGS) $<

fillib.lib: $(OBJECTS)
	$(AR) fillib.lib $(ACTIONS)  
	copy fillib.lib ../fillib.lib

all: fillib.lib

clean:
	del *.obj
	del fillib.lib
