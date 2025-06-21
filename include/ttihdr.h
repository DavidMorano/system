/* ttihdr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Termial-Translate-Index (TTI) file management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	TTIHDR_INCLUDE
#define	TTIHDR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	TTIHDR			struct ttihdr_head
#define	TTIHDR_MAGICSIZE	16
#define	TTIHDR_MAGICSTR		"TERMTRANSINDEX"
#define	TTIHDR_VERSION		0


struct ttihdr_head {
	uint		fsize ;		/* file-size */
	uint		ctime ;		/* create-time */
	uint		rectab ;	/* record-table */
	uint		reclen ;	/* recotd-table-length */
	uint		ostrtab ;	/* overflow-string-table */
	uint		ostrlen ;	/* overflow-string-table length */
	uchar		vetu[4] ;	/* VETU */
} ;

typedef	TTIHDR		ttihdr ;

EXTERNC_begin

extern int	ttihdr_rd(ttihdr *,char *,int) noex ;
extern int	ttihdr_wd(ttihdr *,cchar *,int) noex ;

EXTERNC_end


#endif /* TTIHDR_INCLUDE */


