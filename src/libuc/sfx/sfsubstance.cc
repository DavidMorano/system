/* sfsubstance SUPPORT */
/* lang=C++20 */

/* find substantive part of string (strip garbage around items) */
/* version %I% last-modified %G% */


/* revision history:

	= 2005-06-01, David A­D­ Morano
	This subroutine was originally written.

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
	<0	error

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<char.h>
#include	<mkchar.h>
#include	<ascii.h>

#include	"sfx.h"


/* local defines */


/* external subroutines */


/* forward references */

static int	isnotours(int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int sfsubstance(cchar *sp,int sl,cchar **cpp) noex {
	int		ch ;
	if (sl < 0) sl = strlen(sp) ;
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
	if (cpp) {
	    *cpp = sp ;
	}
	return sl ;
}
/* end subroutine (sfsubstance) */


/* local subroutines */

static int isnotours(int ch) noex {
	int	f = false ;
	f = f || CHAR_ISWHITE(ch) ;
	f = f || (ch == CH_DQUOTE) ;
	f = f || (ch == CH_SQUOTE) ;
	return (! f) ;
}
/* end subroutine (isnotours) */


