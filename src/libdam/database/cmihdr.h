/* cmihdr HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* index for Commandment-entry file */
/* version %I% last-modified %G% */


/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	CMIHDR_INCLUDE
#define	CMIHDR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	CMIHDR			struct cmihdr_head
#define	CMIHDR_MAGICSIZE	16
#define	CMIHDR_MAGICSTR		"CMDINDEX"
#define	CMIHDR_VERSION		0


struct cmihdr_head {
	uint		dbsize ;	/* DB-file size */
	uint		dbtime ;	/* DB modification-time */
	uint		idxsize ;	/* IDX-file size */
	uint		idxtime ;	/* IDX creation-time */
	uint		vioff ;
	uint		vilen ;
	uint		vloff ;
	uint		vllen ;
	uint		nents ;
	uint		maxent ;
	uchar		vetu[4] ;
} ;

typedef	CMIHDR		cmihdr ;

EXTERNC_begin

extern int	cmihdr_rd(cmihdr *,char *,int) noex ;
extern int	cmihdr_wr(cmihdr *,cchar *,int) noex ;

EXTERNC_end


#endif /* CMIHDR_INCLUDE */


