#define MAX_LEVEL 14
#define MAX_NODES  18384  /* should be 2^MAX_LEVEL */
#define OK 0
#define NOT_OK 1
#define MAX_MSG_LEN 256
#define VERSION "1.02" 
#define USAGE "mseq [  -n <terms> -o <param> -i <3-digits> -h -v ]"
#define HELP "mseq [-n <terms> -o <param> -i <3-digits> -h -v ]: run m-sequence analyzer.\n\
-n: use next argument as number of terms.\n\
-o: use next argument as optional parameter (see below.)\n\
-i: use next argument as the first three terms of the sequence (010 default)\n\
-h: print this helpful information\n\
-v: print version number and exit\n\n\
To do something useful you will need to edit the main program in mseq.c,\n\
including whatever utilities in util.c you need to do the job, and then\n\
recompile with cc -o mseq mseq.c util.c.\n\
The -o option allows you to supply a command line argument to modify\n\
the behaviour of the utilities in main.\n\n"

#define DEFAULT_MESSAGE "\nTo do anything useful with this program you will\n\
need to include one or more routines at the appropriate place in main and\n\
recompile. General usage information follows.\n\n"


#define START_STR "010"   /* First 3 terms of the sequence */

struct node_info {
unsigned long n;                /* Time this node is filled */
unsigned long n2;               /* 2nd time node is filled, or 0 */
int m;                          /* Match length */
unsigned char mc;               /* Match count: 0, 1, or 2 */
int depth;                      /* Number of pending excursions */
struct node_info *matches;      /* Pointer to node of match string */
struct node_info *matches_from; /* Pointer to first node matching here */
struct node_info *child0;
struct node_info *child1;       /* For making a tree */
struct node_info *parent;
};


extern struct node_info *nodes[];
extern struct node_info *node_at_time[]; 
extern char mseq[MAX_NODES+1]; 
extern unsigned long max_n;


extern struct node_info *alloc_node(void);
extern int init_node(struct node_info *);
extern int free_node(struct node_info *);
extern int dump_node(struct node_info *);
extern char *node_name(struct node_info *);
extern struct node_info *find_node(char *);
extern void fatal(char *,...);
extern void warn(char *,...);

/* Utilities: defined in util.c */
extern int print_mseq(unsigned long,unsigned long);
extern unsigned long powerof2(int);
extern unsigned long print_match_strings(int);
extern int match_order_report(int);
extern int equi_distrib_report(int,unsigned long);
extern int excursions(unsigned long, unsigned long);
extern int init_match_times(void);
extern int match_back_report(int len);
extern double compression(int n);
