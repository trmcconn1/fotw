##############################################################################
# Makefile for lst2tape
# Uncomment the appropriate section
#              By Terry McConnell
##############################################################################
############## Linux #####################
CFLAGS = -O2 
LIBS = 
CC = gcc
EXTCMD = strip lst2tape
##########################################


############# You should not edit below this line ########################

OBJECTS = lst2tape.o

.c.o:
	$(CC) -c $(CFLAGS) $<


lst2tape: $(OBJECTS)
	$(CC) -o lst2tape $(OBJECTS) $(LIBS) 
	$(EXTCMD)


all: lst2tape

clean:
	rm -f *.o

