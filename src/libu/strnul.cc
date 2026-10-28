/* strnul SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* assert a NUL-terminated string */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

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
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<new>			/* C++STD |nothrow(3c++)| */
#include	<string>		/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ulogerror.h>		/* LIBU */
#include	<strnul.hh>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<dprint.hh>		/* LIBU |DPRINTF(3u)| */

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */

using std::string ;			/* type */
using libu::strwcpy ;			/* subroutine */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

cint		blen = STRNUL_SHORTLEN ;
cbool		f_debug = CF_DEBUG ;


/* exported variables */


/* exported subroutines */

strnul::strnul(cchar *sp,int sl) noex {
    	DPRINTF("ent as=%p\n",as) ;
    	buf[0] = eos ;
	init() ;
	proc(sp,sl) ;
    	DPRINTF("ret\n") ;
} /* end ctor */

strnul::strnul(const strview &sv) noex {
	ccharp	sp = sv.data() ;
	cint	sl = (int) sv.length() ;
    	DPRINTF("ent as=%p\n",as) ;
    	buf[0] = eos ;
	init() ;
	proc(sp,sl) ;
    	DPRINTF("ret\n") ;
} /* end ctor */

void strnul::clear() noex {
    	DPRINTF("ent as=%p\n",as) ;
	rp = nullptr ;
    	buf[0] = eos ;
	if (as) {
    	    DPRINTF("delete\n") ;
	    delete [] as ;
	    as = nullptr ;
	} /* end */
    	DPRINTF("ret as=%p\n",as) ;
    	DPRINTF("ret\n") ;
} /* end method (strnul::proc) */

int strnul::alloc(cchar *sp,int sl) noex {
    	int		al = -1 ;
    	DPRINTF("ent as=%p\n",as) ;
	if ((as = new(nothrow) char[sl + 1]) != nullptr) {
	    al = intconv(strwcpy(as,sp,sl) - as) ;
	    rp = as ;
	} else {
	    ulogerror("strnul",SR_NOMEM,"mem-alloc failure") ;
	    rp = "«mem-alloc-failure»" ;
	    fok = false ;
	} /* end */
    	DPRINTF("ret as=%p\n",as) ;
    	DPRINTF("ret al=%d\n",al) ;
	return al ;
} /* end method (strnul::alloc) */

ccharp strnul::proc(cchar *sp,int sl) noex {
    	DPRINTF("ent sl=%d\n",sl) ;
    	DPRINTF("as=%p\n",as) ;
	if_constexpr (f_debug) {
    	    if (sp) {
    	        if (sl < 0) sl = lenstr(sp) ;
	        string str(sp,sl) ;
    	        DPRINTF("ent s=>%s<\n",str.c_str()) ;
	    } /* end if (non-null) */
	} /* end if_constexpr (f_debug) */
	if (sp) ylikely {
    	    DPRINTF("non-null\n") ;
	    rp = sp ;
	    if (sl >= 0) ylikely {
    	        DPRINTF("non-negative\n") ;
		if (sp[sl] != '\0') ylikely {
    	            DPRINTF("not nul-terminated on length\n") ;
		    if (lenstr(sp,sl) >= sl) ylikely {
    	                DPRINTF("lenstr=%d _ge_ sl=%d\n",lenstr(sp,sl),sl) ;
	                if (sl > blen) {
    	                    DPRINTF("alloc\n") ;
		            rl = alloc(sp,sl) ;
	                } else {
    	                    DPRINTF("store-in-buf\n") ;
		            rl = intconv(strwcpy(buf,sp,sl) - buf) ;
		            rp = buf ;
	                } /* end */
		    } /* end if (storeage required) */
		} /* end if (not nul-terminated as given) */
	    } /* end if (possibly required) */
	} /* end if (need calculation) */
    	DPRINTF("ret rl=%d\n",rl) ;
    	DPRINTF("ret rp=>%s<\n",((rp) ? rp : "null")) ;
	return rp ;
} /* end method (strnul::proc) */

void strnul::dtor() noex {
    	DPRINTF("ent\n") ;
	clear() ;
    	DPRINTF("ret\n") ;
} /* end method (strnul::dtor) */

strnul &strnul::operator = (cchar *sp) noex {
    	DPRINTF("ent sp=>%s<\n",sp) ;
	clear() ;
	proc(sp) ;
    	DPRINTF("ret\n") ;
	return *this ;
} /* end operator (assignment from |ccharp|) */

strnul &strnul::operator = (const strview &sv) noex {
	ccharp	sp = sv.data() ;
	cint	sl = (int) sv.length() ;
    	DPRINTF("ent\n") ;
	clear() ;
	proc(sp,sl) ;
    	DPRINTF("ret\n") ;
	return *this ;
} /* end operator (assignment from |string_view|) */

ccharp strnul::operator () (cchar *sp,int sl) noex {
    	DPRINTF("ent\n") ;
    	clear() ;
	rp = proc(sp,sl) ;
    	DPRINTF("ret\n") ;
	return rp ;
} /* end method (strnul::operator) */

ccharp strnul::operator () (const strview &sv) noex {
	ccharp	sp = sv.data() ;
	cint	sl = (int) sv.length() ;
    	DPRINTF("ent\n") ;
	clear() ;
	rp = proc(sp,sl) ;
    	DPRINTF("ret\n") ;
	return rp ;
} /* end method (strnul::operator) */

void strnul::init() noex {
    	len	(this) ;
	rl	= -1 ;
} /* end method (strnul::init) */


