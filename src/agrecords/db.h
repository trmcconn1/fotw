
#define MAX_DB_SIZE 65536
#define MAX_FIELD 128 /* even Roxanne ... */

struct db_entry {

	char *name;
	char *team;
	char sex;
	char *ag;
	char *event;
	char *date;
	double time;
	double pts;
	int type;
};

extern struct db_entry *my_db[];

extern int db_size;

extern struct db_entry *alloc_entry(int n1, int n2, int n3, int n4, int n5);
extern void sort_db_by_event(int);
extern void sort_db_by_ag(int);
extern void sort_db_by_name(int);
extern void sort_db_by_event_and_ag(int);
extern struct db_entry *search_db_by_event_and_ag(struct db_entry *);
extern void free_entry(struct db_entry *);
