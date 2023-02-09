
#define MAX_DB_SIZE 65536
#define MAX_FIELD 1024

struct db_entry {

	char *ag;
	char *event;
	char *name;
	char *mark;
	char *date;
};

extern struct db_entry *my_db[];

extern int db_size;

extern struct db_entry *alloc_entry(int n1, int n2, int n3, int n4, int n5);
extern void sort_db_by_event();
extern void sort_db_by_ag();
extern void sort_db_by_name();
extern void free_entry(struct db_entry *);
