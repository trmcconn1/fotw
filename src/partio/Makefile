##############################################################################
# Makefile for partitions
# Uncomment the appropriate section 
#              By Terry McConnell
##############################################################################
############## 64 bit Linux #####################
CFLAGS = -O2 -D_POSIX_SOURCE -D_LINUX -D_POSIX -DN_MAX=416 -DDEBUG
LIBS = fillib.a 
CC = gcc
EXTCMD = strip partio
##########################################

############# Do not Edit Below this Line ########################

OBJECTS = main.o partio.o next.o manager.o

.c.o:
	$(CC) -c $(CFLAGS) $<


partio: $(OBJECTS) fillib.a 
	cd fillib && make -f Makefile all
	$(CC) -o partio $(OBJECTS) $(LIBS) 
	$(EXTCMD)

all: partio

clean:
	rm -f *.o

backup: 
	backup.sh
