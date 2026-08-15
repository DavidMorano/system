/* hasneed SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* does the given c-string need to have some specified trailing item */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	haslead

	Description:
	Determine if the given c-string has the specified 
	trailing character.

	Synopsis:
	bool hasneedchr		(cchar *sp,int sl,int chx) noex
	bool hasneedslash	(cchar *sp,int sl) noex

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
#include	"hasneed.h"

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

bool hasneedchr(cchar *sp,int sl,int chx) noex {
    	cbool f = hasendchr(sp,sl,chx) ;
    	return (! f) ;
} /* end subroutine (hasneedchr) */

bool hasneedslash(cchar *sp,int sl) noex {
    	return hasneedchr(sp,sl,'/') ;
} /* end subroutine (hasneedslash) */

bool hastrailchr(cchar *sp,int sl,int chx) noex {
    	return hasneedchr(sp,sl,chx) ;
}

bool hastrailslash(cchar *sp,int sl) noex {
    	return hasneedslash(sp,sl) ;
}


