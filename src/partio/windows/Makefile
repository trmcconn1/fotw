##############################################################################
# Makefile for partitions (MS_DOS VERSION)
# Uncomment the appropriate section 
#              By Terry McConnell
##############################################################################
############## 16 bit Borland (large model)  #####################
CFLAGS = -O2 -ml -DN_MAX=108
LIBS = C:\bc45\lib\cl.lib fillib.lib 
CRT = C:\bc45\lib\c0l.obj
CC = bcc
##########################################

############# Do not Edit Below this Line ########################

OBJECTS = main.obj partio.obj next.obj manager.obj

.c.obj:
	$(CC) -c $(CFLAGS) $<


partio: $(OBJECTS) fillib.lib 
	tlink /Tde $(CRT) $(OBJECTS), partio,, $(LIBS)

all: partio

clean:
	del *.obj
	del fillib.lib

backup: 
	backup.bat
