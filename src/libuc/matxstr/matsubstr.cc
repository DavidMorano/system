/* matsubstr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* match a string w/ various case handling */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	Module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	matsub{x}str

	Description:
	Determine if the given substring is amoung the array of
	strings given (case insensitively).

	Synopsis:
	int mat{x}substr(con mainv a,cchar *sp,int sl) noex

	Arguments:
	{x}		base, case, fold
	a		array of string to match against
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
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<six.h>			/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"matxstr.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* local namespaces */


/* local typedefs */

extern "C" {
    typedef int (*sixsub_f)(cchar *,int,cchar *) noex ;
} /* end extern (C) */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<sixsub_f sixsub>
local int matxsubstr(con mainv a,cchar *sp,int µsl) noex {
    	int		rc = -1 ;
	if (int sl ; a && ((sl = getlenstr(sp,µsl)) >= 0)) {
	    int		i{} ; /* used-afterwards */
	    for (i = 0 ; a[i] ; i += 1) {
	        if (sixsub(sp,sl,a[i]) >= 0) break ;
	    } /* end for */
	    rc = (a[i] != nullptr) ? i : -1 ;
	} /* end if (getlenstr) */
	return rc ;
} /* end subroutine (matxsubstr) */


/* local variables */


/* exported variables */


/* exported subroutines */

int matbasesubstr(con mainv a,cchar *sp,int sl) noex {
    	return matxsubstr<sibasesub>(a,sp,sl) ;
} /* end subroutine (matbasesubstr) */

int matcasesubstr(con mainv a,cchar *sp,int sl) noex {
    	return matxsubstr<sicasesub>(a,sp,sl) ;
} /* end subroutine (matcasesubstr) */

int matfoldsubstr(con mainv a,cchar *sp,int sl) noex {
    	return matxsubstr<sifoldsub>(a,sp,sl) ;
} /* end subroutine (matfoldsubstr) */


