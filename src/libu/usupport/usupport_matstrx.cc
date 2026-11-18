/* usupport_matstrx SUPPORT */
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
	matstr{x}c

	Description:
	Check that the given string matches EXACTLY some string in
	the given array of strings.  This is not a prefix match.
	If we get a match, we return the array index.  If we do not
	match, we return "less-than-zero".

	Synopsis:
	int matstr{x}c(mainv a,cchar *sp,int sl) noex

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
	matstrx

	Description:
	Match a given counted c-string to an entry in an array
	of c-strings.

	Synopsis:
    	int matstrx(mainv a,cchar *sp,int sl) noex

	Arguments:
	a		array of c-strings
	sp		given counted c-string pointer
	sl		given counted c-string length

	Returns:
	>=0		there was a match and this is the matched array index
	<0		no match

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"usupport_toxc.hh"
#include	"usupport_comstrx.hh"
#include	"usupport_matstrx.hh"


/* local defines */


/* imported namespaces */

using libu::comstrbc ;			/* subroutine */
using libu::comstric ;			/* subroutine */
using libu::comstrfc ;			/* subroutine */
using libu::tobc ;			/* subroutine */
using libu::touc ;			/* subroutine */
using libu::tofc ;			/* subroutine */


/* local typedefs */

extern "C" {
    typedef int (*toxc_f)(int) noex ;
    typedef int (*comx_f)(cchar *,cchar *,int) noex ;
} /* end extern */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct mater {
	toxc_f		toxc ;
	comx_f		comx ;
	mater(toxc_f f1,comx_f f2) noex : toxc(f1), comx(f2) { } ;
	int operator () (con mainv a,cchar *sp,int sl) noex {
    	    int		rc = -1 ;
	    if (a && sp) {
	        cint	lch = toxc(sp[0]) ;
	        int	i{} ; /* used-afterwards */
	        if (int m ; sl >= 0) {
	            for (i = 0 ; a[i] ; i += 1) {
		        if ((m = ((sl > 0) && (lch == toxc(a[i][0])))) > 0) {
			    m = comx(a[i],sp,sl) ;
		        }
		        if ((m == sl) && (a[i][m] == '\0')) break ;
	            } /* end for */
	        } else {
	            for (i = 0 ; a[i] ; i += 1) {
		        if ((m = (lch == toxc(a[i][0]))) > 0) {
			    m = comx(a[i],sp,-1) ;
		        }
		        if ((a[i][m] == '\0') && (sp[m] == '\0')) break ;
	            } /* end for */
	        } /* end if */
	        rc = (a[i]) ? i : -1 ;
	    } /* end if (non-null) */
	    return rc ;
        } /* end method (operator) */
    } ; /* end struct (mater) */
} /* end namespace */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

namespace libu {
    int matstrbc(con mainv a,cchar *sp,int sl) noex {
	mater mo(tobc,comstrbc) ;
	return mo(a,sp,sl) ;
    } /* end subroutine (matstrbc) */
    int matstric(con mainv a,cchar *sp,int sl) noex {
	mater mo(touc,comstric) ;
	return mo(a,sp,sl) ;
    } /* end subroutine (matstric) */
    int matstrfc(con mainv a,cchar *sp,int sl) noex {
	mater mo(tofc,comstrfc) ;
	return mo(a,sp,sl) ;
    } /* end subroutine (matstrfc) */
} /* end namespace (libu) */


