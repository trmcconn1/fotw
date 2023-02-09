#define MY_NORMAL 0
#define MY_EOL 1
#define MY_EOF 2
#define MY_ERROR 3
#define STATE_CHANGE 4
#define FIELD_SEP ','

extern void parse_error(char *);
extern char * parse(FILE *, int *);
