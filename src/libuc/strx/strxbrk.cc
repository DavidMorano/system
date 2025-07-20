/* strxbrk SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find a character in a counted string */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-06-08, David A­D­ Morano
	This code was originally written for some reason.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strobrk
	strrbrk

	Description:
	These subroutines are, of course, similar the standard
	|strpbrk(3c)| except that:
       		+ the source string is counted
		+ the search for a character can occur in both obverse and
		reverse order
	Also, the given legnth can be less than one (in which case
	the length is internally computed).

	Synopsis:
	char *strobrk(cchar *s,cchar *ss) noex
	char *strobrk(cchar *s,cchar *ss) noex

	Arguments:
	s		c-string to test
	ss		sub-string to test for

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strchr(3c)| + |strrchr(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<strx.h>		/* |strbrk(3u)| */
#include	<mkchar.h>
#include	<localmisc.h>

#include	"strxbrk.h"

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

char *strobrk(cchar *s,cchar *ss) noex {
    	return strbrk(s,ss) ;
} /* end subroutine (strobrk) */

char *strrbrk(cchar *s,cchar *ss) noex {
	cint		n = lenstr(s) ;
	bool		f = false ;
	char		*rsp ;
	rsp = charp(s + n) ;
	while (--rsp >= s) {
	    cint	ch = mkchar(*rsp) ;
	    f = (strchr(ss,ch) != nullptr) ;
	    if (f) break ;
	} /* end while */
	return (f) ? rsp : nullptr ;
} /* end subroutine (strrbrk) */


