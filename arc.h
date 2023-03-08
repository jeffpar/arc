#include <stdio.h>
#if	UNIX
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#endif

#include <string.h>
#if BSD
#include <strings.h>
#endif

#if	_MTS
#include <mts.h>
#endif

#undef	DOS	/* Just in case... */
#undef	UNIX

/*
 * Assumptions:
 * char = 8 bits
 * short = 16 bits
 * long = 32 bits
 * int >= 16 bits
 */

#if	MSDOS || GEMDOS
#define	DOS	1
#define	CUTOFF	'\\'
#define	OPEN_R	"rb"
#define	OPEN_W	"wb"
#if	__GNUC__
#include <types.h>
#include <string.h>
#endif
#endif

#if	!MSDOS
#define	envfind	getenv
#define	setmem(a, b, c)	memset(a, c, b)
#endif

#if	BSD || SYSV
#define	UNIX	1
#define	CUTOFF	'/'
#define	OPEN_R	"r"
#define	OPEN_W	"w"
#include <ctype.h>
#include <sys/types.h>
#endif

#if	_MTS
#define	CUTOFF	sepchr[0]
#define	OPEN_R	"rb"
#define	OPEN_W	"wb"
#endif

#define	MYBUF	32766		/* Used for fopens and filecopy() */

#if	_MTS || SYSV
#define	rindex	strrchr
#define	index	strchr
#endif

#if	__STDC__
#include <stdlib.h>
#define	VOID	void
#define	PROTO(args)	args
#else
#define	VOID	int
#define	PROTO(args)	()
#endif

/*  ARC - Archive utility - ARC Header
  
    Version 2.17, created on 04/22/87 at 13:09:43
  
    (C) COPYRIGHT 1985-87 by System Enhancement Associates.
    You may copy and distribute this program freely,
    under the terms of the General Public License.
  
    By:	 Thom Henderson
  
    Description: 
	 This is the header file for the ARC archive utility.  It defines
	 global parameters and the references to the external data.
  
  
    Language:
	 Computer Innovations Optimizing C86
*/

#define ARCMARK 26		/* special archive marker        */
#define ARCVER 9		/* archive header version code   */
#define STRLEN 100		/* system standard string length */
#define FNLEN 13		/* file name length              */
#define MAXARG 400		/* maximum number of arguments   */

#include "arcs.h"

#if	!UNIX
typedef unsigned int	u_int;
#ifndef	__GNUC__
typedef unsigned char	u_char;
typedef unsigned short	u_short;
#endif
#endif
#define	reg	register

extern int      keepbak;	/* true if saving the old archive */
#if	!DOS
extern int      image;		/* true to suppress CRLF/LF x-late */
#endif
#if	_MTS
extern char     sepchr[2];	/* Shared file separator, default = ':' */
extern char     tmpchr[2];	/* Temporary file prefix, default = '-' */
#endif
#if	GEMDOS
extern int      hold;		/* hold screen before exiting */
#endif
extern int      warns;		/* true to print warnings */
extern int      note;		/* true to print comments */
extern int      bose;		/* true to be verbose */
extern int      nocomp;		/* true to suppress compression */
extern int      overlays;	/* true to overlay on extract */
extern int      kludge;		/* kludge flag */
extern char    *arctemp;	/* arc temp file prefix */
extern char    *password;	/* encryption password pointer */
extern int      nerrs;		/* number of errors encountered */
extern int      changing;	/* true if archive being modified */

extern char     hdrver;		/* header version */

extern FILE    *arc;		/* the old archive */
extern FILE    *new;		/* the new archive */
extern char     arcname[STRLEN];/* storage for archive name */
extern char     bakname[STRLEN];/* storage for backup copy name */
extern char     newname[STRLEN];/* storage for new archive name */
extern u_short	arcdate;	/* archive date stamp */
extern u_short	arctime;	/* archive time stamp */
extern u_short	olddate;	/* old archive date stamp */
extern u_short	oldtime;	/* old archive time stamp */
extern int      dosquash;	/* squash instead of crunch */

#define arcdie(...)  	{ fprintf(stderr, __VA_ARGS__); exit(1); }

#if	!__STDC__
char		*calloc(), *malloc(), *realloc();
#endif

VOID addarc(			/* add files to archive */
	int	num,		/* number of arguments */
	char	*arg[],		/* pointers to arguments */
	int	move,		/* true if moving file */
	int	update,		/* true if updating */
	int	fresh		/* true if freshening */
);

VOID
delarc(				/* remove files from archive */
	int	num,		/* number of arguments */
	char    *arg[]		/* pointers to arguments */
);

VOID
extarc(				/* extract files from archive */
	int	num,		/* number of arguments */
	char	*arg[],		/* pointers to arguments */
	int	prt		/* true if printing */
);

VOID
lstarc(				/* list files in archive */
	int	num,		/* number of arguments */
	char	*arg[]		/* pointers to arguments */
);

VOID	tstarc(void);		/* test archive */

VOID
cvtarc(				/* convert archive */
	int	num,		/* number of arguments */
	char	*arg[]		/* pointers to arguments */
);

VOID
runarc(				/* run file from archive */
	int	num,		/* number of arguments */
	char	*arg[]		/* pointers to arguments */
);

VOID
expandlst(			/* expand an indirect reference */
	int	n
);

/* arcio.c */

int
readhdr(			/* read a header from an archive */
	ARCHDR	*hdr,		/* storage for header */
	FILE	*f		/* archive to read header from */
);

VOID
writehdr(			/* write a header to an archive */
	ARCHDR	*hdr,		/* header to write */
	FILE	*f		/* archive to write to */
);

VOID
filecopy(			/* bulk file copier */
	FILE		*f,	/* files from and to */
	FILE		*t,
	long            size	/* bytes to copy */
);

/* arcmisc.c */

char *
makefnam(char *rawfn, char *template, char *result);

VOID
upper(char *string);

#if	_MTS
VOID	etoa();
#endif
