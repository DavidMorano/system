/* strnxpbrk */
/* lang=C++20 */

/* find a character in a counted string */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-06-08, David A­D­ Morano
	This subroutine was originally written for some reason.

*/

/* Copyright (c) 1999 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strnpbrk

	Description:
	This subroutine is, of course, like the standard 'strpbrk(3c)'
	except that length of the string to be tested can be given.
	If a test string length of <0 is given, then this subroutine
	acts just like 'strpbrk(3c)'.

	Synopsis:
	char *strnpbrk(cchar *sp,int sl,cchar *ss) noex

	Arguments:
	sp		string to test
	sl		length of string to test
	ss		string of characters to compare against

	Returns:
	-		pointer to found character or nullptr if not found


	Name:
	strnrpbrk

	Description:
	This subroutine is similar to the standard |strnpbrk(3c)|
	except that a pointer to the last character matching a
	character in the break-string is returned (instead of a
	pointer to the first character in the break-string).

	Synopsis:
	char *strnrpbrk(cchar *sp,int sl,cchar *ss) noex

	Arguments:
	sp		pointer to string to test
	sl		length of string to test
	ss		search-string

	Returns:
	-		pointer to found character or nullptr if not found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstring>
#include	<utypedefs.h>
#include	<vlanguage.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"strn.h"


/* local defines */


/* exported subroutines */


/* exported subroutines */

char *strnpbrk(cchar *sp,int sl,cchar *ss) {
	char		*rsp = nullptr ;
	if (sp && ss) {
	    if (sl >= 0) {	
	        bool	f = false ;
	        cchar	*lsp = (sp+sl) ;
	        while ((sp < lsp) && *sp) {
		    cint	ch = mkchar(*sp) ;
	            f = (strchr(ss,ch) != nullptr) ;
		    if (f) break ;
	            sp += 1 ;
	        } /* end while */
	        rsp = (f) ? ((char *) sp) : nullptr ;
	    } else if (sl < 0) {
	        rsp = strpbrk(sp,ss) ;
	    } /* end if */
	} /* end if (non-null) */
	return rsp ;
}
/* end subroutine (strnpbrk) */

char *strnrpbrk(cchar *sp,int sl,cchar *ss) noex {
	bool		f = false ;
	char		*rsp = nullptr ;
	if (sp && ss) {
	    if (sl < 0) sl = strlen(sp) ;
	    rsp = (char *) (sp + sl) ;
	    while (--rsp >= sp) {
	        cint	ch = mkchar(*rsp) ;
	        f = (strchr(ss,ch) != nullptr) ;
	        if (f) break ;
	    } /* end while */
	} /* end if (non-null) */
	return (f) ? rsp : nullptr ;
}
/* end subroutine (strnrpbrk) */


