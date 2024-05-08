/* strlisthdr HEADER */
/* lang=C20 */

/* string-list database-file header */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRLISTHDR_INCLUDE
#define	STRLISTHDR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<localmisc.h>


#define	STRLISTHDR		struct strlisthdr_head

#define	STRLISTHDR_MAGICSTR	"STRLISTHDR"
#define	STRLISTHDR_FSUF		"si"
#define	STRLISTHDR_MAGICLEN	sizeof(STRLISTHDR_MAGICSTR)
#define	STRLISTHDR_MAGICSIZE	16
#define	STRLISTHDR_VERSION	0


struct strlisthdr_head {
	uint		fsize ;		/* file size */
	uint		wtime ;		/* write time (creation time?) */
	uint		stoff ;		/* string table offset */
	uint		stlen ;		/* string table length (n-entries) */
	uint		rtoff ;		/* record-table offset */
	uint		rtlen ;		/* record-table length (n-entries) */
	uint		itoff ;		/* index-table offset */
	uint		itlen ;		/* index-table length (n-entries) */
	uint		nstrs ;		/* total number of strings */
	uint		nskip ;		/* a hash-lookup parameter */
	char		vetu[4] ;
} ;

typedef STRLISTHDR	strlisthdr ;

EXTERNC_begin

/* read from object; into the supplied buffer (like |read(2)|) */
extern int strlisthdr_rd(strlisthdr *,char *,int) noex ;
/* write to  object; from file-buffer to the object (like |write(2)|) */
extern int strlisthdr_wr(strlisthdr *,cchar *,int) noex ;

EXTERNC_end


#endif /* STRLISTHDR_INCLUDE */


