/* sfkeyval SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* get the value part of a key-value pair in a c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A�D� Morano
	This subroutine was originally written for Rightcore Network
	Services (RNS).

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sfcontent
	sfkeyval

	Description:
	This subroutine will search for key-value pairs in the given
	string and return the value associate with the given key
	specified in the call.

	Synopsis:
	int sfcontent(cchar *sp,int sl,cchar **rpp) noex
	int sfkeyval(cchar *sp,int sl,cchar *key,cchar **rpp) noex

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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<strn.h>
#include	<six.h>
#include	<char.h>
#include	<ischarx.h>
#include	<strwcmp.h>
#include	<localmisc.h>

#include	"sfx.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sfcontent(cchar *sp,int sl,cchar **rpp) noex {
	int		rl = 0 ;
	cchar		*rp = nullptr ;
	if (sl < 0) sl = strlen(sp) ;
	if (sl > 0) {
	    if (int si ; (si = sichr(sp,sl,'#')) > 0) {
		rl = sfshrink(sp,si,&rp) ;
	    } else {
		while (sl && iseol(sp[sl-1])) {
		    sl -= 1 ;
		}
		rl = sfshrink(sp,sl,&rp) ;
	    }
	} /* end if (non-zero positive) */
	if (rpp) *rpp = rp ;
	return rl ;
}
/* end subroutine (sfcontent) */

int sfkeyval(cchar *sp,int sl,cchar *key,cchar **rpp) noex {
	int		vl = -1 ;
	cchar		*vp = nullptr ;
	if (sp) {
	    cchar	*cp{} ;
	    if (int cl ; (cl = sfcontent(sp,sl,&cp)) > 0) {
		cchar	*kp{} ;
		if (int kl ; (kl = sfnext(cp,cl,&kp)) > 0) {
		    if (strwcmp(key,kp,kl) == 0) {
			cint	xl = ((cp+cl) - (kp+kl)) ;
			cchar	*xp = (kp+kl) ;
			vl = sfnext(xp,xl,&vp) ;
		    } /* end if (strwcmp) */
		} /* end if (sfnext) */
	    } /* end if (sfcontent) */
	} /* end if (non-null) */
	if (rpp) {
	    *rpp = (vl >= 0) ? vp : nullptr ;
	}
	return vl ;
}
/* end subroutine (sfkeyval) */


