/* bpihdr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Bible-Paragraph-Index */
/* version %I% last-modified %G% */


/* revision history:

	= 2007-03-01, David A­D­ Morano
	This code was originally written.

*/


/* Copyright © 2007 David A­D­ Morano.  All rights reserved. */

#ifndef	BPIHDR_INCLUDE
#define	BPIHDR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	BPIHDR			struct bpihdr_head
#define	BPIHDR_MAGICSIZE	16
#define	BPIHDR_MAGICSTR		"BIBLEPARAINDEX"
#define	BPIHDR_VERSION		0


struct bpihdr_head {
	uint		fsz ;		/* file-size */
	uint		wtime ;		/* write-time */
	uint		vioff ;		/* Verse-Index offset */
	uint		vilen ;		/* Verse-Index length */
	uint		nverses ;	/* number total verses */
	uint		nzverses ;	/* number non-zero verses */
	uint		maxbook ;	/* maximum book number */
	uint		maxchap ;	/* maximum chapter number */
	uchar		vetu[4] ;
} ; /* end struct */

#ifdef	__cplusplus
struct bpihdr : bpihdr_head {
    	int rd		(char *,int)	noex ;
    	int wr		(cchar *,int)	noex ;
} ; /* end struct (bvshdr) */
#else /* __cplusplus */
typedef	BPIHDR		bpihdr ;
#endif /* __cplusplus */


EXTERNC_begin

extern int	bpihdr_rd(bpihdr *,char *,int)	noex ;
extern int	bpihdr_wr(bpihdr *,cchar *,int)	noex ;

EXTERNC_end


#endif /* BPIHDR_INCLUDE */


