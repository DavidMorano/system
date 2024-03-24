/* hasINET4AddrStr SUPPORT */
/* lang=C++20 */

/* test whether the given string contains an INET4 address string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	hasINET4AddrStr

	Description:
	We test whether the given string consists of an INET4 address
	string.

	Synopsis:
	int hasINET4AddrStr(cchar *sp,int sl) noex

	Arguments:
	sp		string to test
	sl		length of strin to test

	Returns:
	false		assertion fails
	true		assertion succeeds

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstring>		/* |strlen(3c)| */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<strn.h>
#include	<hasx.h>
#include	<localmisc.h>

#include	"hasinet4addrstr.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static bool	hasINET4Num(cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

bool hasinet4addrstr(cchar *sp,int sl) noex {
	bool		f = true ;
	int		c = 0 ;
	cchar		*tp ;
	if (sl < 0) sl = strlen(sp) ;
	while ((tp = strnchr(sp,sl,'.')) != nullptr) {
	    f = hasINET4Num(sp,(tp-sp)) ;
	    if (! f) break ;
	    sl -= ((tp+1)-sp) ;
	    sp = (tp+1) ;
	    c += 1 ;
	} /* end while */
	if (f && (sl > 0)) {
	    c += 1 ;
	    f = hasINET4Num(sp,sl) ;
	} /* end if */
	if (f && (c != 4)) {
	    f = false ;
	}
	return f ;
}
/* end subroutine (hasINET4AddrStr) */


/* local subroutines */

static bool hasINET4Num(cchar *sp,int sl) noex {
	return ((sl <= 3) && hasalldig(sp,sl)) ;
}
/* end subroutine (hasINET4Num) */


