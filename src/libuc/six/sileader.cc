/* sileader SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* retrieve the string-index into a "leader" */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-24, David A­D­ Morano
	This object module was originally written.

	= 2025-06-18, David A­D­ Morano
	I added some notational comments below.

*/

/* Copyright (c) 1998,2025 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	sileader

	Description:
	We retrieve the string-index into a "leader."

	Synopsis:
	int sileader(cchar *sp,int sl) noex

	Arguments:
	sp	c-string to search pointer
	sl	c-string to search length

	Returns:
	>=0	OK
	<0	call-usage-error (should not normally happen)

	Notes:
	1. 2025-06-18 -- After reviewing, Yes, this is an obscure
	subroutine (function).  I found where it was sometimes used.
	It is no longer used there.  Yes, as far as I know there
	were only two places where this was used that I found (I
	will not name names here).  Upon examining the code (a
	little bit) it looks like this function returns a leading
	index of the first non-white-space after skipping over a
	string sequence started by a digit character, possibly
	itself surrounded by white-space (which is skipped).
	(2025-06-18, David A­D­ Morano)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<mkchar.h>
#include	<ischarx.h>		/* |isdigitlatin(3uc)| */
#include	<localmisc.h>

#include	"six.h"			/* |sibrk(3uc)| + |siskipwht(3uc)| */


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sileader(cchar *sp,int sl) noex {
	int		si = -1 ; /* return-value */
	if (sp) ylikely {
	    if (sl > 0) ylikely {
	        if ((si = siskipwhite(sp,sl)) > 0) {
	            sp += si ;
	            sl -= si ;
	        }
		if (sl > 0) {
	            if (cint ch = mkchar(sp[0]) ; isdigitlatin(ch)) {
		        if (int ci ; (ci = sibrk(sp,sl," \t")) >= 0) {
	                    si += (ci + 1) ;
			    {
				cint	cl = (sl - (ci+1)) ;
				cchar	*cp = (sp + (ci+1)) ;
	                        si += siskipwhite(cp,cl) ;
			    } /* end block */
	                } else {
	                    si = 0 ;
		        }
	            } /* end if (is-digit) */
		} /* end if (non-zero positive) */
	    } /* end if (positive) */
	} /* end if (non-null) */
	return si ;
}
/* end subroutine (sileader) */


