/* sfcasesub SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* match a substring within a larger string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sfbasesub
	sfcasesub
	sffoldsub

	Description:
	This subroutine determines if the parameter string (argument
	'ss') is or is not in the buffer specified by the first two
	arguments. This subroutine either returns (-1) or it returns
	the length of the found substring.

	Synopsis:
	int sfbasesub(cchar *sp,int sl,cchar *ss,cchar **rpp) noex
	int sfcasesub(cchar *sp,int sl,cchar *ss,cchar **rpp) noex
	int sffoldsub(cchar *sp,int sl,cchar *ss,cchar **rpp) noex

	Arguments:
	sp	string to be examined
	sl	length of string to be examined
	ss	null terminated substring to search for
	rpp	result pointer of beginning of found string

	Returns:
	>=0	length of found substring
	<0	the substring was not found in the main string buffer

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<nleadstr.h>		/* |nleadcasestr(3uc)| */
#include	<toxc.h>
#include	<char.h>
#include	<localmisc.h>

#include	"sfcasesub.h"

import libutil ;

/* local defines */


/* imported namespaces */


/* local typedefs */

extern "C" {
    typedef int	(*toxc_f)(int) noex ;
    typedef int (*nleadxstr_f)(cchar *,cchar *,int) noex ;
}


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct sfxsub {
	toxc_f		toxc ;
	nleadxstr_f	nleadx ;
	sfxsub(toxc_f t,nleadxstr_f n) noex : toxc(t), nleadx(n) { } ;
	int operator () (cchar *,int,cchar *,cchar **) noex ;
    } ; /* end struct (sfxsub) */
} /* end namespace */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sfbasesub(cchar *sp,int sl,cchar *ss,cchar **rpp) noex {
    sfxsub	sx(tobc,nleadbasestr) ;
    return sx(sp,sl,ss,rpp) ;
}

int sfcasesub(cchar *sp,int sl,cchar *ss,cchar **rpp) noex {
    sfxsub	sx(touc,nleadcasestr) ;
    return sx(sp,sl,ss,rpp) ;
}

int sffoldsub(cchar *sp,int sl,cchar *ss,cchar **rpp) noex {
    sfxsub	sx(tofc,nleadfoldstr) ;
    return sx(sp,sl,ss,rpp) ;
}


/* local subroutines */

int sfxsub::operator () (cchar *sp,int sl,cchar *ss,cchar **rpp) noex {
	int		sslen = 0 ; /* return-value */
	bool		f = false ;
	if (sp && ss) ylikely {
	    int		i = 0 ; /* used-below */
	    f = true ;
	    if ((sslen = lenstr(ss)) > 0) ylikely {
	        f = false ;
	        if (sl < 0) sl = lenstr(sp) ;
	        if (sslen <= sl) ylikely {
	            cint	sslead = toxc(ss[0]) ;
	            for (i = 0 ; i <= (sl - sslen) ; i += 1) {
	                if (toxc(sp[i]) == sslead) {
		            cint	m = nleadx((sp + i),ss,sslen) ;
		            f = (m == sslen) ;
			    if (f) break ;
	                }
	            } /* end for */
	        } /* end if (possible) */
	    } /* end if (non-zero string) */
	    if (rpp) {
	        *rpp = (f) ? (sp + i) : nullptr ;
	    }
	} /* end if (non-null) */
	return (f) ? sslen : -1 ;
} /* end method (sfxsub::operator) */


