############# Makefile for utilities for crypto package  ###############
#                     By Terry R. McConnell                            #
########################################################################

#########################################################################
VERSION = 1.1
CC = gcc
#CFLAGS = -DPOSIX -DDEBUG
CFLAGS = -DPOSIX 
#CFLAGS =  -D_POSIX_SOURCE
EXECUTABLES = makebook makepad mkrotor uuencode freq
#########################################################################

#                    Targets for individual executables 

#############
# makebook  #
#############
#
# Use -D_SHORT_STRINGS if your compiler does not support multi-line
# string constants. Use -D_NO_RANDOM if your library does not support
# random and srandom. Edit the target frequency distribution table in
# makebook.c and MAX_FILE_SIZE to taste. (See comments on makepad below
# for changing RNG.) 

makebook: makebook.c 
	$(CC) $(CFLAGS) -DPROGRAM_NAME=\"makebook\" -o makebook makebook.c

############################## other targets ###############################

#############
# makepad   #
#############
#
# Use -D_SHORT_STRINGS if your compiler does not support multi-line
# string constants. Use -D_NO_RANDOM if your library does not support
# random and srandom. You could even -DRANDOM= and -DSRANDOM= if you
# want to add support for a better RNG. A Onetime pad is only as good as
# the RNG! If you define the values of these macros to some nonconflicting
# names, you need also to link an object file or library with their
# implementations.

makepad: makepad.c 
	$(CC) $(CFLAGS) -DPROGRAM_NAME=\"makepad\" -o makepad makepad.c


#############
# mkrotor   #
#############
#
# Use -D_NO_RANDOM if your library does not support
# random and srandom. Use -DNCHARS= and -DSTART_CHAR to build for other
# alphabets. Default is 32 and 95 resp, which gives the alphabet of all
# printable characters. Other choices might be 65 and 26 (upper case letters)
# and 97 and 26 (lower case letters.) 

mkrotor: mkrotor.c 
	$(CC) $(CFLAGS) -DPROGRAM_NAME=\"mkrotor\" -o mkrotor mkrotor.c

#############
# pccode    #
#############

uuencode: uuencode.c bin2pc.c 
	$(CC) $(CFLAGS) -I.. -o uuencode uuencode.c bin2pc.c
	test -e uudecode && rm uudecode
	ln -s uuencode uudecode

freq: freq.c
	$(CC) $(CFLAGS) -o freq freq.c

############################## other targets ###############################
all: $(EXECUTABLES)

distrib: 
	make distclean || true

distclean: 
	rm $(EXECUTABLES)
	rm *.o || true

clean:
	rm *.o || true
