############# Makefile for classical encryption programs ###############
#                     By Terry R. McConnell                            #
########################################################################

#########################################################################
VERSION = 1.4
CC = gcc
# Add -DDEBUG in individual cc lines below if needed, not here.
CFLAGS = -DPOSIX 
#CFLAGS =  -D_POSIX_SOURCE
EXECUTABLES = caesar vigenere wheatstone beaufort playfair uebchi book onetime enigma columnar hill feistel cardano
#########################################################################

#                    Targets for individual executables 

#############
# Caesar    #
#############
caesar: main.c caesar.c
	$(CC) $(CFLAGS) -DPROGRAM_NAME=\"caesar\" -o caesar main.c caesar.c

#############
# Cardano   #
#############
cardano: main.c cardano.c
	$(CC) $(CFLAGS) -DPROGRAM_NAME=\"cardano\" -o cardano main.c cardano.c

#############
# Vigenere  #
#############
# The vigenere can make good use of very long keys. To change the default
# length, add -DMAX_KEY= to the command line.
vigenere: main.c vigenere.c
	$(CC) $(CFLAGS) -DPROGRAM_NAME=\"vigenere\" -o vigenere main.c vigenere.c

#############
# Beaufort  #
#############
# The beaufort is similar to vigenere. The same comments apply to it.
beaufort: main.c beaufort.c
	$(CC) $(CFLAGS) -DPROGRAM_NAME=\"beaufort\" -o beaufort main.c beaufort.c

#############
# Columnar  #
#############
columnar: main.c columnar.c
	$(CC) $(CFLAGS) -DPROGRAM_NAME=\"columnar\" -o columnar main.c columnar.c

#############
# Hill      #
#############
hill: main.c hill.c
	$(CC) $(CFLAGS) -DPROGRAM_NAME=\"hill\" -o hill main.c hill.c

##############
# Wheatstone #
##############
# Plain text cannot contain a character used to mark the second of a double
# letter pair. Default for this character is ~. Use -DUNUSED= to redefine.
wheatstone: main.c wheatstone.c
	$(CC) $(CFLAGS) -DPROGRAM_NAME=\"wheatstone\"  -o wheatstone main.c wheatstone.c

##############
# Playfair   #
##############
# Program interpolates DOUBLE_MARK (default = ~) between doubled letters in
# the plaintext. Use -DDOUBLE_MARK= to redefine
playfair: main.c playfair.c
	$(CC) $(CFLAGS) -DPROGRAM_NAME=\"playfair\"  -o playfair main.c playfair.c

##############
# Uebchi     #
##############
uebchi: main.c uebchi.c
	$(CC) $(CFLAGS) -DPROGRAM_NAME=\"uebchi\"  -o uebchi main.c uebchi.c

##############
# Book       #
##############
# Allow for expansion of plaintext chars into offset numbers
book: main.c book.c
	$(CC) $(CFLAGS) -DOUTPUT_BUFFER_SIZE=4096 -DPROGRAM_NAME=\"book\"  -o book main.c book.c

##############
# Onetime    #
##############
# 
onetime: main.c onetime.c
	$(CC) $(CFLAGS)  -DPROGRAM_NAME=\"onetime\"  -o onetime main.c onetime.c

##############
# Enigma     #
##############
# 
enigma: main.c enigma.c
	$(CC) $(CFLAGS)  -DPROGRAM_NAME=\"enigma\"  -o enigma main.c enigma.c

##############
# Fiestel    #
##############
#
feistel: main.c feistel.c utils/bin2pc.c
	$(CC) $(CFLAGS) -DBLOCK -DUSE_ECB -I. -DPROGRAM_NAME=\"feistel_ecb\" -o feistel_ecb main.c feistel.c utils/bin2pc.c
	$(CC) $(CFLAGS) -DBLOCK -DUSE_CFB -I. -DPROGRAM_NAME=\"feistel_cfb\" -o feistel_cfb main.c feistel.c utils/bin2pc.c
	$(CC) $(CFLAGS) -DBLOCK  -I. -DPROGRAM_NAME=\"feistel_cbc\" -o feistel_cbc main.c feistel.c utils/bin2pc.c
	$(CC) $(CFLAGS) -DBLOCK -DUSE_OFB -I. -DPROGRAM_NAME=\"feistel_ofb\" -o feistel_ofb main.c feistel.c utils/bin2pc.c
	$(CC) $(CFLAGS) -DBLOCK -DUSE_CCM -I. -DPROGRAM_NAME=\"feistel_ccm\" -o feistel_ccm main.c feistel.c utils/bin2pc.c
	ln -s feistel_ecb feistel

############################## other targets ###############################
all: $(EXECUTABLES)
	cd utils && make all

distrib: 
	make distclean || true
	cd ..;tar zcvf crypto$(VERSION).tgz crypto

distclean: 
	rm $(EXECUTABLES) || true
	rm feistel_* || true
	rm *.o || true
	cd utils && make distclean

clean:
	rm *.o || true
	cd utils && make clean

install:
	cp $(EXECUTABLES) ~/bin
