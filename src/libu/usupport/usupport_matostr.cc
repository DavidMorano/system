/* usupport_matostr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* USUPPORT Match-Option-String */
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
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usupport.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"usupport_toxc.hh"
#include	"usupport_nleadstr.hh"
#include	"usupport_matstr.hh"	/* <- needed for default case */
#include	"usupport_matostr.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */

using libu::tobc ;			/* subroutine */
using libu::tolc ;			/* subroutine */
using libu::touc ;			/* subroutine */
using libu::tofc ;			/* subroutine */
using libu::nleadbasestr ;		/* subroutine */
using libu::nleadcasestr ;		/* subroutine */
using libu::nleadfoldstr ;		/* subroutine */
using libu::matbasestr ;		/* subroutine */
using libu::matcasestr ;		/* subroutine */
using libu::matfoldstr ;		/* subroutine */


/* local typedefs */

extern "C" {
    typedef int (*toxc_f)(int) noex ;
    typedef int (*nleadxstr_f)(cchar *,cchar *,int) noex ;
    typedef int (*matxstr_f)(con mainv,cchar *,int) noex ;
} /* end extern */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct mater {
	toxc_f		toxc ;
	nleadxstr_f	nleadx ;
	matxstr_f	matxstr ;
	int matoxstr(con mainv,int,cchar *,int) noex ;
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

namespace libu {
    int matobasestr(con mainv a,int n,cchar *sp,int sl) noex {
	mater	mo(tobc,nleadbasestr,matbasestr) ;
	return mo.matoxstr(a,n,sp,sl) ;
    } /* end subroutine (matobasestr) */
    int matocasestr(con mainv a,int n,cchar *sp,int sl) noex {
	mater	mo(tolc,nleadcasestr,matcasestr) ;
	return mo.matoxstr(a,n,sp,sl) ;
    } /* end subroutine (matocasestr) */
    int matofoldstr(con mainv a,int n,cchar *sp,int sl) noex {
	mater	mo(tofc,nleadfoldstr,matfoldstr) ;
	return mo.matoxstr(a,n,sp,sl) ;
    } /* end subroutine (matofoldstr) */
} /* end namespace (libu) */


/* local subroutines */

int mater::matoxstr(con mainv a,int n,cchar *sp,int µsl) noex {
	int		si = -1 ;
	if (int sl ; a && ((sl = getlenstr(sp,µsl)) >= 0)) {
	    if (n >= 0) {
	        cint	lch = toxc(sp[0]) ;
	        for (int m, i = 0 ; a[i] ; i += 1) {
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
	} /* end if (getlenstr) */
	return si ;
} /* end method (mater::matoxstr) */


