/* cmihdr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* index for Commandment-entry file */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CMIHDR_INCLUDE
#define	CMIHDR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	CMIHDR			struct cmihdr_head
#define	CMIHDR_MAGICSIZE	16
#define	CMIHDR_MAGICSTR		"CMDINDEX"
#define	CMIHDR_VERSION		0


struct cmihdr_head {
	uint		dbsz ;		/* DB-file size */
	uint		idxsz ;		/* IDX-file size */
	uint		dbtime ;	/* DB modification-time */
	uint		idxtime ;	/* IDX creation-time */
	uint		vioff ;
	uint		vilen ;
	uint		vloff ;
	uint		vllen ;
	uint		nents ;
	uint		maxent ;
	uchar		vetu[4] ;
} ; /* end struct */

#ifdef	__cplusplus
struct cmihdr : cmihdr_head {
    	int rd		(char *,int) noex ;
    	int wr		(cchar *,int) noex ;
} ; /* end struct (bvshdr) */
#else /* __cplusplus */
typedef	CMIHDR		cmihdr ;
#endif /* __cplusplus */

EXTERNC_begin

extern int	cmihdr_rd(cmihdr *,char *,int) noex ;
extern int	cmihdr_wr(cmihdr *,cchar *,int) noex ;

EXTERNC_end


#endif /* CMIHDR_INCLUDE */


