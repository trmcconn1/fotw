#!/bin/sh
#
# The files in this directory must be ported from MsDos/Borland C++
# to unix. This script begins that process
# After running this script do a make clean and then a make score.
# That should complete the port

# The following line tells which dos directory we get our dos version
# from. Edit it as necessary
dosdir="/hd/dosc/src/score15"
temp="/tmp/change.$$"
mask="644"

# First copy the files we don't want to clobber to a safe place
echo "Backing up Linux-specific files"
cp score.rc /tmp
cp makefile /tmp
cp source.txt /tmp
cp info.txt /tmp
cp man.txt /tmp
cp gui.cc /tmp
cp rc.dst /tmp

# copy files over from Dos version
echo "Copying files from $dosdir"
cp -v $dosdir/* .


# change all the MsDos ^M, chmod all files

echo "Converting from Dos and chmoding files..."

for file in `ls *`
do
			
	if [ $file != $0 ]
	then
		sed 's///' $file | sed 's///' > $temp
		if [ -s $temp ]
		then
			cp $temp $file
			chmod $mask $file
		fi
	fi
done


# Convert extensions
echo "Converting file extensions..."

for file in `ls *.cpp`
do
	head=`basename $file cpp`
	newname=${head}cc
	mv $file $newname
done
mv gui.cc gui.dos
#

# recover the files saved to tmp
mv /tmp/score.rc .
mv /tmp/makefile .
mv /tmp/source.txt .
mv /tmp/man.txt .
mv /tmp/info.txt .
mv /tmp/gui.cc .
mv /tmp/rc.dst .

#remove a few unwanted files

rm -f *.exe     # Dos exe files
rm -f *.obj     # Dos object files
rm -f bccdos.cfg   # Borland compiler flags file
rm $temp
exit
