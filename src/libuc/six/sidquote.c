/* sidquote SUPPORT (string-index double-quote) */
/* lang=C20 */

/* find the index of the character past a double-quoted string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This subroutine was originally written.

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
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<ascii.h>
#include	<toxc.h>
#include	<localmisc.h>

#include	"six.h"


/* local defines */


/* exported subroutines */

int sidquote(cchar *sp,int sl) noex {
	int		i ;
	bool		f_skip = false ;
	bool		f_done = false ;
	for (i = 0 ; sl && sp[i] && (! f_done) ; i += 1) {
	    cint	ch = mkchar(sp[i]) ;
	    switch (ch) {
	    case '\\':
		f_skip = (! f_skip) ;
		break ;
	    case CH_DQUOTE:
		if (! f_skip) f_done = TRUE ;
	        /* FALLTHROUGH */
	    default:
		f_skip = false ;
		break ;
	    } /* end switch */
	    sl -= 1 ;
	} /* end for */
	return i ;
}
/* end subroutine (sidquote) */


