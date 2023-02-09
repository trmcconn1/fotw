#################################################################
#   Noontime scoring program makefile (Linux Version)           #
#        By Terry McConnell                                     #
#        November 2000                                          #
#################################################################

VERSION = 21
FLAGS =  -O2   
INSTALL_DIR = ~/bin
PERMS = 555
HOME_DIR = /home/mcconnel
CCOBJ = score12.o parsers12.o actions12.o runner12.o team12.o
CCSRC = score12.cc parsers12.cc actions12.cc runner.12cc team12.cc 
CSRC  = atosecs.c table12.c table11.c qsort.c
COBJ = atosecs.o table12.o table11.o qsort.o 
TXT = source.txt aims.txt rules.txt man.txt info.txt todo.txt change.log
CONF = score.rc                       # gets copied to .scorerc
DIR = $(HOME)/src/score2012   # source
FLOPPY = /ext2_floppy   # mount point for backup target
KEYMAPDIR =  /usr/lib/kbd/keytables

mkstnds: mkstnds.c atosecs.c
	gcc mkstnds.c atosecs.c -o mkstnds

score: ${CCOBJ} ${COBJ} 
	g++  ${FLAGS} -o score ${CCOBJ} ${COBJ}  -lm

install: 
	cp score ${INSTALL_DIR}
	cp score.map ${KEYMAPDIR}
	chmod ${PERMS} ${INSTALL_DIR}/score

clean:
	cd ${DIR}; rm -f *.o


backup:
