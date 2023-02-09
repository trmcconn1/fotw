#################################################################
#            harvest                                          #
#        By Terry McConnell                                     #
#         2017                                             #
#################################################################

VERSION = 1.0
FLAGS =  -O2   
CC = gcc
INSTALL_DIR = /home/mcconnel/bin
PERMS = 755
HOME_DIR = /home/mcconnel
TXT = 
DIR = $(HOME)/src/harvest   # source

all: harvest uuencode dos2unix 

harvest: harvest.c parse.c
	${CC} ${FLAGS} -o harvest harvest.c parse.c

uuencode: uuencode.c bin2pc.c 
	$(CC) $(FLAGS)  -o uuencode uuencode.c bin2pc.c

install: 
	cp harvest ${INSTALL_DIR}
	chmod ${PERMS} ${INSTALL_DIR}/harvest
	cp uuencode ${INSTALL_DIR}
	chmod ${PERMS} ${INSTALL_DIR}/uuencode
	ln -s ${INSTALL_DIR}/uuencode ${INSTALL_DIR}/uudecode
	cp dos2unix ${INSTALL_DIR}
	chmod ${PERMS} ${INSTALL_DIR}/dos2unix

clean:
	cd ${DIR}; rm -f *.o; rm harvest uuencode dos2unix uudecode

backup:

