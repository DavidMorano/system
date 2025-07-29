/* strnxbrk SUPPORT */
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

	Name:
	strn{x}pbrk

	Description:
	This subroutine is, of course, like the standard |strpbrk(3c)|
	except that length of the string to be tested can be given.
	If a test string length of <0 is given, then this subroutine
	acts just like |strpbrk(3c)|.

	Synopsis:
	char *str{x}npbrk(cchar *sp,int sl,cchar *ss) noex

	Arguments:
	{x}		base, case, fold
	sp		string to test
	sl		length of string to test
	ss		string of characters to compare against

	Returns:
	-		pointer to found character or nullptr if not found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strchr(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usys.h>		/* |strbrk(3u)| */
#include	<strx.h>		/* |strbrk(3u)| */
#include	<mkchar.h>
#include	<localmisc.h>

#include	"strnxbrk.h"

import libutil ;
import chrset ;

/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */

extern "C++" {
    extern char *strnobrk(cchar *,int,const chrset &) noex ;
    extern char *strnrbrk(cchar *,int,const chrset &) noex ;
}


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

char *strnobrk(cchar *sp,int sl,const chrset &sset) noex {
	char		*rsp = nullptr ;
	if (sp) ylikely {
	    bool	f = false ;
	    if (sl < 0) sl = lenstr(sp) ;
	    for (cchar *lsp = (sp + sl) ; (sp < lsp) && *sp ; sp += 1) {
		cint	ch = mkchar(*sp) ;
	        f = sset.tst(ch) ;
		if (f) break ;
	    } /* end for */
	    if (f) rsp = charp(sp) ;
	} /* end if (non-null) */
	return rsp ;
} /* end subroutine (strnobrk) */

char *strnobrk(cchar *sp,int sl,cchar *ss) noex {
	char		*rsp = nullptr ;
	if (sp && ss) ylikely {
	    if (sl >= 0) {	
		chrset	sset(ss) ;
	        rsp = strnobrk(sp,sl,sset) ;
	    } else {
	        rsp = strbrk(sp,ss) ;
	    } /* end if */
	} /* end if (non-null) */
	return rsp ;
} /* end subroutine (strnobrk) */

char *strnrbrk(cchar *sp,int sl,const chrset &sset) noex {
	char		*rsp = nullptr ;
	if (sp) ylikely {
	    bool	f = false ;
	    if (sl < 0) sl = lenstr(sp) ;
	    for (rsp = charp(sp + sl) ; --rsp >= sp ; ) {
	        cint	ch = mkchar(*rsp) ;
	        f = sset.tst(ch) ;
	        if (f) break ;
	    } /* end for */
	    if (! f) rsp = nullptr ;
	} /* end if (non-null) */
	return rsp ;
} /* end subroutine (strnrbrk) */

char *strnrbrk(cchar *sp,int sl,cchar *ss) noex {
	char		*rsp = nullptr ;
	if (sp && ss) ylikely {
	    chrset	sset(ss) ;
	    rsp = strnrbrk(sp,sl,sset) ;
	} /* end if (non-null) */
	return rsp ;
} /* end subroutine (strnrbrk) */


