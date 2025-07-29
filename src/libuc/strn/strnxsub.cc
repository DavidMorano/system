/* strnxsub SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find a substring within a larger string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

	= 2023-12-20, David A­D­ Morano
	Everyone (!) has |strnstr(3c)| now.  After all of these
	years, the world has caugþt up with me.  I must have actually
	written this subroutine back in the 1980s or so when
	|strnstr(3c)| did not yet exist.  So I am adding a compile-time
	option (see above in this file) to use the UNIX® C-lang
	Standard-Library subroutine |strnstr(3c)| when calling our
	own |strnsub(3uc)|.  Oops!  Note that the function signature
	on the standard |strnstr(3c)| is NOT the same as our own
	old |strnsub(3uc)|, so I adapt it to match.

*/

/* Copyright © 1998,2023 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strn{x}sub

	Description:
	This subroutine determines if the parameter string (argument
	'ss') is or is not a substring specified by the first two
	arguments.  This subroutine either returns a pointer to the
	the begining of the found substring or NULL if not found.

	Synopsis:
	char *strn{x}sub(cchar *sp,int sl,cchar *ss) noex

	Arguments:
	{x}	base, case, fold
	sp	string to be examined
	sl	length of string to be examined
	ss	null terminated substring to search for

	Returns:
	-	pointer to found substring
	NULL	substring was not found

	Notes:
	1. An empty (zero-length) sub-string returns TRUE; that is,
	a pointer to the first character of the c-string to be
	searched is returned (as if a match occurred right there).
	This (current) behavior mimics the function of both
	|strstr(3c)|, |strnstr(3c)|, and |strcasestr(3c)|.
	2. Note that the standard |strcasestr(3c)| subroutine cannot
	be used since it fails to properly handle Latin(-1) characters.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strstr(3c)| + |strnstr(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<nleadstr.h>
#include	<toxc.h>
#include	<char.h>
#include	<localmisc.h>

#include	"strnxsub.h"

import libutil ;

/* local defines */


/* imported namespaces */


/* local typedefs */

extern "C" {
    typedef int (*toxc_f)(int) noex ;
    typedef int (*nleadxstr_f)(cchar *,cchar *,int) noex ;
}


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct strner {
	toxc_f		toxc ;
    	nleadxstr_f	nleadxstr ;
	strner(toxc_f t,nleadxstr_f n) noex : toxc(t), nleadxstr(n) { } ;
	char *strnxsub(cchar *,int,cchar *,int) noex ;
    } ; /* end struct (strner) */
} /* end namespace */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

char *strnbasesub(cchar *sp,int sl,cchar *ss) noex {
	char		*rp = nullptr ;
	if (sp && ss) {
	    if (sl >= 0) {
		if_constexpr (syshas.strnstr) {
		    csize	slsize = size_t(sl) ;
		    rp = strnstr(sp,ss,slsize) ;
		} else {
		    if (cint sslen = lenstr(ss) ; sslen > 0) {
		        strner	so(tobc,nleadbasestr) ;
	                rp = so.strnxsub(sp,sl,ss,sslen) ;
		    } else {
	    	        rp = charp(sp) ;
	            } /* end if (positive) */
		} /* end if_constexpr (f_strnstr) */
	    } else {
	        rp = strstr(sp,ss) ;
	    }
	} /* end if (non-null) */
	return rp ;
}
/* end subroutine (strnbasesub) */

char *strncasesub(cchar *sp,int sl,cchar *ss) noex {
	char		*rp = nullptr ;
	if (sp && ss) {
	    if (cint sslen = lenstr(ss) ; sslen > 0) {
		strner	so(tolc,nleadcasestr) ;
		rp = so.strnxsub(sp,sl,ss,sslen) ;
	    } else {
		rp = charp(sp) ;
	    } /* end if (positive) */
	} /* end if (non-null) */
	return rp ;
}
/* end subroutine (strncasesub) */

char *strnfoldsub(cchar *sp,int sl,cchar *ss) noex {
	char		*rp = nullptr ;
	if (sp && ss) {
	    if (cint sslen = lenstr(ss) ; sslen > 0) {
	        strner	so(tofc,nleadfoldstr) ;
	        rp = so.strnxsub(sp,sl,ss,sslen) ;
	    } else {
		rp = charp(sp) ;
	    } /* end if (positive) */
	} /* end if (non-null) */
    	return rp ;
}
/* end subroutine (strnfoldsub) */


/* local subroutines */

char *strner::strnxsub(cchar *sp,int sl,cchar *ss,int sslen) noex {
	char		*rp = nullptr ;
        if (sl < 0) sl = lenstr(sp) ;
        if (sslen <= sl) {
            cint        sslead = toxc(ss[0]) ;
            int         i ; /* used-afterwards */
            bool        f = false ;
            for (i = 0 ; i <= (sl - sslen) ; i += 1) {
                if (toxc(sp[i]) == sslead) {
                    cint        m = nleadxstr((sp + i),ss,sslen) ;
                    f = (m == sslen) ;
                    if (f) break ;
                } /* end if */
            } /* end for */
	    if (f) rp = charp(sp + i) ;
        } /* end if (valid) */
	return rp ;
}
/* end method (strner::strnxsub) */


