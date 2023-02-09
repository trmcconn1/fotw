/* xo.c: simulation of tic tac toe to see how often we get cat's games */


#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>

#define VERSION "1.0"
#define USAGE "xo [ -s number -t number -V -h -v]"
#define HELP "xo [ -s number -t number -V -h -v]\n\n\
Print information on simulations of random tic tac toe games.\n\n\
-s: Use next argument as RNG seed. (otherwise use system time as seed.)\n\
-t: Use next argument as number of trials. Default 100.\n\
-V: Verbose -- print out every game\n\
-v: Print version number and exit. \n\
-h: Print this helpful information. \n\n"

/* Default values */
#define TRIALS 100
#define INITIAL_SEED 3445
#define TRUE 1
#define FALSE 0

static char board[3][3];

int
main(int argc, char **argv)
{
	int n,i,j=0,k;
	int xwin,owin;
	int verbose = FALSE;
	double p,cats=0.0;
	int trials = TRIALS;
	long seed=0;

	/* Process command line */
	while(++j < argc){
		if(argv[j][0] == '-')
			switch(argv[j][1]){ 
				case 's':
				case 'S':
					if(j+1 >= argc){
						fprintf(stderr,"%s\n",USAGE);
						exit(1);
					}
					seed = atol(argv[j+1]);
					j++;
					continue;
				case 't':
				case 'T':
					if(j+1 >= argc){
						fprintf(stderr,"%s\n",USAGE);
						exit(1);
					}
					trials = atoi(argv[j+1]);
					j++;
					continue;	
				case 'v':
					printf("%s\n",VERSION);
					exit(0);
				case 'V':
					verbose = TRUE;
					continue;
				case '?':
				case 'h':
				case 'H':
					printf("%s\n",HELP);
					exit(0);
				default:
					fprintf(stderr,"xo: unkown option %s\n",
						argv[j]);
					exit(1);
			}
		else {
			fprintf(stderr,"%s\n",USAGE);
			exit(1);
		}
	}
 
	/* If no seed is supplied, then use current system time */
	
	if(!seed)
		if((seed = (long)time(NULL)) == -1){
			seed = INITIAL_SEED; /* if all else fails */
			fprintf(stderr, "Using seed = %d\n",INITIAL_SEED);
		}
		
	srandom(seed);
	printf("Simulating %d trials ...\n",trials);
	n=0;
	while(n++<trials){   /* Loop over trials */

		/* reseed */
		seed = random();
		srandom(seed);

		/* Wipe the board */
		for(i=0;i<3;i++)
			for(j=0;j<3;j++)
				board[i][j]='\0';

		/* k is move number */
		for(k=0;k<9;k++)
			while(1){
				i = random()%3;
				j = random()%3;
				if(board[i][j])continue;
				if(k%2) /* X's move */
					board[i][j] = 'X';
				else
					board[i][j] = 'O';
				break;
			}

		/* Print out the board */
		if(verbose==TRUE){
			for(i=0;i<3;i++)
				printf("%c|%c|%c\n",board[i][0],board[i][1],
					board[i][2]);
			printf("----------\n");
		}

		/* See if anybody won */
		xwin = owin = FALSE;
		for(i=0;i<3;i++)
			if((board[i][0]==board[i][1])&&
					(board[i][1]==board[i][2]))
				if(board[i][0]=='X')
					xwin = TRUE;
				else    owin = TRUE;
		for(j=0;j<3;j++)
			if((board[0][j]==board[1][j])&&
					(board[1][j]==board[2][j]))
				if(board[0][j]=='X')
					xwin = TRUE;
				else    owin = TRUE;


		if((board[0][0]==board[1][1])&&(board[1][1]==board[2][2]))
			if(board[0][0]=='X')
				xwin = TRUE;
			else owin = TRUE;

		if((board[0][2]==board[1][1])&&(board[1][1]==board[2][0]))
			if(board[0][0]=='X')
				xwin = TRUE;
			else owin = TRUE;


	        if((xwin == FALSE)&&(owin==FALSE)) 
			cats = cats+1.0;
		
	}

	p = cats/((double)n);

	/* Print stuff out */

	printf("Cat game probability = %5.3f\n",p);

	return 0;
}

