##############################################################################
# Makefile for ephemeris
# Uncomment the appropriate section
#              By Terry McConnell
##############################################################################
############## Linux #####################
CFLAGS = -O2 -D_POSIX_SOURCE -D_LINUX -D_POSIX -DDEBUG
LIBS = -lm
CC = gcc
EXTCMD = strip ephemeris
##########################################


############# Do not Edit Below this Line ########################

OBJECTS = main.o kepler.o radial_kepler.o ephemeris.o secs.o elements.o convert.o

kepler.o: kepler.c
	$(CC) -c $(CFLAGS) -DNO_MAIN kepler.c

radial_kepler.o: radial_kepler.c
	$(CC) -c $(CFLAGS) radial_kepler.c

.c.o:
	$(CC) -c $(CFLAGS) $<

ephemeris: $(OBJECTS)
	$(CC) -o ephemeris $(OBJECTS) $(LIBS) 
	$(EXTCMD)

kepler: 
	$(CC) -c $(CFLAGS) kepler.c
	$(CC) -o kepler kepler.o -lm
	rm kepler.o

all: ephemeris kepler

clean:
	rm -f *.o

backup: 
	backup.sh
