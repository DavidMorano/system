/* strlisthdr HEADER */
/* lang=C20 */

/* string-list database-file header */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRLISTHDR_INCLUDE
#define	STRLISTHDR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
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
	uint		nskip ;		/* a hash-loookup parameter */
	char		vetu[4] ;
} ;

typedef STRLISTHDR	strlisthdr ;

EXTERNC_begin

extern int strlisthdr_msg(strlisthdr *,int,char *,int) noex ;
/* read from file-buffer to object */
extern int strlisthdr_rd(strlisthdr *,cchar *,int) noex ;
/* write to file-buffer from object */
extern int strlisthdr_wr(strlisthdr *,char *,int) noex ;

EXTERNC_end


#endif /* STRLISTHDR_INCLUDE */


