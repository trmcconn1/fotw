#################################################################
#            thue                                           #
#        By Terry McConnell                                     #
#        September 2014                                             #
#################################################################

VERSION = 1.0
FLAGS =  -O2   
CC = gcc
INSTALL_DIR = /usr/local/bin
PERMS = 555
HOME_DIR = /home/mcconnel
TXT = 
DIR = $(HOME)/src/thue   # source

all: thue matches

thue.o: thue.c
	${CC} -c thue.c

matches.o: matches.c
	${CC} -c matches.c	

thue: thue.o
	${CC} ${FLAGS} -o thue thue.o

matches: matches.o
	${CC} ${FLAGS} -o matches matches.o

install: 
	cp thue matches ${INSTALL_DIR}
	chmod ${PERMS} ${INSTALL_DIR}/thue
	chmod ${PERMS} ${INSTALL_DIR}/matches

clean:
	cd ${DIR}; rm -f *.o


backup:
