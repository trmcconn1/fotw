##############################################################################
# Makefile for partitions
# Uncomment the appropriate section 
#              By Terry McConnell
##############################################################################
############## 64 bit Linux #####################
CFLAGS = -O2 -D_POSIX_SOURCE -D_LINUX -D_POSIX -DN_MAX=416 -DDEBUG
LIBS = filterlib.a 
CC = gcc
EXTCMD = strip partition
##########################################

############# Do not Edit Below this Line ########################

OBJECTS = main.o partition.o next.o manager.o

.c.o:
	$(CC) -c $(CFLAGS) $<


partition: $(OBJECTS) filterlib.a 
	cd filterlib && make -f Makefile all
	$(CC) -o partition $(OBJECTS) $(LIBS) 
	$(EXTCMD)

all: partition

clean:
	rm -f *.o

backup: 
	backup.sh
