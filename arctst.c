/*
 * ARC - Archive utility - ARCTST
 * 
 * Version 2.12, created on 02/03/86 at 23:00:40
 * 
 * (C) COPYRIGHT 1985-87 by System Enhancement Associates.
 * You may copy and distribute this program freely,
 * under the terms of the General Public License.
 * 
 * By:  Thom Henderson
 * 
 * Description: This file contains the routines used to test archive integrity.
 * 
 * Language: Computer Innovations Optimizing C86
 */
#include "arc.h"

VOID	openarc();
int	unpack();

VOID
tstarc(void)
{				/* test integrity of an archive */
	struct heads    hdr;	/* file header */
	long            arcsize, ftell();	/* archive size */

	openarc(0);		/* open archive for reading */
	fseek(arc, 0L, 2);	/* move to end of archive */
	arcsize = ftell(arc);	/* see how big it is */
	fseek(arc, 0L, 0);	/* return to top of archive */

	while (readhdr(&hdr, arc)) {
		if (ftell(arc) + hdr.size > arcsize) {
			printf("Archive truncated in file %s\n", hdr.name);
			nerrs++;
			break;
		} else {
			printf("Testing file: %-12s  ", hdr.name);
			fflush(stdout);
			if (unpack(arc, NULL, &hdr))
				nerrs++;
			else
				printf("okay\n");
		}
	}

	if (nerrs < 1)
		printf("No errors detected\n");
	else if (nerrs == 1)
		printf("One error detected\n");
	else
		printf("%d errors detected\n", nerrs);
}
