/*
Author: Burl26

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/

#include "SM_DB.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


char* CSVstrtok(char *s) {
	char *p;
	static char *nxtp;
	int i;
	if (s) p=s; else {
		if (NULL == nxtp) return NULL;		
		p = nxtp;
	}
	for (i=0; p[i]!=0; i++) {
		if ((p[i] == ',') || (p[i] == '\r') || (p[i] == '\n')) {
			nxtp = &p[i+1];
			p[i] = 0;
			return p;
		}
	}
	p = nxtp;
	nxtp = NULL;
	return p;
}
int write_csv(DBSTRUCT **ldb, long *dbcount, char *fname, char*csverr, long ERROR_BUFFER) {
	FILE *f;
	DBSTRUCT *db;

	db = *ldb;
	f = fopen(fname,"w");
	if (f==NULL) {
		sprintf(csverr, "Could not open CSV file '%s'", fname);
		return 1;	
	}
	for (long i=0; i<(*dbcount); i++) {
		fprintf(f,"%s,%s,%s,%s,%s,%s\n",
		&db[i].line[db[i].title],
		&db[i].line[db[i].artist],
		&db[i].line[db[i].S_title],
		&db[i].line[db[i].S_artist],
		&db[i].line[db[i].pdf],
		&db[i].line[db[i].timestamp]);
	}
	fclose(f);
#ifdef _DEBUG
	printf("%ld records written", *dbcount);
#endif
	return 0;
}

int read_csv(DBSTRUCT **ldb, long *dbcount, char *fname, char*csverr, long ERROR_BUFFER) {
	DBSTRUCT *db;
	FILE *f;
	char line_in[256];
	long i, iline;
	char *token;
	char lineerr[256];
	char lineid[32];

	csverr[0] = 0;
	*dbcount = 0;
	*ldb = NULL;
	f = fopen(fname,"r");
	if (f==NULL) {
		sprintf(csverr, "Could not open CSV file '%s'", fname);
		return 1;	
	}
	//start at the top.  Do not put a header in your CSV file
	iline=1;
	for (*dbcount=0; !feof(f); (*dbcount)++) {
		fgets(line_in,256,f);
		if (strlen(line_in) >= 254) {
			fclose(f);
			dbcount = 0;
			sprintf(csverr, "Line %ld: Line is too long in CSV file", iline);
			return 1;	// line is too long
		}
		iline++;
	}
	(*dbcount)--;
	if (*dbcount <= 0) {
		fclose(f);
		*dbcount = 0;
		sprintf(csverr, "Empty CSV file");
		return 1;	
	}
	// allocate memory for database
	db = (DBSTRUCT *)calloc(*dbcount,sizeof(DBSTRUCT));
	if (db == NULL) {
		fclose(f);
		sprintf(csverr, "Memory error reading %ld lines from CSV file", *dbcount);
		*dbcount = 0;
		return 1;	
	}
	// re-read
	rewind(f);
	iline = 1;
	for (i=0; i<*dbcount; i++) {
		lineerr[0] = 0;
		db[i].line[0] = 0; // leading null
		fgets(&db[i].line[1],256,f);
		// start parsing
		token = CSVstrtok(&db[i].line[1]);
		if (token) db[i].title = token - db[i].line; else db[i].title = 0; 
		token = CSVstrtok(NULL);
		if (token) db[i].artist = token - db[i].line; else db[i].artist = 0; 
		token = CSVstrtok(NULL);
		if (token) db[i].S_title = token - db[i].line; else db[i].S_title = 0; 
		token = CSVstrtok(NULL);
		if (token) db[i].S_artist = token - db[i].line; else db[i].S_artist = 0;
		token = CSVstrtok(NULL);
		if (token) db[i].pdf = token - db[i].line; else db[i].pdf = 0; 
		token = CSVstrtok(NULL);
		if (token) db[i].timestamp = token - db[i].line; else db[i].timestamp = 0;
		db[i].index = i;
		// failure detected
		if (db[i].line[db[i].title] == 0) strcat(lineerr, "<title> ");
		if (db[i].line[db[i].artist] == 0) strcat(lineerr, "<artist> ");
		if (db[i].line[db[i].S_title] == 0) strcat(lineerr, "<S_TITLE> ");
		if (db[i].line[db[i].S_artist] == 0) strcat(lineerr, "<S_ARTIST> ");
		if (db[i].line[db[i].pdf] == 0) strcat(lineerr, "<PDF> ");
		if (lineerr[0]) {
			// blank line detected
			sprintf(lineid, "Line %ld: ", iline);		// max length is 13 (with 6 digit number)
			strncat(csverr, lineid, ERROR_BUFFER);
			if (strlen(lineerr) == 44) 
				strncat(csverr, "Empty line detected\n", ERROR_BUFFER);
			else {
				strncat(csverr, "Missing ", ERROR_BUFFER);
				strncat(csverr, lineerr, ERROR_BUFFER);	// max is 44 
				strncat(csverr, "\n", ERROR_BUFFER);	// max is 44 
			}
			// detect too many errors
			if (strlen(csverr) >= (size_t)(ERROR_BUFFER - 60)) {
				strncat(csverr, "\nTOO MANY ERRORS!\n", ERROR_BUFFER);
				// abort if too many errors
				fclose(f);
				*dbcount = 0;
				free(db);
				db = NULL;
				return 1;
			}
		}
		iline++;
	}
	fclose(f);
	*ldb = db;
	return (strlen(csverr)); 
}

int sort_title(const void *a, const void *b) {
	DBSTRUCT *da, *db;
	da = (DBSTRUCT *)a;
	db = (DBSTRUCT *)b;
	if (da->S_title == 0) return -1;
	if (db->S_title == 0) return 1;
	return(strcmp(&da->line[da->S_title], &db->line[db->S_title]));
}

int sort_recent(const void *a, const void *b) {
	DBSTRUCT *da, *db;
	da = (DBSTRUCT *)a;
	db = (DBSTRUCT *)b;
	if (da->timestamp == 0) return -1;
	if (db->timestamp == 0) return 1;
	return(strcmp(&da->line[da->timestamp], &db->line[db->timestamp]));
}

int sort_artist(const void *a, const void *b) {
	int i;
	DBSTRUCT *da, *db;
	da = (DBSTRUCT *)a;
	db = (DBSTRUCT *)b;
	// first sort artist
	if (da->S_artist == 0) return -1;
	if (db->S_artist == 0) return 1;
	i = strcmp(&da->line[da->S_artist], &db->line[db->S_artist]);
	if (i != 0) return i;
	// then sort by title
	if (da->S_title == 0) return -1;
	if (db->S_title == 0) return 1;
	return(strcmp(&da->line[da->S_title], &db->line[db->S_title]));
}

