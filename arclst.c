/*
 * $Header: /cvsroot/arc/arc/arclst.c,v 1.2 2003/10/31 02:22:36 highlandsun Exp $
 */

/*  ARC - Archive utility - ARCLST
  
    Version 2.39, created on 04/22/87 at 13:48:29
  
    (C) COPYRIGHT 1985-87 by System Enhancement Associates.
    You may copy and distribute this program freely,
    under the terms of the General Public License.
  
    By:	 Thom Henderson
  
    Description:
	 This file contains the routines used to list the contents
	 of an archive.
  
    Language:
	 Computer Innovations Optimizing C86
*/
#include "arc.h"

VOID	rempath(), openarc(), closearc();
int	match();
VOID	lstfile();

VOID
lstarc(				/* list files in archive */
	int	num,		/* number of arguments */
	char	*arg[]		/* pointers to arguments */
)
{
	struct heads    hdr;	/* header data */
	int             list;	/* true to list a file */
	int             did[MAXARG];	/* true when argument was used */
	long            tnum, tlen, tsize;	/* totals */
	int             n;	/* index */

	tnum = tlen = tsize = 0;/* reset totals */

	for (n = 0; n < num; n++)	/* for each argument */
		did[n] = 0;	/* reset usage flag */
	rempath(num, arg);	/* strip off paths */

	if (note && !kludge) {
		printf("Name          Length  ");
		if (bose)
			printf("  Stowage    SF   Size now");
		printf("  Date     ");
		if (bose)
			printf("  Time    CRC");
		printf("\n");

		printf("============  ========");
		if (bose)
			printf("  ========  ====  ========");
		printf("  =========");
		if (bose)
			printf("  ======  ====");
		printf("\n");
	}
	openarc(0);		/* open archive for reading */

	if (num) {		/* if files were named */
		while (readhdr(&hdr, arc)) {	/* process all archive files */
			list = 0;	/* reset list flag */
			for (n = 0; n < num; n++) {	/* for each template
							 * given */
				if (match(hdr.name, arg[n])) {
					list = 1;	/* turn on list flag */
					did[n] = 1;	/* turn on usage flag */
					break;	/* stop looking */
				}
			}

			if (list) {	/* if this file is wanted */
				if (!kludge)
					lstfile(&hdr);	/* then tell about it */
				tnum++;	/* update totals */
				tlen += hdr.length;
				tsize += hdr.size;
			}
			fseek(arc, hdr.size, 1);	/* move to next header */
		}
	} else
		while (readhdr(&hdr, arc)) {	/* else report on all files */
			if (!kludge)
				lstfile(&hdr);
			tnum++;	/* update totals */
			tlen += hdr.length;
			tsize += hdr.size;
			fseek(arc, hdr.size, 1);	/* skip to next header */
		}

	closearc(0);		/* close archive after reading */

	if (note && !kludge) {
		printf("        ====  ========");
		if (bose)
			printf("            ====  ========");
		printf("\n");
	}
	if (note) {
		printf("Total %6ld  %8ld", tnum, tlen);
		if (bose) {
			if (tlen)
				printf("            %3ld%%", 100L - (100L * tsize) / tlen);
			else
				printf("            ---");
			printf("  %8ld", tsize);
		}
		printf("\n");

		for (n = 0; n < num; n++) {	/* report unused args */
			if (!did[n]) {
				printf("File not found: %s\n", arg[n]);
				nerrs++;
			}
		}
	}
}

VOID
lstfile(struct heads *hdr)		/* tell about a file */
{
	int             yr, mo, dy;	/* parts of a date */
	int             hh, mm;		/* parts of a time */

	static char    *mon[] =		/* month abbreviations */
	{"???",				/* For month == 0 */
	 "Jan", "Feb", "Mar", "Apr",
	 "May", "Jun", "Jul", "Aug",
	 "Sep", "Oct", "Nov", "Dec"
	};

	if (!note) {			/* no notes means short listing */
		printf("%s\n", hdr->name);
		return;
	}

	yr = (hdr->date >> 9) & 0x7f;	/* dissect the date */
	mo = (hdr->date >> 5) & 0x0f;
	if (mo > 12)			/* just in case... */
		mo = 0;
	dy = hdr->date & 0x1f;

	hh = (hdr->time >> 11) & 0x1f;	/* dissect the time */
	mm = (hdr->time >> 5) & 0x3f;
/*	ss = (hdr->time & 0x1f) * 2; */	/* seconds, not used */

	printf("%-12s  %8ld  ", hdr->name, hdr->length);

	if (bose) {
		switch (hdrver) {
		case 1:
		case 2:
			printf("   --   ");
			break;
		case 3:
			printf(" Packed ");
			break;
		case 4:
			printf("Squeezed");
			break;
		case 5:
		case 6:
		case 7:
			printf("crunched");
			break;
		case 8:
			printf("Crunched");
			break;
		case 9:
			printf("Squashed");
			break;
		default:
			printf("Unknown!");
		}

		if (hdr->length)
			printf("  %3ld%%", 100L - (100L * hdr->size) / hdr->length);
		else
			printf("  ---");
		printf("  %8ld  ", hdr->size);
	}
	printf("%2d %3s %02d", dy, mon[mo], (yr + 80) % 100);

	if (bose)
		printf("  %2d:%02d%c  %04x",
		       (hh > 12 ? hh - 12 : hh), mm, (hh > 11 ? 'p' : 'a'),
		       hdr->crc & 0xffff);

	printf("\n");
}
