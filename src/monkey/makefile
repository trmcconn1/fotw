#################################################################
#   Monkey Program Makefile (Unix Version)                      #
#        By Terry McConnell                                     #
#        January, 1997                                          #
#################################################################

##########################################
VERSION = \"1.01 Monkey based upon /usr/dict/words\"
DEPTH = 4
MATOPTS = -li
DICT = /usr/dict/words
FLAGS = -O -I./matrix -I. 
CSRC  = main.c node.c
COBJ = main.o node.o 
LIBS = matrix/libmatrix.a
TXT = doc/monkey.1 doc/mkmatrix.1
###########################################

.c.o:
	${CC} ${FLAGS} -c $<

./matrix/build.h: mkmatrix
	mkmatrix -dir matrix -depth ${DEPTH}\
	-d ${DICT} ${MATOPTS}

libmatrix.a: ./matrix/build.h
	cd matrix;\
	make matrix; make clean;\
	echo \#define VERSION ${VERSION} >> build.h

monkey: libmatrix.a ${COBJ}
	${CC} -o monkey ${FLAGS} ${CSRC} ${LIBS}

mkmatrix: mkmatrix.o node.o
	${CC} -o mkmatrix ${FLAGS} mkmatrix.o node.o

clean:
	rm -f *.o;\
	cd matrix; make clean


backup:
	if \
	mount | grep '/fd/ext2_floppy' ;\
	then  \
	echo "Copying C source"; cp -vp ${CSRC} ${FLOPPY};\
	echo "Copying Text files"; cp -vp ${TXT} ${FLOPPY};\
	echo "Copying executables"; cp -vp monkey ${FLOPPY};\
	echo "Copying makefile"; cp -vp makefile ${FLOPPY};\
	echo "Archive complete. Don't forget to umount floppy fs!";\
	else \
	echo "Floppy filesystem not mounted"; \
	echo "Make a filesystem as root with /sbin/mke2fs /dev/fd0"; \
	echo "Then mount /ext2_floppy"; \
	fi

