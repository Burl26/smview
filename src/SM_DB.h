typedef struct dbstruct {
	char line[257]; // holds the pre-parsed string
	int title;		// these can't be pointers because qsort messes up pointers
	int	artist;
	int S_artist;
	int S_title;
	int pdf;
	int timestamp;
	long index;
} DBSTRUCT;

// double-click timeout callback
static void dbl_click_chk(void *v) {
	int *d = (int *)v;
	*d = 0;
}

//quicksort callbacks
int sort_title(const void *a, const void *b);
int sort_recent(const void *a, const void *b);
int sort_artist(const void *a, const void *b);
// reads the csv file
int read_csv(DBSTRUCT **db, long *dbcount, char *fname, char *errbuf, long errbuflen);
int write_csv(DBSTRUCT **db, long *dbcount, char *fname, char *errbuf, long errbuflen);
