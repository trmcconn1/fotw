/*===========================================================================

  Prog    : WRBOOT
            write boot block on floppy disk that boots the monitor
	    then install monitor 

Originally by Claudio Tantignone

  Compile : bcc -ml -O -Z wini.c          

  usage   : wrboot

*/
  

/*===========================================================================*

	Funcionamiento del programa
	
	a) lee archivo bb.com, hace pad a 512 bytes y escribe boot
           block en floppy disk
*/
           
/*===========================================================================*/

#define  VERSION "2.0"         /* version */
#define USAGE "wrboot [-h -v -f -m]"
#define HELP "wrboot [-h -v -f -m] install monitor boot block on first hd partition"
#define HELP2 "-h: print this helpful message"
#define HELP3 "-v: print version number and exit"
#define HELP4 "-f: install on floppy a: instead"
#define HELP5 "-m: install an mbr that boots the active partition"

#include <dos.h>
#include <bios.h>
#include <alloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dir.h>
#include <mem.h>
#include <conio.h>   
#include <fcntl.h>
#include <sys\stat.h>

/*===========================================================================*/
/* defines */
/*===========================================================================*/

#define RESETDSK        0        /* function reset disk       */
#define GETSTATUS       1        /* function get drive status */
#define READVBLK        2        /* function read block       */
#define WRITEVBLK       3        /* function write disk block */
#define VERIFYTRK       4        /* function verify disk addr */
#define BLOCK           512      /* buffer size for block     */
/* These settings need to be compatible with fdbblk.asm */
#define BOOTBLOCK       "FDBBLK.BIN" /* boot code                 */
#define HDBOOTBLOCK     "HDBBLK.BIN"
#define MBRBLOCK        "MBRBLK.BIN"
#define MONITOR         "BOOTMON.COM" /* This will be booted */
#define MONITOR_KB      20   /* upper bound on size of monitor in k */
#define MONITOR_OFFSET  2    /* number of blocks to skip before monitor code 
                                is written on medium */


/*===========================================================================*/
/* variables */
/*===========================================================================*/

static unsigned char buffer[BLOCK];      /* general buffer for drive */
static unsigned char buffer2[BLOCK];      /* general buffer for drive */
static int debug=0;                      /* debug flag */

/*===========================================================================*
 *                              prterr                                       * 
 *===========================================================================*/
prterr(int byt)       /* print biosdisk error */
{
      fprintf(stderr,"\nwrboot: ");
      
      if ((byt & 0x80) != 0)
	 fprintf(stderr, "Disk timed-out.\n");
      else if ((byt & 0x40) != 0)
	 fprintf(stderr, "Seek failure.\n");
      else if ((byt & 0x20) != 0)
	 fprintf(stderr, "Controller error.\n");
      else if ((byt & 0x10) != 0)
	 fprintf(stderr, "Data error on disk read (CRC).\n");
      else if ((byt & 0x08) != 0)
	 fprintf(stderr, "DMA overrun on operation.\n");
      else if ((byt & 0x04) != 0)
	 fprintf(stderr, "Request sector not found.\n");
      else if ((byt & 0x02) != 0)
	 fprintf(stderr, "Disk write protected.\n");
      else if ((byt & 0x01) != 0)
	 fprintf(stderr, "Ilegal command passed to driver.\n");
      else printf("Status = 0x%x \n", byt);

   return(0);
}

/*===========================================================================*
 *                              main                                         * 
 *===========================================================================*/
