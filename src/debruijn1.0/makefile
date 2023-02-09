#################################################################
#            debruijn                                           #
#        By Terry McConnell                                     #
#        April 2013                                             #
#################################################################

VERSION = 1.0
FLAGS =  -O2  -DMAX_LEVEL=16 -DMAX_STRING=65536 
INSTALL_DIR = ${HOME}/bin
PERMS = 555
CSRC  = debruijn.c EM.c linear.c fixedpoint.c util.c helix.c readfile.c main.c
COBJ = debruijn.o EM.o linear.o fixedpoint.o util.o helix.o readfile.o main.o
TXT = 
DIR = $(HOME)/src/debruijn1.0   # source

.c.o:
	${CC} -c ${FLAGS} $<

debruijn: ${COBJ} 
	${CC}  ${FLAGS} -o debruijn ${COBJ}  

install: 
	cp debruijn ${INSTALL_DIR}
	chmod ${PERMS} ${INSTALL_DIR}/debruijn

clean:
	cd ${DIR}; rm -f *.o


backup:
