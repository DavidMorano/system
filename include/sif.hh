/* sif HEADER */
/* lang=C++20 */

/* string-interate-find object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SIF_INCLUDE
#define	SIF_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* ordered first to configure */
#include	<cstring>		/* |strlen(3c)| */
#include	<utypedefs.h>
#include	<clanguage.h>


struct sif {
	cchar		*sp ;
	cchar		*sstr = nullptr ;
	int		sl ;
	int		sch = 0 ;
	sif(cchar *p) noex : sp(p) {
	    sl = strlen(sp) ;
	} ;
	sif(cchar *p,int l,int c = 0) noex : sp(p), sl(l), sch(c) { 
	    if (sl < 0) sl = strlen(sp) ;
	} ;
	sif(cchar *p,int l,cchar *s = nullptr) noex : sp(p), sl(l) { 
	    if (sl < 0) sl = strlen(sp) ;
	    sstr = s ;
	} ;
	int next(cchar **) noex ;
	int operator () (cchar **rpp) noex {
	    return next(rpp) ;
	} ;
} ; /* end struct (sif) */


#endif /* __cplusplus */
#endif /* SIF_INCLUDE */


