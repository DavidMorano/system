/* strnul SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* assert a NUL-terminated c-string */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	strnul

	Descriptor:
	This object (STRNUL) provides support for creating
	NUL-terminated strings when only a counted string is
	available.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<new>			/* C++STD */
#include	<string_view>		/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ulogerror.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<dprint.hh>		/* LIBU |DPRINTF(3u)| */

#include	"strnul.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */

using strview = std::string_view ;	/* type */
using libu::strwcpy ;			/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

cint		blen		= STRNUL_SHORTLEN ;
cbool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

strnul::strnul(cchar *sp,int sl) noex {
    	buf[0] = eos ;
	init() ;
	proc(sp,sl) ;
} /* end ctor */

strnul::strnul(const strview &sv) noex {
	ccharp	sp = sv.data() ;
	cint	sl = (int) sv.length() ;
    	buf[0] = eos ;
	init() ;
	proc(sp,sl) ;
} /* end ctor */

void strnul::clear() noex {
	rp = nullptr ;
    	buf[0] = eos ;
	if (as) {
	    delete [] as ;
	    as = nullptr ;
	} /* end */
} /* end method (strnul:clear) */

int strnul::alloc(cchar *sp,int sl) noex {
    	cnullptr	np{} ;
    	cnothrow	nt{} ;
    	int		al = -1 ;
	if ((as = new(nt) char[sl + 1]) != np) {
	    al = intconv(strwcpy(as,sp,sl) - as) ;
	    rp = as ;
	} else {
	    cint rs = SR_NOMEM ;
	    ulogerror("strnul",rs,"mem-alloc failure") ;
	    rp = "«mem-alloc-failure»" ;
	    fok = false ;
	} /* end */
	return al ;
} /* end method (strnul::alloc) */

ccharp strnul::proc(cchar *sp,int sl) noex {
	if (sp) ylikely {
	    rp = sp ;
	    fok = true ;
	    if (sl >= 0) /* do-not-know */ {
		if (rl = lenstr(sp,sl) ; sp[rl] != eos) {
		    if (rl > blen) {
		        alloc(sp,rl) ;
	            } else {
		        strwcpy(buf,sp,rl) ;
		        rp = buf ;
		    } /* end if (storage required) */
		} /* end if (need nul-termination) */
	    } else {
		rl = lenstr(sp) ;
	    } /* end if (possibly required) */
	} /* end if (non-null) */
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
	rp = proc(sp,sl) ;
	return rp ;
} /* end method (strnul::operator) */

ccharp strnul::operator () (const strview &sv) noex {
	ccharp	sp = sv.data() ;
	cint	sl = (int) sv.length() ;
	clear() ;
	rp = proc(sp,sl) ;
	return rp ;
} /* end method (strnul::operator) */

void strnul::init() noex {
    	len	(this) ;
	rl	= -1 ;
} /* end method (strnul::init) */

strnul_len::operator int () const noex {
    	return op->rl ;
} /* end method (strnul_len::operator) */