int main(int argc, char **argv)
{
    FILE *fd;
    int byt;
    int n=0,track,sector,head,cylinder;
    int use_hd = 1;
    int install_mbr = 0;
    int j=0;

/* Process command line */

	while(++j < argc){
		if(argv[j][0] == '-')
			switch(argv[j][1]){ 
				case 'v':
				case 'V':
					printf("%s\n",VERSION);
					exit(0);
				case '?':
				case 'h':
				case 'H':
					printf("%s\n",HELP);
					printf("%s\n",HELP2);
					printf("%s\n",HELP3);
					printf("%s\n",HELP4);
					printf("%s\n\n",HELP5);
					exit(0);
				case 'f':
					use_hd = 0;
					break;
				case 'm':
					install_mbr = 1;
					break;
				default:
					fprintf(stderr,"wrboot: unkown option %s\n",
						argv[j]);
					exit(1);
			}
		else {
			fprintf(stderr,"wrboot:%s\n",USAGE);
			exit(1);
		}
	}

    printf("\nBoot Block Writer V%s - All rights reserved\n", VERSION);
    printf("1995 - C.M.T. Software Utilities Inc.\n");

    printf("wrboot: reading boot block image ...\n");

    if(use_hd){

/* read the bootblock of the hd into the buffer. Then we will copy over only
   the initial part of the bootblock, leaving the partition table in place */

    biosdisk(2,0x80,0,0,1,1,buffer);

    if ((fd = fopen(HDBOOTBLOCK,"rb")) == NULL) {
       perror("fopen boot file");
       exit(errno);
       }
    }
    else
    if ((fd = fopen(BOOTBLOCK,"rb")) == NULL) {
       perror("fopen boot file");
       exit(errno);
       }

    if (fread(buffer,sizeof(char),BLOCK,fd) == 0) {
       perror("fread boot file");
       exit(errno);
       }

    (void)fclose(fd);       /* close boot file */

   if(install_mbr){

/* read the mbr of the hd into the buffer. Then we will copy over only
   the initial part of the bootblock, leaving the partition table in place */

    biosdisk(2,0x80,0,0,1,1,buffer2);

    if ((fd = fopen(MBRBLOCK,"rb")) == NULL) {
       perror("fopen mbr file");
       exit(errno);
       }

    if (fread(buffer2,sizeof(char),446,fd) == 0) {
       perror("fread mbr file");
       exit(errno);
       }

    buffer2[510] = 0x55;
    buffer2[511] = 0xAA;

    (void)fclose(fd);       /* close boot file */
    }
    
    buffer[510] = 0x55;
    buffer[511] = 0xAA;

    if(use_hd)
    	printf("wrboot: writing monitor boot block into HD partition 1 ...\n");
    else
        printf("wrboot: writing monitor boot into floppy A drive ...\n");

/* int biosdisk(int cmd, int drive, int head, int track/cylinder, int sector
      int nsects, void *buffer) */

if(use_hd){
	if(install_mbr){
	   printf("wrboot: installing a master boot record...\n");
           byt = biosdisk(WRITEVBLK, 0x80, 0, 0, 1, 1, buffer2); /* mbr */
        }
       /* byt = biosdisk(WRITEVBLK, 0x80, 1, 0, 1, 1, buffer); */
}
else
       byt = biosdisk(WRITEVBLK, 0, 0, 0, 1, 1, buffer);

   /* si hubo error imprimo mensaje en pantalla */

   if (byt != 0) {
      prterr(byt);     /* print error */
      printf("\nwrboot: write error, please retry ...\n");
      exit(1);
      }

    printf("wrboot: boot block written\n");

    printf("wrboot: installing monitor.com on the device...\n");

    if ((fd = fopen(MONITOR,"rb")) == NULL) {
       perror("fopen monitor file");
       exit(errno);
       }

if(use_hd){
    head = 0;
    sector = MONITOR_OFFSET + 1; /* sectors count from 1, not 0 */
    n = 0;
    while(fread(buffer,sizeof(char),BLOCK,fd) == BLOCK) {
      byt = biosdisk(WRITEVBLK, 0x80, head, 0, sector, 1, buffer);
      sector++;
      n++;
    }
    byt = biosdisk(WRITEVBLK, 0x80, head, 0, sector, 1, buffer);
    if(byt)n++;
}
else {
    track = 0;
    sector = MONITOR_OFFSET + 1; /* sectors count from 1, not 0 */
    n = 0;
    while(fread(buffer,sizeof(char),BLOCK,fd) == BLOCK) {
      byt = biosdisk(WRITEVBLK, 0, 0, track, sector, 1, buffer);
      sector++;
      if(sector > 18){
	sector = 1;
	track++;
      }
      n++;
    }
    byt = biosdisk(WRITEVBLK, 0, 0, track, sector, 1, buffer);
    if(byt)n++;
}
    printf("wrboot: wrote %d blocks at offset %d blocks\n",n,MONITOR_OFFSET);

    (void)fclose(fd);       /* close monitor file */

}





