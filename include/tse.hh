/* tse HEADER (Time-Stamp Entry) */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* machine status entry */
/* version %I% last-modified %G% */


/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

#ifndef	TSE_INCLUDE
#define	TSE_INCLUDE
#ifdef	__cplusplus /* C++ only */


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	TSE		struct tse
/* entry field lengths */
#define	TSE_LCOUNT		4
#define	TSE_LUTIME		4	/* time - entry update */
#define	TSE_LCTIME		4	/* time - entry creation */
#define	TSE_LHASH		4
#define	TSE_LKEYNAME		32
/* entry field offsets */
/* do this carefully! */
/* there is no good automatic way to do this in C language (sigh) */
/* the C language does not have all of the advantages of assembly language! */
#define	TSE_OCOUNT		0
#define	TSE_OUTIME		(TSE_OCOUNT + TSE_LCOUNT)
#define	TSE_OCTIME		(TSE_OUTIME + TSE_LUTIME)
#define	TSE_OHASH		(TSE_OCTIME + TSE_LCTIME)
#define	TSE_OKEYNAME		(TSE_OHASH + TSE_LHASH)
#define	TSE_SIZE		(TSE_OKEYNAME + TSE_LKEYNAME)


struct tse {
	uint		count ;		/* count */
	uint		utime ;		/* update time-stamp */
	uint		ctime ;		/* creation time-stamp */
	uint		hash ;
	char		keyname[TSE_LKEYNAME+ 1] ;
	int rd(char *,int) noex ;
	int wr(cchar *,int) noex ;
	int rdu(char *,int) noex ;
	int wru(cchar *,int) noex ;
} ;

typedef	TSE		tse ;


#endif /* __cplusplus */
#endif /* TSE_INCLUDE */


