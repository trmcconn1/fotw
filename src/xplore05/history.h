/* Implements history list for forward/back in browser */

/* To allow this to be set from the compiler command line */
#ifndef MAX_HISTORY
#define MAX_HISTORY 50
#endif

/* The actual list is a buffer of pointers to strings (directory
paths returned by the browser). Both of these are defined in history.c. */

extern char *history[];
extern int n_hist; /* number of entries in history array */

extern int add_to_history(char *new_item); /* enter new item at end */
extern int free_history(int); 


