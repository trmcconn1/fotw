#!/bin/sh

UNIX2DOS=${HOME}/bin/unix2dos


echo backing up partition code
# backup windows version
for myfile in *.c 
do $UNIX2DOS < $myfile > windows/$myfile
done
for myfile in *.h
do $UNIX2DOS < $myfile > windows/$myfile
done
for myfile in fillib/*.c
do $UNIX2DOS < $myfile > windows/$myfile
done
for myfile in fillib/*.h
do $UNIX2DOS < $myfile > windows/$myfile
done

rm *.o
rm partition
cd ..
tar zcf partio.tgz partio
zip -r partio.zip partio

