/* strnxterm SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find a character in a counted string */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-06-08, David AÂ­DÂ­ Morano
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
#include	<usysbase.h>
#include	<usys.h>
#include	<strx.h>		/* |strbrk(3u)| */
#include	<mkchar.h>
#include	<baops.h>		/* |batst(3u)| */
#include	<localmisc.h>

#include	"strnxterm.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| */

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

char *strnoterm(cchar *sp,int µsl,cchar *terms) noex {
	char		*rsp = nullptr ;
	if (int sl ; terms && ((sl = getlenstr(sp,µsl)) > 0)) {
	    bool	f = false ;
	    for (cchar *lsp = (sp + sl) ; (sp < lsp) && *sp ; sp += 1) {
		cint	ch = mkchar(*sp) ;
	        f = batst(terms,ch) ;
		if (f) break ;
	    } /* end for */
	    if (f) rsp = charp(sp) ;
	} /* end if (getlenstr) */
	return rsp ;
} /* end subroutine (strnoterm) */

char *strnrterm(cchar *sp,int µsl,cchar *terms) noex {
	char		*rsp = nullptr ;
	if (int sl ; terms && ((sl = getlenstr(sp,µsl)) > 0)) {
	    bool	f = false ;
	    for (rsp = charp(sp + sl) ; --rsp >= sp ; ) {
	        cint	ch = mkchar(*rsp) ;
	        f = batst(terms,ch) ;
	        if (f) break ;
	    } /* end for */
	    if (! f) rsp = nullptr ;
	} /* end if (getlenstr) */
	return rsp ;
} /* end subroutine (strnrterm) */


