/* hasend SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* does the given c-string have the specified trailing character */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	hasendchr
	hasendslash

	Description:
	Determine if the given c-string has the specified 
	trailing character.

	Synopsis:
	bool hasendchr		(cchar *sp,int sl,int chx) noex
	bool hasendslash	(cchar *sp,int sl) noex

	Arguments:
	sp		c-string pointer
	sl		c-string length
	chx		character to match to

	Returns:
	true		the given c-string has the trailling character
	false		the given c-string did not match

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>		/* CSTD |UCHAR_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<ascii.h>		/* LIBU */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"hasend.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

bool hasendchr(cchar *sp,int µsl,int chx) noex {
	bool		f = false ;
	chx &= UCHAR_MAX ;
	if (int sl ; (sl = getlenstr(sp,µsl)) > 0) {
	    cint ch = mkchar(sp[sl - 1]) ;
	    f = (ch == chx) ;
	} /* end if (getlenstr) */
	return f ;
} /* end subroutine (hasendchr) */

bool hasendslash(cchar *sp,int sl) noex {
    	return hasendchr(sp,sl,'/') ;
} /* end subroutine (hasendslash) */

bool hastrailchr(cchar *sp,int sl,int chx) noex {
    	return hasendchr(sp,sl,chx) ;
} /* end subroutine */

bool hastrailslash(cchar *sp,int sl) noex {
    	return hasendslash(sp,sl) ;
} /* end subroutine */


