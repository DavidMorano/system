/* strnxterm SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find a character in a counted string */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-06-08, David A­D­ Morano
	This code was originally written for some reason.

*/

/* Copyright (c) 1999 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	strnoterm
	strnrterm

	Description:
	This subroutine is, of course, like the standard |strpbrk(3c)|
	except that length of the string to be tested can be given.

	Synopsis:
	char *strnoterm(cchar *sp,int sl,cchar *ss) noex
	char *strnrterm(cchar *sp,int sl,cchar *ss) noex

	Arguments:
	{x}		o, r
	sp		string to test
	sl		length of string to test
	ss		bit-array of characters to compare against

	Returns:
	-		pointer to found character or nullptr if not found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usys.h>		/* |strbrk(3u)| */
#include	<strx.h>		/* |strbrk(3u)| */
#include	<mkchar.h>
#include	<baops.h>		/* |batst(3u)| */
#include	<localmisc.h>

#include	"strnxterm.h"

import libutil ;

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

char *strnoterm(cchar *sp,int sl,cchar *terms) noex {
	char		*rsp = nullptr ;
	if (sp && terms) ylikely {
	    bool	f = false ;
	    if (sl < 0) sl = lenstr(sp) ;
	    for (cchar *lsp = (sp + sl) ; (sp < lsp) && *sp ; sp += 1) {
		cint	ch = mkchar(*sp) ;
	        f = batst(terms,ch) ;
		if (f) break ;
	    } /* end for */
	    if (f) rsp = charp(sp) ;
	} /* end if (non-null) */
	return rsp ;
} /* end subroutine (strnoterm) */

char *strnrterm(cchar *sp,int sl,cchar *terms) noex {
	char		*rsp = nullptr ;
	if (sp && terms) ylikely {
	    bool	f = false ;
	    if (sl < 0) sl = lenstr(sp) ;
	    for (rsp = charp(sp + sl) ; --rsp >= sp ; ) {
	        cint	ch = mkchar(*rsp) ;
	        f = batst(terms,ch) ;
	        if (f) break ;
	    } /* end for */
	    if (! f) rsp = nullptr ;
	} /* end if (non-null) */
	return rsp ;
} /* end subroutine (strnrterm) */


