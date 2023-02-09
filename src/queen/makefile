############# Makefile for number theory programs        ###############
#                     By Terry R. McConnell                            #
########################################################################

#########################################################################
VERSION = 1.1
CC = gcc
# Add -DDEBUG in individual cc lines below if needed, not here.
CFLAGS = -DPOSIX -L ./libprimes 
#CFLAGS =  -D_POSIX_SOURCE
EXECUTABLES = euclid farey happy totient linear quadratic eratosthenes factor egypt
#########################################################################

prime.h: libprimes.h
prime.o: prime.c prime.h
factorization.o: factorization.c prime.h
zp.o: zp.c
zn.o: zn.c

.c.o: 
	$(CC) -c $(CFLAGS) $<



libprimes.h: 
	cd libprimes && make libprimes


#                    Targets for individual executables 

#############
# Euclid    #
#############
euclid: euclid.c
	$(CC) $(CFLAGS) -DMAIN -o euclid euclid.c


#############
# Egypt     #
############
egypt: egypt.c euclid.o
	$(CC) $(CFLAGS) -DMAIN -o egypt egypt.c euclid.o

#############
# Totient   #
#############
totient: totient.c
	$(CC) $(CFLAGS)  -o totient -DMAIN totient.c

#############
# farey     #
#############
farey: farey.c euclid.c totient.c
	$(CC) $(CFLAGS) -c totient.c euclid.c
	$(CC) $(CFLAGS) -c -DMAIN farey.c
	$(CC) $(CFLAGS) -o farey farey.o euclid.o totient.o

#############
#  happy    #
#############
happy: happy.c
	$(CC) $(CFLAGS) -o happy happy.c

#############
# factor    #
#############
factor: factor.c factorization.o prime.o libprimes.h
	$(CC) $(CFLAGS) -c factor.c
	$(CC) $(CFLAGS) -o factor factor.o prime.o factorization.o -lm -lprimes

#############
#  linear   #
#############
linear: linear.c euclid.o
	$(CC) $(CFLAGS) -DMAIN -o linear linear.c euclid.o

#############
# quadratic #
#############
quadratic: quadratic.c zp.o euclid.o zn.o
	$(CC) $(CFLAGS) -DMAIN -o quadratic quadratic.c zp.o euclid.o zn.o


##############
#eratoshtenes#
##############
eratosthenes: eratosthenes.c
	$(CC) $(CFLAGS) -DMAIN -o eratosthenes eratosthenes.c 


############################## other targets ###############################
all: $(EXECUTABLES)

backup: 
	make distclean || true
	cd ..;tar zcvf queen$(VERSION).tgz queen

distclean: 
	rm $(EXECUTABLES) || true
	rm *.o || true
	cd libprimes && make realclean

clean:
	rm *.o || true
	cd libprimes && make clean

