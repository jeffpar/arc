/*
 * ARC - Archive utility - ARCMATCH
 * 
 * Version 2.17, created on 12/17/85 at 20:32:18
 * 
 * (C) COPYRIGHT 1985-87 by System Enhancement Associates.
 * You may copy and distribute this program freely,
 * under the terms of the General Public License.
 * 
 * By:  Thom Henderson
 * 
 * Description: This file contains service routines needed to maintain an
 * archive.
 * 
 * Language: Computer Innovations Optimizing C86
 */
#include "arc.h"

int
match(char *n, char *t)	/* test name against template */
{
#if	_MTS
	fortran         patbuild(), patmatch(), patfree();
	static int      patlen = (-1);
	static int      patwork = 0;
	static int      patswch = 0;
	char            patccid[4];
	static char     patchar[2] = "?";
	static char     oldtemp[16] = 0;
	int             namlen, RETCODE;

	if (strcmp(t, oldtemp)) {
		if (patwork)
			patfree(&patwork);
		strcpy(oldtemp, t);
		patlen = strlen(oldtemp);
		patbuild(oldtemp, &patlen, &patwork, &patswch, patccid, patchar, _retcode RETCODE);
		if (RETCODE > 8) {
			printf("MTS: patbuild returned %d\n", RETCODE);
			arcdie("bad wildcard in filename\n");
		}
	}
	namlen = strlen(n);
	patmatch(n, &namlen, &patwork, _retcode RETCODE);
	switch (RETCODE) {
	case 0:
		return (1);
	case 4:
		return (0);
	default:
		arcdie("wildcard pattern match failed\n");
	}
}

#else
#if	!UNIX
	upper(n);
	upper(t);		/* avoid case problems */
#endif	/* UNIX */

	/* first match name part */

	while ((*n && *n != '.') || (*t && *t != '.')) {
		if (*n != *t && *t != '?') {	/* match fail? */
			if (*t != '*')	/* wildcard fail? */
				return 0;	/* then no match */
			else {	/* else jump over wildcard */
				while (*n && *n != '.')
					n++;
				while (*t && *t != '.')
					t++;
				break;	/* name part matches wildcard */
			}
		} else {	/* match good for this char */
			n++;	/* advance to next char */
			t++;
		}
	}

	if (*n && *n == '.')
		n++;		/* skip extension delimiters */
	if (*t && *t == '.')
		t++;

	/* now match name part */

	while (*n || *t) {
		if (*n != *t && *t != '?') {	/* match fail? */
			if (*t != '*')	/* wildcard fail? */
				return 0;	/* then no match */
			else
				return 1;	/* else good enough */
		} else {	/* match good for this char */
			n++;	/* advance to next char */
			t++;
		}
	}

	return 1;		/* match worked */
}
#endif

VOID
rempath(nargs, arg)		/* remove paths from filenames */
	int             nargs;	/* number of names */
	char           *arg[];	/* pointers to names */
{
	char           *i;	/* string index */
	int             n;	/* index */

	for (n = 0; n < nargs; n++) {	/* for each supplied name */
		if (!(i = rindex(arg[n], '\\')))	/* search for end of
							 * path */
			if (!(i = rindex(arg[n], '/')))
				i = rindex(arg[n], ':');
		if (i)		/* if path was found */
			arg[n] = i + 1;	/* then skip it */
	}
}
