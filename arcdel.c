/*
 * ARC - Archive utility - ARCDEL
 * 
 * Version 2.09, created on 02/03/86 at 22:53:27
 * 
 * (C) COPYRIGHT 1985-87 by System Enhancement Associates.
 * You may copy and distribute this program freely,
 * under the terms of the General Public License.
 * 
 * By:  Thom Henderson
 * 
 * Description: This file contains the routines used to delete entries in an
 * archive.
 * 
 * Language: Computer Innovations Optimizing C86
 */
#include "arc.h"

VOID rempath(), openarc(), closearc();
int	match();

VOID
delarc(				/* remove files from archive */
	int	num,		/* number of arguments */
	char    *arg[]		/* pointers to arguments */
)
{
	struct heads    hdr;	/* header data */
	int             del;	/* true to delete a file */
	int             did[MAXARG];/* true when argument used */
	int             n;	/* index */

	if (!num)		/* she must specify which */
		arcdie("Please indicate which files to delete\n");

	for (n = 0; n < num; n++)	/* for each argument */
		did[n] = 0;	/* reset usage flag */
	rempath(num, arg);	/* strip off paths */

	openarc(1);		/* open archive for changes */

	while (readhdr(&hdr, arc)) {	/* while more entries in archive */
		del = 0;	/* reset delete flag */
		for (n = 0; n < num; n++) {	/* for each template given */
			if (match(hdr.name, arg[n])) {
				del = 1;	/* turn on delete flag */
				did[n] = 1;	/* turn on usage flag */
				break;	/* stop looking */
			}
		}

		if (del) {	/* skip over unwanted files */
			fseek(arc, hdr.size, 1);
			if (note)
				printf("Deleting file: %s\n", hdr.name);
		} else {	/* else copy over file data */
			writehdr(&hdr, new);	/* write out header and file */
			filecopy(arc, new, hdr.size);
		}
	}

	hdrver = 0;		/* special end of archive type */
	writehdr(&hdr, new);	/* write out archive end marker */
	closearc(1);		/* close archive after changes */

	if (note) {
		for (n = 0; n < num; n++) {	/* report unused arguments */
			if (!did[n]) {
				printf("File not found: %s\n", arg[n]);
				nerrs++;
			}
		}
	}
}
