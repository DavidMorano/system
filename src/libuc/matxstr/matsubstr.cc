/* matsubstr SUPPORT */
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
	Check is the given substring is amoung the array of strings
	given (case insensitively).

	Synopsis:
	int mat{x}substr(mainv a,cchar *sp,int sl) noex

	Arguments:
	a		array of string to match against
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
#include	<six.h>
#include	<localmisc.h>

#include	"matxstr.h"


/* local defines */


/* local namespaces */


/* local typedefs */

extern "C" {
    typedef int (*sixsub_f)(cchar *,int,cchar *) noex ;
}


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<sixsub_f sixsub>
int matxsubstr(mainv a,cchar *sp,int sl) noex {
	int		i{} ; /* used-afterwards */
	for (i = 0 ; a[i] ; i += 1) {
	    if (sixsub(sp,sl,a[i]) >= 0) break ;
	} /* end for */
	return (a[i] != nullptr) ? i : -1 ;
}
/* end subroutine (matxsubstr) */


/* local variables */


/* exported variables */


/* exported subroutines */

int matbasesubstr(mainv a,cchar *sp,int sl) noex {
    	return matxsubstr<sibasesub>(a,sp,sl) ;
}
/* end subroutine (matbasesubstr) */

int matcasesubstr(mainv a,cchar *sp,int sl) noex {
    	return matxsubstr<sicasesub>(a,sp,sl) ;
}
/* end subroutine (matcasesubstr) */

int matfoldsubstr(mainv a,cchar *sp,int sl) noex {
    	return matxsubstr<sifoldsub>(a,sp,sl) ;
}
/* end subroutine (matfoldsubstr) */


