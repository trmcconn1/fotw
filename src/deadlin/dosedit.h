/* dosedit.h                                                       
*/

/* Control character equivalents to Dos Function Keys */
/* Only a few are standard -- I made the rest up */
/* For the real "edlin experience" you should change your kernel's keymap
   table to assign the function key scancodes to these control chars */

#define ESCAPE ''   /* Kills the current line */
#define BACKSPACE ''
#define TAB '	'
#define INS ''     /* Insert */
#define LINEFEED 0x0A
#define PRINTER_ECHO ''
#define F1 ''        /* Repeats previously entered line a char at a time */
#define F2 ''       /* Copy previous line up to next char entered */
#define F3 ''       /* Displays the previously entered line */
#define F4 ''  /* Copy previous line from next char entered onward */
#define F5 ''  /* Copy current line to previous line buffer */
#define F6 '' /*  This must be set to the end of file control char */
#define	QUIT 0x03  /* ^C: Leave input mode */
#define DEL  ''
#define LITERAL ''   /* Quotes ^V EOF and newline */
#define ALT ''   /* Allows entering extended chars from the numpad  */ 
