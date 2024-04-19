/* sfwhitedot SUPPORT */
/* lang=C++20 */

/* get a substring present *before* the first dot */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A�D� Morano
	This subroutine gets the leading string before the first
	dot character (white-space cleaned up).

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sfwhitedot

	Description:
	This subroutine retrieves the substring *before* the first
	dot character.  If there is no explict terminator (dot
	character), an empty indication (result = 0) is returned.
	The returned string is white-space shrunken.

	Synopsis:
	int sfwhitedot(cchar *sp,int sl,cchar **rpp) noex

	Arguments:
	sp		given string
	sl		given string length
	rpp		pointer to hold result pointer

	Returns:
	>=0		length of retrieved nodename
	<0		error

	Philosophy:
	The terminator is *required* in order for a non-empty result
	string to be returned.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<char.h>
#include	<strn.h>

#include	"sfx.h"


/* local defines */


/* imported namespaces */


/* local typedefs */

typedef charp (*strxchr_f)(cchar *,int,int) noex ;


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct sub_sfxchr {
	strxchr_f	fun ;
	sub_sfxchr(strxchr_f f) noex : fun(f) { } ;
	int operator () (cchar *,int,int,cchar **) noex ;
    } ; /* end struct (sub_sfxchr) */
}


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sfochr(cchar *sp,int sl,int sch,cchar **rpp) noex {
	sub_sfxchr	so(strnochr) ;
	return so(sp,sl,sch,rpp) ;
}

int sfrchr(cchar *sp,int sl,int sch,cchar **rpp) noex {
	sub_sfxchr	so(strnrchr) ;
	return so(sp,sl,sch,rpp) ;
}

int sfwhitedot(cchar *sp,int sl,cchar **rpp) noex {
	return sfochr(sp,sl,'.',rpp) ;
}
/* end subroutine (sfwhitedot) */


/* local subroutines */

int sub_sfxchr::operator () (cchar *sp,int sl,int sch,cchar **rpp) noex {
	int		rl = -1 ;
	cchar		*rp = nullptr ;
	if (sp) {
	    if (sl < 0) sl = strlen(sp) ;
	    if (sl > 0) {
	        while (sl && CHAR_ISWHITE(*sp)) {
	            sp += 1 ;
	            sl -= 1 ;
	        }
	        if (cchar *tp ; (tp = fun(sp,sl,sch)) != nullptr) {
	            sl = (tp - sp) ;
		    rp = sp ;
	        }
	        while (sl && CHAR_ISWHITE(sp[sl - 1])) {
	            sl -= 1 ;
	        }
	        if (rp) rl = sl ;
	    } /* end if (non-zero positive) */
	} /* end if (non-null) */
	if (rpp) *rpp = rp ;
	return rl ;
}
/* end method (sub_sfxchr::operator) */


