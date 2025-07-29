/* sfsbrk SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* return a sub-string delimited by a break character, if one is found */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	sfobrk
	sfrbrk

	Description:
	This subroutine returns a sub-string delimited between
	the start of a given string and a break character found
	within that same given string (of one id found).  If no
	break character is present, no-found result is returned.

	Synopsis:
	int sfobrk(cchar *sp,int sl,cchar *ss,cchar **rpp) noex
	int sfrbrk(cchar *sp,int sl,cchar *ss,cchar **rpp) noex

	Arguments:
	sp	string to be examined
	sl 	length of string to be examined
	sb	null-terminated string of break characters to break on
	rpp	result pointer of beginning of found break-string

	Returns:
	>=0	length of the found break-string
	<0	(will be '-1') no characters from string 'sb' present in 'sp'

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<strn.h>		/* |strnxbrk(3uc)| */
#include	<char.h>		/* |char_iswhite(3uc)| */
#include	<localmisc.h>

#include	"sfx.h"
#include	"sfxbrk.h"

import libutil ;

/* local defines */


/* local typedefs */

extern "C" {
    typedef charp (*xbrk_f)(cc *,int,cc *) noex ;
}


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct sfxbrk {
	xbrk_f	xbrk ;
	sfxbrk(xbrk_f x) noex : xbrk(x) { } ;
	int operator () (cc *,int,cc *,cc **) noex ;
    } ; /* end struct (xtrxbrk) */
} /* end namespace */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sfobrk(cchar *sp,int sl,cchar *sb,cchar **rpp) noex {
    	sfxbrk		sx(strnobrk) ;
	return sx(sp,sl,sb,rpp) ;
}
/* end subroutine (sfobrk) */

int sfrbrk(cchar *sp,int sl,cchar *sb,cchar **rpp) noex {
    	sfxbrk		sx(strnrbrk) ;
	return sx(sp,sl,sb,rpp) ;
}
/* end subroutine (sfrbrk) */


/* local subroutines */

int sfxbrk::operator () (cchar *sp,int sl,cchar *sb,cchar **rpp) noex {
	int		rl = -1 ; /* return-value */
	cchar		*rp = nullptr ;
	if (sp) ylikely {
	    if (sl < 0) sl = lenstr(sp) ;
	    if (sl > 0) ylikely {
	        while (sl && CHAR_ISWHITE(*sp)) {
	            sp += 1 ;
	            sl -= 1 ;
	        }
	        if ((rp = xbrk(sp,sl,sb)) != nullptr) {
		    rl = intconv(rp - sp) ;
		    rp = sp ;
	            while (rl && CHAR_ISWHITE(rp[rl - 1])) {
	                rl -= 1 ;
	            }
	        } /* end if (hit) */
	    } /* end if (non-zero positive) */
	} /* end if (non-null) */
	if (rpp) *rpp = rp ;
	return rl ;
} /* end method (sfxbrk:operator) */


