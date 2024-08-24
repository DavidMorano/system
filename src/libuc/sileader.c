/* sileader SUPPORT */
/* lang=C++20 */

/* retrieve the string-index into a "leader" */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-24, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright (c) 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	We retrieve the string-index into a "leader."

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstring>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<ascii.h>
#include	<toxc.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"six.h"


/* local defines */


/* external subroutines */


/* exported subroutines */

int sileader(cchar *sp,int sl) noex {
	int		si = 0 ;
	if (sl > 0) {
	    int		ch ;
	    if ((si = siskipwhite(sp,sl)) > 0) {
	        sp += si ;
	        sl -= si ;
	    }
	    ch = mkchar(sp[0]) ;
	    if (isdigitlatin(ch)) {
		int	ci ;
	        if ((ci = sibreak(sp,sl," \t")) > 0) {
	            si += ci ;
	            si += siskipwhite((sp + ci),(sl - ci)) ;
	        } else {
	            si = 0 ;
		}
	    } /* end if (is-digit) */
	} /* end if (positive) */
	return si ;
}
/* end subroutine (sileader) */


