/* prognamevar SUPPORT */
/* charset=ISO8859-1 */
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
	prognamevar

	Descriptor:
	This object module (prognamevar) provides support for creating
	NUL-terminated strings when only a counted string is
	available.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<new>			/* |nothrow(3c++)| */
#include	<usyscalls.h>
#include	<usupport.h>
#include	<ulogerror.h>
#include	<localmisc.h>

#include	"prognamevar.hh"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/ureserve.ccm"

import libutil ;			/* |lenstr(3u)| */
import ureserve ;			/* |sfbasename(3u)| */

/* local defines */


/* imported namespaces */

using libu::strwcpy ;			/* subroutine */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

void prognamevar::proc(cchar *pp,int pl) noex {
    sl = sfbasename(pp,pl,&sp) ;
} /* end method (prognamevar::proc) */

prognamevar::operator ccharp () noex {
    	cnullptr	np{} ;
	cnothrow	nt{} ;
	if (rp == nullptr) {
	    rp = sp ;
	    if ((sl >= 0) && (sp[sl] != '\0')) {
	        if (sl > STRNUL_SHORTLEN) {
		    if ((as = new(nt) char[sl + 1]) != np) {
			strwcpy(as,sp,sl) ;
			rp = as ;
		    } else {
			ulogerror("prognamevar",SR_NOMEM,"mem-alloc failure") ;
			rp = "«mem-alloc-failure»" ;
		    }
	        } else {
		    strwcpy(buf,sp,sl) ;
		    rp = buf ;
	        }
	    } /* end if (possibly required) */
	} /* end if (need calculation) */
	return rp ;
} /* end method (prognamevar::operator) */


