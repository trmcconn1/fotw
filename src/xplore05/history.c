/* history.c: Implements history list for forward/back in browser */

#include "history.h"
#include <stdio.h>
#include <stdlib.h>

/* The actual list is a buffer of pointers to strings (directory
paths returned by the browser.) N.B. these pointers must reference
dynamically allocated memory because we free them as needed. */

char *history[MAX_HISTORY];
int n_hist; /* number of entries in history array */
	    /* index of current item, cur_hist, is defined and maintained
               externally */

/* The following routine is used only by add_to_history (below) when
   the history list capacity has been reached. It shifts everything down,
   losing the bottom element. Left means down. */

static int left_shift_history(char *new_item){ /* enter new item at top */

	int i;

	if(n_hist<=0) return 1; /* no can do */

	free(history[0]);
	for(i=0;i<n_hist-1;i++)
		history[i]=history[i+1];

	history[n_hist-1] = new_item;
	return 0;
}

/* Add new history item at top of list (assuming there is room.) Update
   n_hist. */

int add_to_history(char *new_item) {

	if(n_hist < MAX_HISTORY) 
		history[n_hist++] = new_item;
	else
		left_shift_history(new_item);
	return 0;
}
		
/* Free all history entries after the nth one. Update n_hist. */

int free_history(int n){ 

	int j;
	for(j=n+1;j<n_hist;j++)
		free(history[j]);
	n_hist = n+1;
	return 0;
}


