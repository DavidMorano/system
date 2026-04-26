/* strnul SUPPORT */
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
	strnul

	Descriptor:
	This object module (strnul) provides support for creating
	NUL-terminated strings when only a counted string is
	available.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<new>			/* |nothrow(3c++)| */
#include	<usyscalls.h>
#include	<ulogerror.h>
#include	<localmisc.h>

#include	"strnul.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

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

cint		blen = STRNUL_SHORTLEN ;


/* exported variables */


/* exported subroutines */

strnul::strnul(cchar *sp,int sl) noex {
    	buf[0] = '\0' ;
	proc(sp,sl) ;
} /* end ctor */

strnul::strnul(const strview &sv) noex {
	ccharp	sp = sv.data() ;
	cint	sl = (int) sv.length() ;
    	buf[0] = '\0' ;
	proc(sp,sl) ;
} /* end ctor */

void strnul::clear() noex {
	rp = nullptr ;
    	buf[0] = '\0' ;
	if (as) {
	    delete [] as ;
	    as = nullptr ;
	}
} /* end method (strnul::proc) */

void strnul::alloc(cchar *sp,int sl) noex {
	if ((as = new(nothrow) char[sl + 1]) != nullptr) {
	    strwcpy(as,sp,sl) ;
	    rp = as ;
	} else {
	    ulogerror("strnul",SR_NOMEM,"mem-alloc failure") ;
	    rp = "«mem-alloc-failure»" ;
	    fok = false ;
	}
} /* end method (strnul::alloc) */

ccharp strnul::proc(cchar *sp,int sl) noex {
	if (sp) {
	    rp = sp ;
	    if (sl >= 0) {
		if (sp[sl] != '\0') {
		    if (lenstr(sp,sl) >= sl) {
	                if (sl > blen) {
		            alloc(sp,sl) ;
	                } else {
		            strwcpy(buf,sp,sl) ;
		            rp = buf ;
	                }
		    } /* end if (storeage required) */
		} /* end if (not nul-terminated as given) */
	    } /* end if (possibly required) */
	} /* end if (need calculation) */
	return rp ;
} /* end method (strnul::proc) */

void strnul::dtor() noex {
	clear() ;
} /* end method (strnul::dtor) */

strnul &strnul::operator = (cchar *sp) noex {
	clear() ;
	proc(sp) ;
	return *this ;
} /* end operator (assignment from |ccharp|) */

strnul &strnul::operator = (const strview &sv) noex {
	ccharp	sp = sv.data() ;
	cint	sl = (int) sv.length() ;
	clear() ;
	proc(sp,sl) ;
	return *this ;
} /* end operator (assignment from |string_view|) */

ccharp strnul::operator () (cchar *sp,int sl) noex {
    	clear() ;
	return proc(sp,sl) ;
} /* end method (strnul::operator) */

ccharp strnul::operator () (const strview &sv) noex {
	ccharp	sp = sv.data() ;
	cint	sl = (int) sv.length() ;
	clear() ;
	return proc(sp,sl) ;
} /* end method (strnul::operator) */


