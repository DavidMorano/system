/* strnxpbrk SUPPORT */
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
#include	<cstring>		/* |strchr(3c)| + |strpbrk(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"strnxpbrk.h"


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

char *strnopbrk(cchar *sp,int sl,cchar *ss) noex {
	char		*rsp = nullptr ;
	if (sp && ss) {
	    if (sl >= 0) {	
	        bool	f = false ;
	        cchar	*lsp = (sp + sl) ;
	        while ((sp < lsp) && *sp) {
		    cint	ch = mkchar(*sp) ;
	            f = (strchr(ss,ch) != nullptr) ;
		    if (f) break ;
	            sp += 1 ;
	        } /* end while */
	        rsp = (f) ? charp(sp) : nullptr ;
	    } else if (sl < 0) {
	        rsp = strpbrk(sp,ss) ;
	    } /* end if */
	} /* end if (non-null) */
	return rsp ;
}
/* end subroutine (strnopbrk) */

char *strnrpbrk(cchar *sp,int sl,cchar *ss) noex {
	bool		f = false ;
	char		*rsp = nullptr ;
	if (sp && ss) {
	    if (sl < 0) sl = strlen(sp) ;
	    rsp = charp(sp + sl) ;
	    while (--rsp >= sp) {
	        cint	ch = mkchar(*rsp) ;
	        f = (strchr(ss,ch) != nullptr) ;
	        if (f) break ;
	    } /* end while */
	} /* end if (non-null) */
	return (f) ? rsp : nullptr ;
}
/* end subroutine (strnrpbrk) */


