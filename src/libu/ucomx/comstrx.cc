/* comstrx SUPPORT */
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

  	Group:
	nlead{x}str

	Names:
	comstrbc
	comstric
	comstrfc

	Description:
	Calculate the number of characters that two strings have in
	common from their leading edges.  If we get a match at all
	we return the number of characters matched.  If we do not
	get a match, we return a negative number.  The second given
	string is allowed to have an optional length supplied.

	Synopsis:
	int nlead{x}str(cchar **bs,cchar *sp,int sl) noex

	Arguments:
	{x}		'base', 'case', 'fold'
	bs		base string to compare against
	sp		test-string to test against the base string
	sl		length of test-string

	Returns:
	>=0		length of matched string (0=no-match)
	<0		error (-1 only due to NULL argument)

	Notes:
	1. Rationale for using templates rather than passing a 
	function-pointer:
	The thinking is that I want the template instantiation to
	"see" any inlined subroutine and optimize it all out rather
	than being forced to actually call a subroutine (if only a
	pointer was supplied).  Anywat, that was the thinking, even
	if misguided.  Also note that this goes against one of the
	major coding standard rules which is: to optimize for space
	rather than time (without the compiler doing that operation
	itself).

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

#include	"nleadstr.h"


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

local int comstrxc(toxc_f toxc,cchar *bs,cchar *sp,int sl) noex {
	int		i = -1 ; /* return-value */
	if (bs && sp) ylikely {
	    if (sl < 0) sl = INT_MAX ;
	    for (i = 0 ; (i < sl) && bs[i] && sp[i] ; i += 1) {
		cint ch1 = mkchar(bs[i]) ;
		cint ch2 = mkchar(sp[i]) ;
	        if (toxc(ch1) != toxc(ch2)) break ;
	    } /* end for */
	} /* end if (non-null) */
	return i ;
} /* end subroutine (comstrxc) */


/* local variables */


/* exported variables */


/* exported subroutines */

int comstrbc(cchar *bs,cchar *sp,int sl) noex {
	return comstrxc(tobc,bs,sp,sl) ;
} /* end subroutine (comstrbc) */

int comstric(cchar *bs,cchar *sp,int sl) noex {
	return comstrxc(touc,bs,sp,sl) ;
} /* end subroutine (comstric) */

int comstrfc(cchar *bs,cchar *sp,int sl) noex {
	return comstrxc(tofc,bs,sp,sl) ;
} /* end subroutine (comstrfc) */


