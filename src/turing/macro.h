/* header file for associative array of macros (see macro.h) */

#define MAX_MACRO_TEXT 16384
#define MAX_MACRO_NAME 256
#define MAX_MACRO_ARGS 16

struct macro_struct {
	char *name;
	char *value;
	int argc;
	char **argv;
};

extern struct macro_struct *getmacro(char *name);
extern struct macro_struct *addmacro(struct macro_struct *);
extern int rmmacro(char *name); 
extern char *expand_macro(struct macro_struct *,char *);
extern void dump_macros();

