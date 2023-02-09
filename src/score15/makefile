#################################################################
#   Noontime scoring program makefile (Linux Version)           #
#        By Terry McConnell                                     #
#        November 2000                                          #
#################################################################

VERSION = 16
FLAGS =  -O2 -DNCURSES  
INSTALL_DIR = /usr/local/bin
PERMS = 555
HOME_DIR = /home/mcconnel
CCOBJ = score.o parsers.o actions.o runner.o team.o
CCSRC = score.cc parsers.cc actions.cc runner.cc team.cc 
CSRC  = atosecs.c table.c qsort.c
COBJ = atosecs.o table.o qsort.o 
TXT = source.txt aims.txt rules.txt man.txt info.txt todo.txt change.log
CONF = score.rc                       # gets copied to .scorerc
DIR = $(HOME)/src/score15   # source
FLOPPY = /ext2_floppy   # mount point for backup target
KEYMAPDIR =  /usr/lib/kbd/keytables

mkstnds: mkstnds.c atosecs.c
	gcc mkstnds.c atosecs.c -o mkstnds

score: ${CCOBJ} ${COBJ} gui.o
	g++  ${FLAGS} -o score ${CCOBJ} ${COBJ} -lncurses -lm

gui.o: gui.cc
	g++ ${FLAGS} -c gui.cc 

install: 
	cp score ${INSTALL_DIR}
	cp score.map ${KEYMAPDIR}
	chmod ${PERMS} ${INSTALL_DIR}/score

clean:
	cd ${DIR}; rm -f *.o


backup:
