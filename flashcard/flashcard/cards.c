/* cards.c: code to manage data-base of flash cards */

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cards.h"
#include <math.h>
#include "mtwist.h"  /* Use Mersenne twister as PRNG */

int selectionmode = RANDOM; /* Default mode is random */

// mtwist.c requires this to be implemented

void abort()
{
	exit(1);
	
}



int ncards;
int wrongtotal;

struct cardstruct mycards[MAX_CARDS];

int newcardnumber(int current)
{

	int j=0;
	double r,u=0.0;

	if(selectionmode == CONSECUTIVE)return (current+1) % ncards;

	/* Select a new card number at random, but in such
	a way that the probability a given card will be selected is
	proportional to the number of times it has been gotten wrong.
	*/
	r = mt_drand()*((double)wrongtotal);
	while(u<r){u += (double)(mycards[j].nincorrect); j++;}
	return j-1;
	
}

int coinflip()
{
	return mt_lrand()%2;
}
/* for testing */

void initializecards(){

	/* obselete testing stuff 
mycards[0].seqno = 0;
wcscpy(mycards[0].english, L"To deceive");
wcscpy(mycards[0].foreign,L"fallō, -ere, fefellī");
mycards[0].nincorrect = 3;
mycards[1].seqno = 1;
wcscpy(mycards[1].english, L"Noun: talk, report, rumor, reputation");
wcscpy(mycards[1].foreign,L"fāma, -ae");
mycards[1].nincorrect = 3;
mycards[2].seqno = 2;
wcscpy(mycards[2].english,L"servant (not servus)");
wcscpy(mycards[2].foreign, L"ancilla, -ae");
mycards[2].nincorrect = 3;
ncards = 3;
*/

}
