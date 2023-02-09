#!/bin/sh


echo backing up ephemeris code
rm *.o
rm ephemeris
cd ..
tar zcf ephemeris2.0.tgz ephemeris2.0

