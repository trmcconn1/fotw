/* Main program for crypt-like programs that use various historical
   encryption methods of historical interest.

	By Terry R. McConnell

   Handles things that don't depend on the particular method of encryption.
   Processes command line, gets password from user, handles terminal and
   file I/O. 

   Current version runs on Unix-like systems only.

*/

#include "global.h"
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

int getpasswd(char *key);

int encrypt_on = 1;  /* Flag: are we encrypting, or decrypting? This matters
                         with some encryption methods. */

/* Depending on the method used, we link in various implementations of
   the following functions. The first  must return the number of cypher text
   characters placed in the cypher_text buffer, or -1 in the event of
   an error.  It should be called with null arguments first to allow
   for initializations to be done. */

extern int my_encrypt(char *plain_text, char *cypher_text, int nchars);

/* Some methods need to control the size of blocks to be enciphered. For
   most methods the following routine will just store INPUT_BUFFER_SIZE
   through the pointer argument. 
   It is expected to return 0 if everything is OK
   and non-zero if there is an error.  */

extern int set_block_size(int *);

/* Contains the password (key) obtained from the user */
char key[MAX_KEY+1];	


int main(int argc, char **argv)
{
	char *input_buffer;    /* plain text */
	char *output_buffer;  /* cypher text */
	int input_block_size = INPUT_BUFFER_SIZE;
	int n,m,i;

	if( (argc > 1) && (strcmp(argv[1],"-d")==0)) {
		encrypt_on = 0;
		argc--;
		argv++; 
	}
		  

	/* If a password is given on the command line, we must 
		write over argv[1] so that the password is not visible to 
                ps(1) */

	if(argc > 2){
		fprintf(stderr,"Usage: %s %s \n",PROGRAM_NAME,USAGE);
		exit(1);
	}
	if(argc == 2) {
		strcpy(key,argv[1]);
		n = strlen(argv[1]);
		for(i=0;i<n;i++) argv[1][i] = '\0';
	}
	/* Else, we must get the password from the user */
	else
	if(MAX_KEY > 0)
	  if(getpasswd(key) == -1){
		my_error(" (Unable to get password) ");
		exit(1);
	  }

	/* Call encryption routine to let it do any initialization */
	if((m=my_encrypt(NULL,NULL,0))== -1){
			my_error("Error in encryption routine. \n");
			exit(1);
	}

	set_block_size(&input_block_size);
	input_buffer = (char *)malloc(input_block_size*sizeof(char));
	output_buffer = (char *)malloc(2*input_block_size*sizeof(char));
	if(!input_buffer || !output_buffer){
		my_error("Cannot allocate buffer memory.\n");
		exit(1);
	}

	/* Slurp in blocks of input, encrypt, and spew out */
	while(n = read(0,input_buffer,input_block_size)){
		if( n == -1){
			my_error("Read error on stdin\n");
			exit(1);
		}
		if((m=my_encrypt(input_buffer,output_buffer,n)) == -1){
			my_error("Error in encryption routine.\n");
			exit(1);
		}
		if(write(1,output_buffer,m) != m){
			my_error("Write error on stdout\n");
			exit(1);
		}
		/* Nothing currently uses it, but the block size could
                   vary dynamically */
#if 0
		set_block_size(&input_block_size);
#endif
	}

	free(input_buffer);
	free(output_buffer);
 	exit(0);
}

void my_error(char *msg)
{
	fprintf(stderr,"%s: %s",PROGRAM_NAME,msg);
}

#ifdef POSIX
#include <termios.h>
int getpasswd(char *key)
{
	int n=0;
	char c;
	int fd;
	char mask_char = '*'; /* Character to print in place of passwd char */
	struct termios my_termios;
	char key_msg[] = "Key: ";
	char back_space[] = "\b \b";
	char new_line = '\n';

	/* Open a handle to the console in case input is redirected */

	if((fd = open("/dev/tty",O_RDWR))==-1){
		my_error("Cannot open terminal\n");
		return -1;
	}
	if(tcgetattr(fd,&my_termios)!=0)
		return -1;
	my_termios.c_lflag &= ~ICANON; /* "Cbreak" mode */
	my_termios.c_lflag &= ~ECHO;  /* Don't echo typed characters */
	my_termios.c_cc[VMIN] = 1;   /* Reads return after each char */
	my_termios.c_cc[VTIME] = 0; /* Reads do not timeout */

	/* don't echo passwd to screen */

	if(tcsetattr(fd,TCSANOW,&my_termios)!=0)
		return -1; 

	write(fd,key_msg,strlen(key_msg));

	/* Aquire key characters, echo *'s to screen, and allow for
           primitive editing */

	while((read(fd,&c,1)==1)&&(c!='\n')&&(c != '\r')){
		if((c == my_termios.c_cc[VERASE])&&(n>0)){
			n--;
			key--;
			write(fd,back_space,strlen(back_space));
			continue;
		}	
		if(n++ >= MAX_KEY)break;
		*key++ = c;
		write(fd,&mask_char,1); 
	}
	*key = '\0';
	write(fd,&new_line,1);

	my_termios.c_lflag |= ECHO;
	my_termios.c_lflag |= ICANON;
	tcsetattr(fd,TCSADRAIN,&my_termios);
	close(fd);
	return 0;
}
#else
#include <sgtty.h>
int getpasswd(char *key)
{
	int n=0;
	char c;
	int fd;
	char key_msg[] = "Key: ";
	char back_space[] = "\b \b";
	struct sgttyb my_sgttyb;
 	char mask_char = '*'; /* Character to print in place of passwd char */
	char new_line = '\n';

	/* Open the console in case input is redirected */

	if((fd = open("/dev/tty",O_RDWR))==-1){
		my_error("Cannot open terminal\n");
		return -1;
	}

	if(ioctl(fd,TIOCGETP,&my_sgttyb)!=0)
		return -1;
	my_sgttyb.sg_flags &= ~ECHO;
	my_sgttyb.sg_flags |= CBREAK;

	if(ioctl(fd,TIOCSETP,&my_sgttyb)!=0)
		return -1;

 	write(fd,key_msg,strlen(key_msg));
	/* Aquire key characters, echo *'s to screen, and allow for
           primitive editing */

 	while((read(fd,&c,1)==1)&&(c!='\n')&&(c != '\r')){
		if((c == my_sgttyb.sg_erase)&&(n>0)){
			n--;
			key--;
			write(fd,back_space,strlen(back_space));
			continue;
		}	
		if(n++ >= MAX_KEY)break;
		*key++ = c;
		write(fd,&mask_char,1);
	}
	*key = '\0';
  	write(fd,&new_line,1);
	my_sgttyb.sg_flags |= ECHO;
	my_sgttyb.sg_flags &= ~CBREAK;
	ioctl(0,TIOCSETP,&my_sgttyb);
	close(fd);
	return 0;
}
#endif
