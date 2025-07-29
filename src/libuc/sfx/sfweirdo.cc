/* sfweirdo SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* return a sub-string starting at a break character, if one is found */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	sfweirdo

	Description:
	This subroutine returns a sub-string starting at a break
	character, if one is found within a counted source c-string.

	Synopsis:
	int sfweido(cchar *sp,int sl,cchar *ss,cchar **rpp) noex

	Arguments:
	sp	string to be examined
	sl 	length of string to be examined
	sb	null-terminated string of break characters to break on
	rpp	result pointer of beginning of found break-string

	Returns:
	>=0	length of the found break-string (from break to end of string)
	<0	(will be '-1') no characters from string 'sb' present in 'sp'

	Notes:
	The resulting 'break-string', if one is found at all, starts
	at the break character itself (not just after the
	break-character).   The resulting (returned) length is from
	the break character to the end of the original string.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<strn.h>		/* |strnobrk(3uc)| */
#include	<mkchar.h>
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

int sfweirdo(cchar *sp,int sl,cchar *sb,cchar **rpp) noex {
    	sfxbrk		sx(strnobrk) ;
	return sx(sp,sl,sb,rpp) ;
}
/* end subroutine (sfweirdo) */


/* local subroutines */

int sfxbrk::operator () (cchar *sp,int sl,cchar *sb,cchar **rpp) noex {
	bool		f = false ;
	cchar		*rp = nullptr ;
	if (sp) {
	    if (sl < 0) sl = lenstr(sp) ;
	    if ((rp = xbrk(sp,sl,sb)) != nullptr) {
		f = true ;
		sl -= intconv(rp - sp) ;
	    }
	} /* end if (non-null) */
	if (rpp) {
	    *rpp = (f) ? rp : nullptr ;
	}
	return (f) ? sl : -1 ;
} /* end method (sfxbrk:operator) */


