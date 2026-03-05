/* hasonly SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* does the counted c-string have some characteristic? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-10-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	hasonly{x}

	Names:
	hasonlypl
	hasonlymi
	hasonlypm
	hasonlyme

	Description:
	These subroutines check if the specified counted c-string
	has only the characteristic we are looking for.

	Synopsis:
	bool hasonlypl(cchar *sp,int sl) noex
	bool hasonlymi(cchar *sp,int sl) noex
	bool hasonlypm(cchar *sp,int sl) noex
	bool hasonlyme(cchar *sp,int sl) noex

	Arguments:
	{x}		one of: pl, mi, pm, me
	sp		given counted c-string pointer
	sl		given counted c-string length

	Returns:
	false
	true

	Usage:
	hasonlypl	- has-only plus
	hasonlymi	- has-only minus
	hasonlypm	- has-only plus-minus
	hasonlyme	- has-only me

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<mkchar.h>
#include	<ischarx.h>		/* |is{x}(3uc)| */
#include	<localmisc.h>		/* |UC(3dam)| */

#include	"hasonly.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */

extern "C" {
    typedef bool (*isx_f)(int) noex ;
}


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct owner {
	isx_f	isx ;
	owner(isx_f f) noex : isx(f) { } ;
	bool operator () (cchar *,int) noex ;
    } ; /* end struct (owner) */
} /* end namespace */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

bool hasonlypl(cchar *sp,int µsl) noex {
    	owner oo(ispl) ;
	return oo(sp,µsl) ;
}
/* end subroutine (hasonlypl) */

bool hasonlymi(cchar *sp,int µsl) noex {
    	owner oo(ismi) ;
	return oo(sp,µsl) ;
}
/* end subroutine (hasonlymi) */

bool hasonlypm(cchar *sp,int µsl) noex {
    	owner oo(ispm) ;
	return oo(sp,µsl) ;
}
/* end subroutine (hasonlypm) */

bool hasonlyme(cchar *sp,int µsl) noex {
    	owner oo(isme) ;
	return oo(sp,µsl) ;
}
/* end subroutine (hasonlyme) */


/* local subroutines */

bool owner::operator () (cchar *sp,int µsl) noex {
        bool            f = false ;
	if (int sl ; (sl = getlenstr(sp,µsl)) == 1) {
	    f = isx(*sp) ;
	} /* end if (getlenstr) */
        return f ;
} /* end method (owner::operator) */


