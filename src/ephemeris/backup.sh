#!/bin/sh


echo backing up ephemeris code
rm *.o
rm ephemeris
cd ..
tar zcf ephemeris.tgz ephemeris

