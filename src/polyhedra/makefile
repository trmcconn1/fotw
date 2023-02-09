############# Makefile for polyhedra program             ###############
#                     By Terry R. McConnell                            #
########################################################################

#########################################################################
VERSION = 1.0
CC = gcc
# Add -DDEBUG in individual cc lines below if needed, not here.
CFLAGS = -DPOSIX 
#CFLAGS =  -D_POSIX_SOURCE
EXECUTABLES = poly 
#########################################################################

#                    Targets for individual executables 

poly: main.c poly.c poly.h
	$(CC) $(CFLAGS)  -o poly main.c poly.c

############################## other targets ###############################
all: $(EXECUTABLES)

backup: 
	make distclean || true
	cd ..;tar zcvf poly$(VERSION).tgz polyhedra

distclean: 
	rm $(EXECUTABLES) || true
	rm *.o || true
	rm core.* || true

clean:
	rm *.o || true
