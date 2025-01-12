/*
 * ARC - Archive utility - ARCCVT
 * 
 * Version 1.16, created on 02/03/86 at 22:53:02
 * 
 * (C) COPYRIGHT 1985-87 by System Enhancement Associates.
 * You may copy and distribute this program freely,
 * under the terms of the General Public License.
 * 
 * By:  Thom Henderson
 * 
 * Description: This file contains the routines used to convert archives to use
 * newer file storage methods.
 * 
 * Language: Computer Innovations Optimizing C86
 */
#include "arc.h"

int	match(), unpack();
VOID	filecopy(), openarc(), rempath(), closearc(), pack();
static	VOID	cvtfile();

static char     tempname[STRLEN];	/* temp file name */

VOID
cvtarc(				/* convert archive */
	int	num,		/* number of arguments */
	char	*arg[]		/* pointers to arguments */
)
{
	struct heads    hdr;	/* file header */
	int             cvt;	/* true to convert current file */
	int             did[MAXARG];/* true when argument was used */
	int             n;	/* index */

	if (arctemp)		/* use temp area if specified */
		sprintf(tempname, "%s.CVT", arctemp);
	else
		makefnam("$ARCTEMP.CVT", arcname, tempname);
#if	!DOS
	image = 1;
#endif

	openarc(1);		/* open archive for changes */

	for (n = 0; n < num; n++)	/* for each argument */
		did[n] = 0;	/* reset usage flag */
	rempath(num, arg);	/* strip off paths */

	if (num) {		/* if files were named */
		while (readhdr(&hdr, arc)) {	/* while more files to check */
			cvt = 0;/* reset convert flag */
			for (n = 0; n < num; n++) {	/* for each template
							 * given */
				if (match(hdr.name, arg[n])) {
					cvt = 1;	/* turn on convert flag */
					did[n] = 1;	/* turn on usage flag */
					break;	/* stop looking */
				}
			}

			if (cvt)/* if converting this one */
				cvtfile(&hdr);	/* then do it */
			else {	/* else just copy it */
				writehdr(&hdr, new);
				filecopy(arc, new, hdr.size);
			}
		}
	} else
		while (readhdr(&hdr, arc))	/* else convert all files */
			cvtfile(&hdr);

	hdrver = 0;		/* archive EOF type */
	writehdr(&hdr, new);	/* write out our end marker */
	closearc(1);		/* close archive after changes */

	if (note) {
		for (n = 0; n < num; n++) {	/* report unused args */
			if (!did[n]) {
				printf("File not found: %s\n", arg[n]);
				nerrs++;
			}
		}
	}
}

static	VOID
cvtfile(hdr)			/* convert a file */
	struct heads   *hdr;	/* pointer to header data */
{
	long            starts;	/* where the file goes */
	FILE           *tmp;	/* temporary file */

	if (!(tmp = fopen(tempname, "w+b")))
		arcdie("Unable to create temporary file %s\n", tempname);

	if (note) {
		printf("Converting file: %-12s   reading, ", hdr->name);
		fflush(stdout);
	}
	unpack(arc, tmp, hdr);	/* unpack the entry */
	fseek(tmp, 0L, 0);	/* reset temp for reading */

	starts = ftell(new);	/* note where header goes */
	hdrver = ARCVER;		/* anything but end marker */
	writehdr(hdr, new);	/* write out header skeleton */
#if	_MTS
	atoe(hdr->name, FNLEN);	/* writehdr translated this... */
#endif
	pack(tmp, new, hdr);	/* pack file into archive */
	fseek(new, starts, 0);	/* move back to header skeleton */
	writehdr(hdr, new);	/* write out real header */
	fseek(new, hdr->size, 1);	/* skip over data to next header */
	fclose(tmp);		/* all done with the file */
	if (unlink(tempname) && warns) {
		printf("Cannot unsave %s\n", tempname);
		nerrs++;
	}
}
