/* txtindexhdr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* text-index hash file */
/* version %I% last-modified %G% */


/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	TXTINDEXHDR_INCLUDE
#define	TXTINDEXHDR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */


#define	TXTINDEXHDR		struct txtindexhdr_head
#define	TXTINDEXHDR_MAGICSIZE	16
#define	TXTINDEXHDR_MAGICSTR	"MKINVHASH"
#define	TXTINDEXHDR_VERSION	0


struct txtindexhdr_head {
	uint		hfsize ;	/* hash-file size */
	uint		tfsize ;	/* tag-file size */
	uint		ersize ;	/* eigen-record table size */
	uint		eisize ;	/* eigen-index table size */
	uint		wtime ;		/* write-time */
	uint		sdnoff ;	/* dirname offset */
	uint		sfnoff ;	/* filename offset */
	uint		listoff ;	/* list offset */
	uint		esoff ;		/* eigen-string table offset */
	uint		essize ;	/* eigen-string table length (size) */
	uint		eroff ;		/* eigen-record table offset */
	uint		erlen ;
	uint		eioff ;		/* eigen-index table offset */
	uint		eilen ;
	uint		eiskip ;	/* eigen-index table skip-factor */
	uint		taboff ;	/* main hash-table offset */
	uint		tablen ;	/* main hash-table length */
	uint		taglen ;	/* total number of tags */
	uint		maxtags ;	/* maximum tags in any list */
	uint		minwlen ;	/* minimum word length */
	uint		maxwlen ;	/* maximum word length */
	uchar		vetu[4] ;
} ; /* end struct */

#ifdef	__cplusplus
struct txtindexhdr : txtindexhdr_head {
    	int rd		(char *,int)	noex ;
    	int wr		(cchar *,int)	noex ;
} ; /* end struct (bvshdr) */
#else /* __cplusplus */
typedef	TXTINDEXHDR	txtindexhdr ;
#endif /* __cplusplus */

EXTERNC_begin

extern int	txtindexhdr_rd(txtindexhdr *,char *,int) noex ;
extern int	txtindexhdr_wr(txtindexhdr *,cchar *,int) noex ;

EXTERNC_end


#endif /* TXTINDEXHDR_INCLUDE */


