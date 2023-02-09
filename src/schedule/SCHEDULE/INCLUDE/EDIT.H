#include <stdlib.h>
#include <stdio.h>

#define DEFAULTLANDSCAPE  "&k2G&l1O(10U(s0p16.67h10V"
#define DEFAULTMESSAGE " <esc> = Menu Activate"
#define DEFAULTNORMALPRINTER  "&k2G"
#define SCREENLINES 22
#define MAIN_MENU_ITEMS 5
#define MAX_ITEMS 5          /* on a submenu */
#define MAIN_MENU_ITEM_WIDTH 8
#define PROMPTSTRING "input>"     // for dialogs
#define MAXLINES 5000
#define MAXBYTES 40000
#define LINELENGTH 80
#define BACKSPACE '\b'
#define DELETE '\x53'
#define INSERT 0x52
#define RIGHTARROW 0x4D
#define LEFTARROW 0x4B
#define DOWNARROW 0x50
#define UPARROW 0x48
#define LINEFEED '\x0A'
#define RETURN '\x0D'
#define ESCAPE ''
#define PAGEUP 0x49
#define PAGEDOWN 0x51
#define END 0x4F
#define HOME 0x47
#define CONTROL_HOME 0x77
#define CONTROL_END 0x75
#define CONTROL_PAGEUP 0x84
#define CONTROL_PAGEDOWN 0x76
#define CONTROL_RIGHTARROW 0x74
#define CONTROL_LEFTARROW 0x73
#define UPARROWSYMBOL 0x18
#define DOWNARROWSYMBOL 0x19
#define SCROLL_DOWN 0x07
#define SCROLL_UP 0x06
#define CONTROL_Z '\x1A'

/* Global Variables */

extern int lineno;
extern int X,Y;    
extern int TotalLines;  // Number of logical lines in file
extern char LineBuf[];
extern unsigned LinePositions[]; /* offsets of each logical line in input buffer*/
			       /* Logical lines count parts of lines which
				  are split off by word wrap   */
extern int TabStops[];
extern char MainBuffer[];
extern unsigned buf_pos;      // size of input file limited by this
extern unsigned  buf_end;
extern int cutbuf_pos;
extern int AlterFlag;
extern char Normalprinter[];
extern char Landscape[];

#ifdef __CSOURCE__
extern  char *bufgets(char *s, int n, const char *bp, unsigned int start, unsigned int size);
#endif

