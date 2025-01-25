/* strnxsub SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* find a substring within a larger string */
/* version %I% last-modified %G% */

#define	CF_STRNSTR	1		/* used standard |strnstr(3c)| */
#define	CF_STRCASESTR	1		/* used standard |strcasestr(3c)| */

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
	old |strnsub(3uc)|, so I adapt it to match.  Everything
	diddo w/ the (now) standard |strcasestr(3c)|.

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

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* <- |strstr(3c)| + |strnstr(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<toxc.h>
#include	<nleadstr.h>
#include	<char.h>
#include	<localmisc.h>

#include	"strnxsub.h"


/* local defines */

#ifndef	CF_STRNSTR
#define	CF_STRNSTR	0
#endif

#ifndef	CF_STRCASESTR
#define	CF_STRCASESTR	0
#endif


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
}


/* forward references */


/* local variables */

constexpr bool	f_strnstr = CF_STRNSTR ;
constexpr bool	f_strcasestr = CF_STRCASESTR ;


/* exported variables */


/* exported subroutines */

char *strnbasesub(cchar *sp,int sl,cchar *ss) noex {
	char		*rp = nullptr ;
	if (sp && ss) {
	    if (sl >= 0) {
		if_constexpr (f_strnstr) {
		    rp = strnstr(sp,ss,sl) ;
		} else {
		    strner	so(tobc,nleadbasestr) ;
		    cint	sslen = strlen(ss) ;
	            rp = so.strnxsub(sp,sl,ss,sslen) ;
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
	    cint	sslen = strlen(ss) ;
	    rp = charp(sp) ;
	    if (sslen > 0) {
		if (sl >= 0) {
		    strner	so(tolc,nleadcasestr) ;
		    rp = so.strnxsub(sp,sl,ss,sslen) ;
		} else {
		    if_constexpr (f_strcasestr) {
		        rp = strcasestr(sp,ss) ;
		    } else {
		        strner	so(tolc,nleadcasestr) ;
		        rp = so.strnxsub(sp,sl,ss,sslen) ;
		    } /* end if_constexpr (f_strcasestr) */
		} /* end if */
	    } /* end if (positive) */
	} /* end if (non-null) */
	return rp ;
}
/* end subroutine (strncasesub) */

char *strbfoldsub(cchar *sp,int sl,cchar *ss) {
	strner	so(tofc,nleadfoldstr) ;
	cint	sslen = strlen(ss) ;
    	return so.strnxsub(sp,sl,ss,sslen) ;
}
/* end subroutine (strnfoldsub) */


/* local subroutines */

char *strner::strnxsub(cchar *sp,int sl,cchar *ss,int sslen) noex {
	char		*rp = nullptr ;
        if (sl < 0) sl = strlen(sp) ;
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
            rp = charp((f) ? (sp + i) : nullptr) ;
        } /* end if */
	return rp ;
}
/* end method (strner::strnxsub) */


