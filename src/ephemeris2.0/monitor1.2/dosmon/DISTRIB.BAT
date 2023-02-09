@ECHO OFF
REM ARCHIVE PACKAGE FOR DISTRIBUTION
REM This assumes my disk layout. YMMV
REM Version of this batchfile is 2.0

cd .. 

REM Don't clobber old version if we forgot to update this file!
if exist dosmon12.zip goto gripe

pkzip -rp dosmon12.zip monitor\*.*
goto cleanup

:gripe 
echo Archive already exists. Remove, or edit distrib.bat for new version.
:cleanup
cd c:\src\monitor
