/* matstr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Match a String */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	Module was originally written.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mat{x}str

	Description:
	Check that the given string matches EXACTLY some string in
	the given array of strings.  This is not a prefix match.
	If we get a match, we return the array index.  If we do not
	match, we return "less-than-zero".

	Synopsis:
	int mat(x)str(mainv a,cchar *sp,int sl) noex

	Arguments:
	{x}		base, case, fold
	a		array of string to match against
	sp		string to test against array
	sl		length of test string

	Returns:
	>=0		index of match in array
	<0		no match found (not further distinguished)

	Notes:
	1. Rationale for using templates rather than passing a 
	function-pointer:
	The thinking is that I want the template instantiation to
	"see" any inlined subroutine and optimize it all out rather
	than being forced to actually call a subroutine (if only a
	pointer was supplied).  Anyway, that was the thinking, even
	if misguided.  Also note that this goes against one of the
	major coding standard rules which is: to optimize for space
	rather than time (without the compiler doing that operation
	itself).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<nleadstr.h>
#include	<toxc.h>
#include	<localmisc.h>

#include	"matstr.h"


/* local defines */


/* imported namespaces */


/* local typedefs */

extern "C" {
    typedef int (*toxc_f)(int) noex ;
    typedef int (*nleadxstr_f)(cchar *,cchar *,int) noex ;
}


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* subroutine-templates */

template<toxc_f toxc,nleadxstr_f nleadxstr>
int matxstr(mainv a,cchar *sp,int sl) noex {
	cint		lch = toxc(sp[0]) ;
	int		i{} ; /* used-afterwards */
	int		m ;
	if (sl >= 0) {
	    for (i = 0 ; a[i] ; i += 1) {
		m = ((sl > 0) && (lch == toxc(a[i][0]))) ;
		if (m > 0) m = nleadxstr(a[i],sp,sl) ;
		if ((m == sl) && (a[i][m] == '\0')) break ;
	    } /* end for */
	} else {
	    for (i = 0 ; a[i] ; i += 1) {
		m = (lch == toxc(a[i][0])) ;
		if (m > 0) m = nleadxstr(a[i],sp,-1) ;
		if ((a[i][m] == '\0') && (sp[m] == '\0')) break ;
	    } /* end for */
	} /* end if */
	return (a[i]) ? i : -1 ;
}
/* end subroutine-template (matxstr) */


/* exported variables */


/* exported subroutines */

int matbasestr(mainv a,cchar *sp,int sl) noex {
	return matxstr<tobc,nleadbasestr>(a,sp,sl) ;
}
/* end subroutine (matbasestr) */

int matcasestr(mainv a,cchar *sp,int sl) noex {
	return matxstr<touc,nleadcasestr>(a,sp,sl) ;
}
/* end subroutine (matcasestr) */

int matfoldstr(mainv a,cchar *sp,int sl) noex {
	return matxstr<tofc,nleadfoldstr>(a,sp,sl) ;
}
/* end subroutine (matfoldstr) */


