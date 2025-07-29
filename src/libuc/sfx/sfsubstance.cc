/* sfsubstance SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find substantive part of string (strip garbage around items) */
/* version %I% last-modified %G% */


/* revision history:

	= 2005-06-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2005 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sfsubstance

	Description:
	This subroutine finds the substantive part of the given
	string.  Basically, some qutoing is removed from the beginning
	and end of the given string (if present).  This sort of
	function is useful for removing some quoting-like cruft
	from EMA "from" strings as found in mail-message FROM
	headers.

	Synopsis:
	int sfsubstance(cchar *sp,int sl,cchar **cpp) noex

	Arguments:
	sp	supplied string to test
	sl	length of supplied string to test
	cpp	pointer to store start of found string

	Returns:
	>=0	length of result
	<0	error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<ascii.h>		/* |CH_{xx}| */
#include	<char.h>		/* |char_iswhite(3uc)| */
#include	<mkchar.h>
#include	<localmisc.h>

#include	"sfx.h"

import libutil ;

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static bool	isnotours(int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int sfsubstance(cchar *sp,int sl,cchar **rpp) noex {
    	if (sp) ylikely {
	    int		ch ; /* used-mulitple */
	    if (sl < 0) sl = lenstr(sp) ;
	    while (sl > 0) {
	        ch = mkchar(sp[0]) ;
	        if (isnotours(ch)) break ;
	        sp += 1 ;
	        sl -= 1 ;
	    }
	    while (sl > 0) {
	        ch = mkchar(sp[sl-1]) ;
	        if (isnotours(ch)) break ;
	        sl -= 1 ;
	    }
	} else {
	    sl = -1 ;
	} /* end if (non-null) */
	if (rpp) *rpp = sp ;
	return sl ;
}
/* end subroutine (sfsubstance) */


/* local subroutines */

static bool isnotours(int ch) noex {
	bool		f = false ;
	f = f || CHAR_ISWHITE(ch) ;
	f = f || (ch == CH_DQUOTE) ;
	f = f || (ch == CH_SQUOTE) ;
	return (! f) ;
}
/* end subroutine (isnotours) */


