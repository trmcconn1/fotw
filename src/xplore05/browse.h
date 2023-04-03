#define TOP_DY 3 /* these include box chars */
#define BOT_DY 18
#define TOP_DX 80    /* Should be a multiple of 8 */
#define LEFT_DX 17   /* these two must add up to TOP_DX */
#define RIGHT_DX 63  
#define TOP_X 0
#define TOP_Y 3
#define RIGHT_COLS 3   /* number of columns in right pane display */
#define NTOP_PANEL 0
#define NLEFT_PANEL 1
#define NRIGHT_PANEL 2
#define FILESELECT 0
#define DIRSELECT 1

/* The maximum length of filenames displayed in the the right pane */
#define RIGHT_MAXLEN (RIGHT_DX-2)/RIGHT_COLS - 1

#define MAX_PATH 256

#ifdef NCURSES  /* ncurses can handle colors */
#define TOP_PAIR 1
#define LEFT_PAIR 2
#define RIGHT_PAIR 3
#define EDIT_PAIR 4
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

#define UNUSED_CHAR ''      /* This should be a char that has no meaning
                                 to the browser */

#include <sys/stat.h>
#define IS_EXECUTABLE(m) ((S_ISREG((m)))&&((m)&(S_IXUSR|S_IXGRP|S_IXOTH)))
#define IS_DIRECTORY(m)  (((m)&S_IFMT) == S_IFDIR)
#ifdef S_IFLNK
#define IS_LINK(m) (((m)&S_IFMT) == S_IFLNK)
#else 
#define IS_LINK(m) 0
#endif
#define IS_SPECIAL(m) (S_ISCHR((m))||S_ISBLK((m)))
#define IS_FIFO(m) (((m)&S_IFMT) == S_IFIFO)

#ifdef _MINIX
#define VERT ACS_VLINE
#define HOR ACS_HLINE
#else
#define VERT '|'
#define HOR '-'
#endif

#ifndef LPR
#define LPR "lpr"
#endif

/*  Characters for end of filenames for special files */
#define NORMAL_TAG ' '
#define EXEC_TAG '*'
#define DIR_TAG '/'
#define LINK_TAG '@'
#define SPECIAL_TAG '$'
#define FIFO_TAG '|'

/* Possible return values from the browser */
#define BROWSER_EXIT 0
#define BROWSER_RSEL 1
#define BROWSER_LSEL 9 
#define BROWSER_EDIT 2
#define BROWSER_ERROR 3
#define BROWSER_BACK 4
#define BROWSER_FWD 5
#define BROWSER_DUMP 6
#define BROWSER_MORE 7
#define BROWSER_MULTIPLE 8
#define BROWSER_NEVER -1    /* never returned from browser */

#ifdef NO_STRDUP
extern char *strdup(char *);
#endif
extern int browse(void *,char *);
