/* strnul SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* assert a NUL-terminated string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strnul

	Descriptor:
	This object module (strnul) provides support for creating
	NUL-terminated strings when only a counted string is
	available.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<new>
#include	<usystem.h>
#include	<ulogerror.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"strnul.hh"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

strnul::operator ccharp () noex {
	if (rp == nullptr) {
	    rp = sp ;
	    if ((sl >= 0) && (sp[sl] != '\0')) {
	        if (sl > STRNUL_SHORTLEN) {
		    if ((as = new(nothrow) char[sl + 1]) != nullptr) {
			strwcpy(as,sp,sl) ;
			rp = as ;
		    } else {
			ulogerror("strnul",SR_NOMEM,"mem-alloc failure") ;
			rp = "«mem-alloc-failure»" ;
		    }
	        } else {
		    strwcpy(buf,sp,sl) ;
		    rp = buf ;
	        }
	    } /* end if (possibly required) */
	} /* end if (need calculation) */
	return rp ;
}
/* end method (strnul::operator) */


