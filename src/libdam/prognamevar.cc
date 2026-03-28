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
#include	<six.h>			/* LIBUC */
#include	<getourenv.h>		/* LIBUC */
#include	<shellunder.h>		/* LIBUC */
#include	<localmisc.h>

#include	"prognamevar.hh"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/ureserve.ccm"

import libutil ;			/* |lenstr(3u)| */
import ureserve ;			/* |sfbasename(3u)| */

/* local defines */


/* imported namespaces */

using libu::strwcpy ;			/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

bool prognamevar::proc(cchar *pp,int pl) noex {
    	bool		f = false ;
        if ((sl = sfbasename(pp,pl,&sp)) > 0) {
            if (cint si = sirchr(sp,sl,'.') ; si > 0) {
	        sl = si ;
		f = true ;
	    } else if (si < 0) {
		sl = pl ;
		f = true ;
	    }
        } /* end if (sfbasename) */
	return f ;
} /* end method (prognamevar::proc) */

bool prognamevar::procenv(mainv envv) noex {
    	bool		f = false ;
	if (envv) {
	    if (cchar *valp = getourenv(envv,"_") ; valp) {
		int rs{} ;
		if (shellunder_dat d ; (rs = shellunder_load(&d,valp)) >= 0) {
		    f = proc(d.execname) ;
		} else if (rs < 0) {
		    ulogerror("prognamevar",rs,"shellunder") ;
		}
	    } /* end if (getourenv) */
	} /* end if (non-null) */
	return f ;
} /* end method (prognamevar::procenv) */

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
			cint rsnomem = SR_NOMEM ;
			ulogerror("prognamevar",rsnomem,"mem-alloc failure") ;
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


