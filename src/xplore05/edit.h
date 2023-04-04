/*  minimal useful curses program header */
 
#include <curses.h>

extern int line_edit(WINDOW *,int,char *);

/* ANSI escape sequences */
#ifndef DOWN_ARROW
#define DOWN_ARROW "[B"
#endif
#ifndef RIGHT_ARROW
#define RIGHT_ARROW "[C"
#endif

#define UNUSED_CHAR ''

#ifndef MAX_NAME
#define MAX_NAME 1024
#endif

/* ANSI escape sequences w/o leading ^[ */
#define UP_ARROW "[A"
#define DOWN_ARROW "[B"
#define LEFT_ARROW "[D"
#define RIGHT_ARROW "[C"

/* These escape sequences seem to be sent by my keypad */
/* This is called alternate keypad mode or keypad application mode */
/* Note that those are Oh's not zeros */

#define UP_ARROW_AP "Ox"
#define RIGHT_ARROW_AP "Ov"
#define DOWN_ARROW_AP "Or"
#define LEFT_ARROW_AP "Ot"

/* Numerical codes that stand for these */
#define UP_CODE 1000
#define DOWN_CODE 1001
#define LEFT_CODE 1002
#define RIGHT_CODE 1003
