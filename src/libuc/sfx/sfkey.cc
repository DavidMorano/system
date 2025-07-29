/* sfkey SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the key part of a compound string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This subroutine was originally written for Rightcore Network
	Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sfkey

	Description:
	This subroutine will extract the key part of a compound string.
	For example, consider the following compound string:
		A=the_dog_house
	The 'A' would be the key, and the part 'the_dog_house' is
	the value.  Important: If there is no key (there is no '='
	character), then we return an error!

	Synopsis:
	int sfkey(cchar *s,int slen,cchar **rpp) noex

	Arguments:
	s		pointer to string to test
	slen		length of string to test
	rpp		pointer to result pointer to store found key

	Returns:
	>=0		length of found key string
	<0		no key was found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<strn.h>		/* |strnchr(3uc)| */
#include	<char.h>		/* |char_iswhite(3uc)| */
#include	<localmisc.h>

#include	"sfx.h"

import libutil ;

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sfkey(cchar *sp,int sl,cchar **rpp) noex {
	int		kl = -1 ; /* return-value */
	if (sp) {
	    if (sl < 0) sl = lenstr(sp) ;
	    if (cchar *tp ; (tp = strnchr(sp,sl,'=')) != nullptr) {
	        kl = intconv(tp - sp) ;
	        while ((kl > 0) && CHAR_ISWHITE(sp[kl - 1])) {
	            kl -= 1 ;
		}
	    } /* end if */
	} /* end if (non-null) */
	if (rpp) {
	    *rpp = (kl >= 0) ? sp : nullptr ;
	}
	return kl ;
}
/* end subroutine (sfkey) */


