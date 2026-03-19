/* usupport_matstr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® kernel support subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-26, David A­D­ Morano
	This was first written to give a little bit to UNIX® what
	we have in our own circuit-pack OSes!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

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

	Name:
	matstr

	Description:
	Match a given counted c-string to an entry in an array
	of c-strings.

	Synopsis:
    	int matstr(mainv a,cchar *sp,int sl) noex

	Arguments:
	a		array of c-strings
	sp		given counted c-string pointer
	sl		given counted c-string length

	Returns:
	>=0		there was a match and this is the matched array index
	<0		no match

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>

#include	"usupport_toxc.h"
#include	"usupport_nleadstr.h"
#include	"usupport_matstr.h"


/* local defines */


/* imported namespaces */

using libu::nleadbasestr ;	/* subroutine */
using libu::nleadcasestr ;	/* subroutine */
using libu::nleadfoldstr ;	/* subroutine */
using libu::tobc ;		/* subroutine */
using libu::touc ;		/* subroutine */
using libu::tofc ;		/* subroutine */


/* local typedefs */

extern "C" {
    typedef int (*toxc_f)(int) noex ;
    typedef int (*nleadxstr_f)(cchar *,cchar *,int) noex ;
}


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct matxstr {
	toxc_f		toxc ;
	nleadxstr_f	nleadxstr ;
	matxstr(toxc_f f1,nleadxstr_f f2) noex : toxc(f1), nleadxstr(f2) { } ;
	int operator () (mainv a,cchar *sp,int sl) noex {
    	    int		rc = -1 ;
	    if (a && sp) {
	        cint	lch = toxc(sp[0]) ;
	        int	i{} ; /* used-afterwards */
	        if (int m ; sl >= 0) {
	            for (i = 0 ; a[i] ; i += 1) {
		        if ((m = ((sl > 0) && (lch == toxc(a[i][0])))) > 0) {
			    m = nleadxstr(a[i],sp,sl) ;
		        }
		        if ((m == sl) && (a[i][m] == '\0')) break ;
	            } /* end for */
	        } else {
	            for (i = 0 ; a[i] ; i += 1) {
		        if ((m = (lch == toxc(a[i][0]))) > 0) {
			    m = nleadxstr(a[i],sp,-1) ;
		        }
		        if ((a[i][m] == '\0') && (sp[m] == '\0')) break ;
	            } /* end for */
	        } /* end if */
	        rc = (a[i]) ? i : -1 ;
	    } /* end if (non-null) */
	    return rc ;
        } /* end method (operator) */
    } ; /* end struct (matxstr) */
} /* end namespace */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

namespace libu {
    int matbasestr(mainv a,cchar *sp,int sl) noex {
	matxstr mo(tobc,nleadbasestr) ;
	return mo(a,sp,sl) ;
    } /* end subroutine (matbasestr) */
    int matcasestr(mainv a,cchar *sp,int sl) noex {
	matxstr mo(touc,nleadcasestr) ;
	return mo(a,sp,sl) ;
    } /* end subroutine (matcasestr) */
    int matfoldstr(mainv a,cchar *sp,int sl) noex {
	matxstr mo(tofc,nleadfoldstr) ;
	return mo(a,sp,sl) ;
    } /* end subroutine (matfoldstr) */
} /* end namespace (libu) */


