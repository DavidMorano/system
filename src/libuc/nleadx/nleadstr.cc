/* nleadstr SUPPORT */
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
	nleadstr

	Description:
	This is the same as |nleadbasestr| but with a different
	(simplified) name.


	Names:
	nleadbasestr
	nleadcasestr
	nleadfoldstr

	Description:
	Calculate the number of characters that two string have in
	common from their leading edges. If we get a match at all
	we return the number of characters matched. If we do not
	get a match, we return a negative number. The second given
	string is allowed to have an optional length supplied.

	Synopsis:
	int nlead{x}str(cchar **bs,cchar *sp,int sl) noex

	Arguments:
	bs		base string to compare against
	sp		test-string to test against the base string
	sl		length of test-string

	Returns:
	>=0		match found and it matched up to this length
	<0		no match (system-return)

	Notes:
	1. Rationale for using templates rather than passing a 
	function-pointer:
	The thinking is that I want the template instantiation to
	"see" any inlined subroutine and optimize it all out rather
	than being forced to actually call a subroutine (if only a
	pointer was supplied). Anywat, that was the thinking, even
	if misguided. Also note that this goes against one of the
	major coding standard rules which is: to optimize for space
	rather than time (without the compiler doing that operation
	itself).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<toxc.h>
#include	<localmisc.h>

#include	"nleadstr.h"


/* local defines */


/* imported namespaces */


/* local typedefs */

extern "C" {
    typedef int (*toxc_f)(int) noex ;
}


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* subroutine-templates */

template <toxc_f toxc>
static int nleadxxxxstr(cchar *bs,cchar *sp,int sl) noex {
	int		i = -1 ; /* return-value */
	if (bs) ylikely {
	    if (sl < 0) sl = INT_MAX ;
	    for (i = 0 ; (i < sl) && bs[i] && sp[i] ; i += 1) {
	        if (toxc(bs[i]) != toxc(sp[i])) break ;
	    } /* end for */
	} /* end if (non-null) */
	return i ;
}
/* end subroutine-template (nleadxxxxstr)*/


/* exported subroutines */

int nleadbasestr(cchar *bs,cchar *sp,int sl) noex {
	return nleadxxxxstr<tobc>(bs,sp,sl) ;
}
/* end subroutine (nleadbasestr) */

int nleadcasestr(cchar *bs,cchar *sp,int sl) noex {
	return nleadxxxxstr<touc>(bs,sp,sl) ;
}
/* end subroutine (nleadcasestr) */

int nleadfoldstr(cchar *bs,cchar *sp,int sl) noex {
	return nleadxxxxstr<tofc>(bs,sp,sl) ;
}
/* end subroutine (nleadfoldstr) */


