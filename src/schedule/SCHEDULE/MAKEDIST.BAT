@ECHO OFF
REM MAKE THE DISTRIBUTION FLOPPIES (Dos Batch File)
REM
REM Usage: makedist
REM Uses pkzip to archive files and create 1-2 disks in floppy a:

REM THE FOLLOWING SHOULD BE SET TO THE PARENT OF THIS DIRECTORY
SET HOME=C:\src


cd %HOME%
pkzip schedule.zip -rP schedule\*.*


CHOICE "Make Distribution Disk 1 (a:schedule.zip) ?"
IF ERRORLEVEL 2 GOTO second
PAUSE Make sure a formatted floppy is in drive a:

copy schedule.zip a:

:second
CHOICE "Make Distribution Disk 2 (a:schedule\*.*) ?"
IF ERRORLEVEL 2 GOTO skip
PAUSE Make sure a formatted floppy is in drive a: 
pkunzip -d schedule.zip a:

:skip
REM  Clean up and restore original directory                                     
ECHO Archive file is %HOME%\schedule.zip
CHOICE "Discard it?"
IF ERRORLEVEL 2 GOTO done
del schedule.zip
:done
cd %HOME%\schedule
