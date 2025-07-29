/* sfnamecomp SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find something inside a string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sfnamecomp

	Description:
	We find some sort of sub-string within the given string.
	We remove any slashes from the beginning of the string
	numbering more than two, and we remove all trailing slash
	characters.

	Synopsis:
	int sfnamecomp(cchar *sp,int sl,cchar **spp) noex

	Arguments:
	sp		pointer to start of user supplied buffer
	sl		length of user supplied buffer
	spp		pointer to pointer of the found field

	Returns:
	>0		length of found field
	==0		no field found or a NL character was encountered

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>

#include	"sfx.h"

import libutil ;

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static inline bool isleader(cchar *sp) noex {
	int		i = 3 ;
    	bool		f = true ;
	f = f && (sp[--i] == '/') ;
	f = f && (sp[--i] == '/') ;
	f = f && (sp[--i] == '/') ;
	return f ;
}
/* end subroutine (isleader) */


/* local variables */


/* exported variables */


/* exported subroutines */

int sfnamecomp(cchar *sp,int sl,cchar **rpp) noex {
    	if (sp) ylikely {
	    if (sl < 0) sl = lenstr(sp) ;
	    if (sl > 1) {
	        while ((sl > 2) && isleader(sp)) {
	            sp += 1 ;
	            sl -= 1 ;
	        } /* end while */
	        while ((sl > 1) && (sp[sl-1] == '/')) {
	            sl -= 1 ;
	        }
	    } /* end if (non-zero positive) */
	} else {
	    sl = -1 ;
	} /* end if (non-null) */
	if (rpp) *rpp = sp ;
	return sl ;
}
/* end subroutine (sfnamecomp) */


