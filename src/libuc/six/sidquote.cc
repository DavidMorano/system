/* sidquote SUPPORT (string-index double-quote) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find the index of the character past a double-quoted string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sidquote

	Description:
	This subroutine searches for the end of a double-quoted
	string.

	Synopsis:
	int sidquote(cchar *sp,int sl) noex

	Arguments:
	sp	string to be examined
	sl	length of string of break character to break on

	Returns:
	>=0	index of search character in the given string
	<0	the search character was not found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<ascii.h>
#include	<toxc.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"six.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sidquote(cchar *sp,int sl) noex {
	int		i = -1 ; /* used-afterwards (return value) */
	if (sp) ylikely {
	    bool	f_skip = false ;
	    bool	f_done = false ;
	    for (i = 0 ; sl && sp[i] && (! f_done) ; i += 1) {
	        cint	ch = mkchar(sp[i]) ;
	        switch (ch) {
	        case '\\':
		    f_skip = (! f_skip) ;
		    break ;
	        case CH_DQUOTE:
		    if (! f_skip) f_done = TRUE ;
		    fallthrough ;
	            /* FALLTHROUGH */
	        default:
		    f_skip = false ;
		    break ;
	        } /* end switch */
	        sl -= 1 ;
	    } /* end for */
	} /* end if (non-null) */
	return i ;
}
/* end subroutine (sidquote) */


