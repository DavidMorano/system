/* matostr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Match-Option-String */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	Module was originally written.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mato{x}str

	Description:
	Check that the given string matches a MINIMUM number of
	leading characters for some string in the given array of
	strings.  If we get a match, we return the array index.  If
	we do not match, we return "less-than-zero".

	Synopsis:
	int mato{x}str(cchar *const *a,int n,cchar *sp,int sl) noex

	Arguments:
	{x}		base, case, fold
	a		array of string to match against
	n		minimum number of characters that must match
	sp		string to test against array
	sl		length of test string

	Returns:
	>=0		index of match in array
	<0		no match found (not further distinguished)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<toxc.h>
#include	<nleadstr.h>
#include	<localmisc.h>

#include	"matstr.h"		/* <- needed for default case */
#include	"matostr.h"

import libutil ;

/* local defines */


/* imported namespaces */


/* local typedefs */

extern "C" {
    typedef int (*toxc_f)(int) noex ;
    typedef int (*nleadxstr_f)(cchar *,cchar *,int) noex ;
    typedef int (*matxstr_f)(mainv,cchar *,int) noex ;
}


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct mater {
	toxc_f		toxc ;
	nleadxstr_f	nleadx ;
	matxstr_f	matxstr ;
	int matoxstr(mainv,int,cchar *,int) noex ;
	mater(toxc_f t,nleadxstr_f n,matxstr_f m) noex { 
	    toxc = t ;
	    nleadx = n ;
	    matxstr = m ;
	} ; /* end ctor */
    } ; /* end struct (mater) */
} /* end namespace */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int matobasestr(mainv a,int n,cchar *sp,int sl) noex {
	mater	mo(tobc,nleadbasestr,matbasestr) ;
	return mo.matoxstr(a,n,sp,sl) ;
}
/* end subroutine (matobasestr) */

int matocasestr(mainv a,int n,cchar *sp,int sl) noex {
	mater	mo(tolc,nleadcasestr,matcasestr) ;
	return mo.matoxstr(a,n,sp,sl) ;
}
/* end subroutine (matocasestr) */

int matofoldstr(mainv a,int n,cchar *sp,int sl) noex {
	mater	mo(tofc,nleadfoldstr,matfoldstr) ;
	return mo.matoxstr(a,n,sp,sl) ;
}
/* end subroutine (matofoldstr) */


/* local subroutines */

int mater::matoxstr(mainv a,int n,cchar *sp,int sl) noex {
	int		si = -1 ;
	if (sl < 0) sl = lenstr(sp) ;
	if (n >= 0) {
	    cint	lch = toxc(sp[0]) ;
	    for (int i = 0 ; a[i] ; i += 1) {
	        int	m ;
	        cchar	*ap = a[i] ;
	        if ((m = (lch == toxc(ap[0]))) > 0) {
	            m = nleadx(ap,sp,sl) ;
	        }
	        if ((m == sl) && ((m >= n) || (ap[m] == '\0'))) {
		    si = i ;
		    break ; 
	        }
	    } /* end for */
	} else {
	    si = matxstr(a,sp,sl) ;
	}
	return si ;
}
/* end method (mater::matoxstr) */


