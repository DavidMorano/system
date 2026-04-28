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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<new>			/* |nothrow(3c++)| */
#include	<string>
#include	<usyscalls.h>
#include	<ulogerror.h>
#include	<localmisc.h>
#include	<dprint.hh>		/* LIBU |DPRINT(3u)| */

#include	"strnul.hh"

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
    	buf[0] = '\0' ;
	proc(sp,sl) ;
    	DPRINTF("ret\n") ;
} /* end ctor */

strnul::strnul(const strview &sv) noex {
	ccharp	sp = sv.data() ;
	cint	sl = (int) sv.length() ;
    	DPRINTF("ent as=%p\n",as) ;
    	buf[0] = '\0' ;
	proc(sp,sl) ;
    	DPRINTF("ret\n") ;
} /* end ctor */

void strnul::clear() noex {
    	DPRINTF("ent as=%p\n",as) ;
	rp = nullptr ;
    	buf[0] = '\0' ;
	if (as) {
    	    DPRINTF("delete\n") ;
	    delete [] as ;
	    as = nullptr ;
	}
    	DPRINTF("ret as=%p\n",as) ;
    	DPRINTF("ret\n") ;
} /* end method (strnul::proc) */

void strnul::alloc(cchar *sp,int sl) noex {
    	DPRINTF("ent as=%p\n",as) ;
	if ((as = new(nothrow) char[sl + 1]) != nullptr) {
	    strwcpy(as,sp,sl) ;
	    rp = as ;
	} else {
	    ulogerror("strnul",SR_NOMEM,"mem-alloc failure") ;
	    rp = "«mem-alloc-failure»" ;
	    fok = false ;
	}
    	DPRINTF("ret as=%p\n",as) ;
    	DPRINTF("ret\n") ;
} /* end method (strnul::alloc) */

ccharp strnul::proc(cchar *sp,int sl) noex {
    	DPRINTF("ent sl=%d\n",sl) ;
    	DPRINTF("as=%p\n",as) ;
    	if (sp) {
    	if (sl < 0) sl = lenstr(sp) ;
	string	str(sp,sl) ;
    	DPRINTF("ent s=>%s<\n",str.c_str()) ;
	}
	if (sp) {
    	    DPRINTF("non-null\n") ;
	    rp = sp ;
	    if (sl >= 0) {
    	        DPRINTF("non-negative\n") ;
		if (sp[sl] != '\0') {
    	            DPRINTF("not nul-terminated on length\n") ;
		    if (lenstr(sp,sl) >= sl) {
    	                DPRINTF("lenstr=%d _ge_ sl=%d\n",lenstr(sp,sl),sl) ;
	                if (sl > blen) {
    	                    DPRINTF("alloc\n") ;
		            alloc(sp,sl) ;
	                } else {
    	                    DPRINTF("store-in-buf\n") ;
		            strwcpy(buf,sp,sl) ;
		            rp = buf ;
	                }
		    } /* end if (storeage required) */
		} /* end if (not nul-terminated as given) */
	    } /* end if (possibly required) */
	} /* end if (need calculation) */
    	DPRINTF("ret as=%p\n",as) ;
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


