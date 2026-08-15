/* cyihdr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Calendar-Year-Index DB header (text-index for CYI-INDEX file) */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CYIHDR_INCLUDE
#define	CYIHDR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	CYIHDR			struct cyihdr_head
#define	CYIHDR_MAGICSZ		16
#define	CYIHDR_MAGICSTR		"CALENDARINDEX"
#define	CYIHDR_VERSION		0


struct cyihdr_head {
	uint		fsz ;
	uint		wtime ;
	uint		diroff ;
	uint		caloff ;
	uint		vioff ;
	uint		vilen ;
	uint		vloff ;
	uint		vllen ;
	uint		nentries ;
	uint		nskip ;
	uint		year ;		/* the year index was made for */
	uchar		vetu[4] ;
} ; /* end struct */

#ifdef	__cplusplus
struct cyihdr : cyihdr_head {
    	int rd		(char *,int) noex ;
    	int wr		(cchar *,int) noex ;
} ; /* end struct (bvshdr) */
#else /* __cplusplus */
typedef	CYIHDR		cyihdr ;
#endif /* __cplusplus */

EXTERNC_begin

extern int cyihdr_rd(cyihdr *,char *,int) noex ;
extern int cyihdr_wr(cyihdr *,cchar *,int) noex ;

EXTERNC_end


#endif /* CYIHDR_INCLUDE */


