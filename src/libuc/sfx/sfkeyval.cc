/* sfkeyval SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the value part of a key-value pair in a c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This subroutine was originally written for Rightcore Network
	Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sfcontent
	sfkeyval

	Description:
	This subroutine will search for key-value pairs in the given
	string and return the value associate with the given key
	specified in the call.

	Synopsis:
	int sfcontent	(cchar *sp,int sl,cchar **rpp) noex
	int sfkeyval	(cchar *sp,int sl,cchar *key,cchar **rpp) noex

	Arguments:
	sp		base string pointer
	sl		base string length
	key		c-string key to search for
	rpp		pointer to result pointer to store found value

	Returns:
	>=0		length of found value c-string
	<0		no key was found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<six.h>			/* LIBUC |sichr(3uc)| */
#include	<strwcmp.h>		/* LIBUC */
#include	<ischarx.h>		/* LIBUC |iseol(3uc)| */
#include	<localmisc.h>		/* LIBU */

#include	"sfx.h"			/* |sfshrink(3uc)| + |sfnext(3uc)| */
#include	"sfkeyval.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* lenstr(3u) */

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sfcontent(cchar *sp,int µsl,cchar **rpp) noex {
	int		rl = -1 ; /* return-value */
	cchar		*rp = nullptr ;
	if (int sl = getlenstr(sp,µsl) ; sl >= 0) ylikely {
	    rl = 0 ;
	    if (sl > 0) ylikely {
	        if (int si = sichr(sp,sl,'#') ; si >= 0) {
		    sl = si ;
	        } else {
		    while (sl && iseol(sp[sl - 1])) {
			sl -= 1 ;
		    } /* end while */
	        } /* end if (comment or EOL) */
		if (sl) {
		    rl = sfshrink(sp,sl,&rp) ;
		} else {
		    rp = sp ;
		}
	    } /* end if (non-zero positive) */
	} /* end if (getlenstr) */
	if (rpp) *rpp = rp ;
	return rl ;
} /* end subroutine (sfcontent) */

int sfkeyval(cchar *sp,int sl,cchar *key,cchar **rpp) noex {
	int		vl = -1 ; /* return-value */
	cchar		*vp = nullptr ;
	if (sp) ylikely {
	    cchar *cp{} ;
	    if (int cl = sfcontent(sp,sl,&cp) ; cl > 0) {
		cchar *kp{} ;
		if (int kl = sfnext(cp,cl,&kp) ; kl > 0) {
		    if (strwcmp(key,kp,kl) == 0) {
			cint	xl = intconv((cp + cl) - (kp + kl)) ;
			cchar	*xp = (kp + kl) ;
			vl = sfnext(xp,xl,&vp) ;
		    } /* end if (strwcmp) */
		} /* end if (sfnext) */
	    } /* end if (sfcontent) */
	} /* end if (non-null) */
	if (rpp) {
	    *rpp = (vl >= 0) ? vp : nullptr ;
	}
	return vl ;
} /* end subroutine (sfkeyval) */


