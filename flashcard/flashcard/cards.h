#define BUFFERSIZ 64

struct cardstruct {
	int seqno;
	wchar_t english[BUFFERSIZ];
	wchar_t foreign[BUFFERSIZ];
	int ncorrect;
	int nincorrect;
};




#define NATIVETOFOREIGN 0
#define FOREIGNTONATIVE 1
#define RANDOM 2
#define CONSECUTIVE 3

#define MAX_CARDS 1028



