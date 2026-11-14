/* comstrk SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* match on the leading part of a string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	nleadkeystr

	Description:
	Check that the given string matches (case independently)
	the LEADING part of some string in the given array of
	strings.  If we get a match, we return the number of characters
	matched.  If we do not match, we return a negative number.

	Synopsis:
	int nleadkeystr(cchar *bs,cchar *sp,int sl) noex

	Arguments:
	bs		base string to compare against
	sp		test-string to test against the base string
	sl		length of test-string

	Returns:
	>=0		match found and it matched up to this length
	<0		no match or error

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usupport.h>		/* LIBU |toxc(3u)| */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"comstrk.h"


/* local defines */


/* imported namespaces */

using libu::tobc ;			/* subroutine */
using libu::touc ;			/* subroutine */
using libu::tofc ;			/* subroutine */


/* local typedefs */

extern "C" {
    typedef int (*toxc_f)(int) noex ;
} /* end extern (C) */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int comstrkxc(toxc_f toxc,cchar *bs,cchar *sp,int sl) noex {
	int		i = -1 ; /* return-value */
	if (bs && sp) ylikely {
	    if (sl < 0) sl = INT_MAX ;
	    for (i = 0 ; (i < sl) && bs[i] && sp[i] ; i += 1) {
		cint ch1 = mkchar(bs[i]) ;
		cint ch2 = mkchar(sp[i]) ;
	        if (toxc(ch1) != toxc(ch2)) break ;
	        if ((ch1 == '=') || (ch2 == '=')) break ;
	    } /* end for */
	} /* end if (non-null) */
	return i ;
} /* end subroutine (comstrkxc) */


/* local variables */


/* exported variables */


/* exported subroutines */


/* external subroutines */

int comstrkbc(cchar *bs,cchar *sp,int sl) noex {
	return comstrkxc(tobc,bs,sp,sl) ;
} /* end subroutine (comstrkbc) */

int comstrkic(cchar *bs,cchar *sp,int sl) noex {
	return comstrkxc(touc,bs,sp,sl) ;
} /* end subroutine (comstrkic) */

int comstrkfc(cchar *bs,cchar *sp,int sl) noex {
	return comstrkxc(tofc,bs,sp,sl) ;
} /* end subroutine (comstrkfc) */


