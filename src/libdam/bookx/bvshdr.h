/* bvshdr HEADER (Bible Verse Structure) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* index for Bible-Verse-Structure (BVS) file */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	BVSHDR_INCLUDE
#define	BVSHDR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	BVSHDR			struct bvshdr_head
#define	BVSHDR_MAGICSIZE	16
#define	BVSHDR_MAGICSTR		"BVS"
#define	BVSHDR_VERSION		0


struct bvshdr_head {
	uint		fsz ;		/* file-size */
	uint		wtime ;		/* write-time */
	uint		nverses ;	/* number total verses */
	uint		nzverses ;	/* number non-zero verses */
	uint		nzbooks ;	/* number non-zero books */
	uint		btoff ;		/* book-table offset */
	uint		btlen ;		/* book-table length */
	uint		ctoff ;		/* chapter-table offset */
	uint		ctlen ;		/* chapter-table length */
	uchar		vetu[4] ;
} ; /* end struct (bvshdr_head) */

#ifdef	__cplusplus
struct bvshdr : bvshdr_head {
    	int rd		(char *,int)	noex ;
    	int wr		(cchar *,int)	noex ;
} ; /* end struct (bvshdr) */
#else /* __cplusplus */
typedef	BVSHDR		bvshdr ;
#endif /* __cplusplus */

EXTERNC_begin

extern int	bvshdr_rd(bvshdr *,char *,int) noex ;
extern int	bvshdr_wr(bvshdr *,cchar *,int) noex ;

EXTERNC_end


#endif /* BVSHDR_INCLUDE */


