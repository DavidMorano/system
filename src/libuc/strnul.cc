/* strnul */
/* lang=C++20 */

/* assert a NUL-terminated string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object module (strnul) provides support for creating
	NUL-terminated strings when only a counted string is
	available.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<new>
#include	<usystem.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"strnul.hh"


/* local defines */


/* local namespaces */

using std::nothrow ;


/* local typedefs */


/* external subroutines */


/* exported subroutines */

strnul::operator charp () noex {
	cchar		*rp = sp ;
	if (sl >= 0) {
	    if (sp[sl] != '\0') {
	        if (sl > STRNUL_SHORTLEN) {
		    if ((as = new(nothrow) char[(sl+1)]) != nullptr) {
			strwcpy(as,sp,sl) ;
			rp = as ;
		    } else {
			rp = nullptr ;
		    }
	        } else {
		    strwcpy(buf,sp,sl) ;
		    rp = buf ;
	        }
	    } /* end if (required) */
	} /* end if (possibly required) */
	return rp ;
}
/* end method (strnul_co::operator) */


