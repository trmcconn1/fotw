/*  minimal useful curses program header */
 
#include <curses.h>

/* ANSI escape sequences */
#ifndef DOWN_ARROW
#define DOWN_ARROW "[B"
#endif
#ifndef RIGHT_ARROW
#define RIGHT_ARROW "[C"
#endif

#ifndef MAX_NAME
#define MAX_NAME 1024
#endif

extern int line_edit(WINDOW *, int, char *);
