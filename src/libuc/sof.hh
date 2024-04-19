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
#include	<utypedefs.h>
#include	<clanguage.h>


struct sif {
	cchar	*sbuf ;
	int	slen ;
	sif(cchar *p,int l) noex ; sbuf(p), slen(l) { } ;
	int next(cchar **) noex ;
} ;


#endif /* __cplusplus */
#endif /* SIF_INCLUDE */


