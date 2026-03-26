/* matnstr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* match a counted string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	Module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	matnstr

	Description:
	Check that the given string exactly matches the leading
	part of some string in the given array of strings.  If we
	get a match, we return the array index.  If we do not match,
	we return "less-than-zero".

	Synopsis:
	int matnstr(mainv a,cchar *sp,int sl) noex

	Arguments:
	a		array of string to match against
	sp		string to test against array
	sl		length of test string

	Returns:
	>=0		index of match in array
	<0		no match found (not further distinguished)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<nleadstr.h>
#include	<stdclib.hh>		/* STDCLIB */
#include	<strnxxxxcmp.h>		/* LIBUC */
#include	<mkchar.h>
#include	<localmisc.h>

#include	"matnstr.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	strncasecmp	std_strncasecmp


/* imported namespaces */

using stdclib::std_strncasecmp ;		/* subroutine */


/* local typedefs */

extern "C" {
    typedef int (*nxcmp_f)(cchar *,cchar *,int) noex ;
}


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct mater {
	nxcmp_f		xcmp ;
	int matnxstr(mainv,cchar *,int) noex ;
	mater(nxcmp_f c) noex : xcmp(c) { } ;
    } ; /* end struct (mater) */
} /* end namespace */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int matnbasestr(mainv a,cchar *sp,int sl) noex {
	mater	mo(strnbasecmp) ;
	return mo.matnxstr(a,sp,sl) ;
} /* end subroutine (matnbasestr) */

int matncasestr(mainv a,cchar *sp,int sl) noex {
	mater	mo(strncasecmp) ;
	return mo.matnxstr(a,sp,sl) ;
} /* end subroutine (matncasestr) */

int matnfoldstr(mainv a,cchar *sp,int sl) noex {
	mater	mo(strnfoldcmp) ;
	return mo.matnxstr(a,sp,sl) ;
} /* end subroutine (matnfoldstr) */


/* local subroutines */

int mater::matnxstr(mainv a,cchar *sp,int µsl) noex {
    	int		rc = -1 ;
	if (int sl ; a && ((sl = getlenstr(sp,µsl)) >= 0)) {
	    cint	lc = sp[0] ; /* ok: everything promotes the same */
	    int		i{} ; /* used-afterwards */
	    for (i = 0 ; a[i] ; i += 1) {
	        if ((lc == a[i][0]) && (xcmp(a[i],sp,sl) == 0)) break ;
	    } /* end for */
	    rc = (a[i] != nullptr) ? i : -1 ;
	} /* end if (getlenstr) */
	return rc ;
} /* end subroutine (matnxstr) */


