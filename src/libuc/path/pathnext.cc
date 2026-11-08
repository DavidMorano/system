/* pathnext SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find the next path component */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	This was written as part of the FILECHECK utility.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	pathdangle

	Description:
	This subroutine finds a sub-path within a larger path
	(consisting of a counted c-string) that is a dangling
	soft-link.

	Synopsis:
	int pathdangle(cchar *pbuf,int plen) noex

	Arguments:
	pbuf		source path to examine pointer
	plen		source path to examine length

	Returns:
	>=0		length of cleaned up path
	<0		error code (system-return)

	Notes:
	1. Implementation: This subroutine has been coerced to compile
	and link using only the lIBU library.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdio>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<deb.hh>		/* LIBU |DEBPRINTF(3u)| */

#include	"pathnext.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */
import deb ;				/* debugging (LIBU) */

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* local structures */


/* forward references */


/* local variables */

cbool			f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int pathnext(cchar *sp,int sl,cchar **rpp) noex {
    	int		rl = 0 ; /* return-value */
	cchar		*rp = nullptr ;
	bool		f_len = (sl >= 0) ;
	while (((! f_len) || (sl > 0)) && (*sp == '/')) {
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	rp = sp ;
	while ((((! f_len) && *sp) || (sl > 0)) && (*sp != '/')) {
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	rl = intconv(sp - rp) ;
	if (rpp) {
	    *rpp = rp ;
	} /* end */
	return rl ;
} /* end subroutine (pathnext) */


