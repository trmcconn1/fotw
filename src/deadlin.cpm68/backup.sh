#!/bin/sh
#
# This is written for my own setup. Change to suit yours
# 

OS=`uname`
echo Insert a formatted floppy in drive a: and strike a key when ready.
read response

cd ..
tar cvfz deadlin.tgz deadlin

case $OS in
	Linux) mount /fd/minix || exit
	       cp deadlin.tgz /fd/minix
	       umount /fd/minix
	       ;;
	Minix) mount /dev/fd0 /mnt || exit
               cp deadlin.tgz /mnt
	       umount /dev/fd0
	       ;;
	*)  ;;          # punt
esac
