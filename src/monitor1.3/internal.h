/* declarations of built-in (internal) commands, internal command typedef,
and jump table for internal commands. We have taken some care to
make the first letter of these commands unique so they can indexed
from a table of size 26 - bear this in mind if you decided to add
any new ones */

/* ptr to internal command type */

typedef int (*ICMD)(int, char **, char **);

extern int ninternals;

/* Here are the internal command declarations in alpha order */
/* The capitalization prevents namespace conflict */

extern int Echo(int,char **, char **);
extern int Goto(int,char **, char **);
extern int Help(int,char **, char **);
extern int If(int,char **, char **);
extern int Mem(int, char **, char **);
extern int Pop(int,char **, char **);
extern int Quit(int, char **, char **);
extern int Read(int, char **, char **);
extern int Set(int, char **, char **);

/* Here is the jump table declaration - implemented in internal.c */

extern ICMD internals[]; 
extern void initialize_jumptable(void);


