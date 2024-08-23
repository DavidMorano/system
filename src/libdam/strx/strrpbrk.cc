/* strrpbrk SUPPORT */
/* lang=C++20 */

/* find a character in a counted string */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-06-08, David A­D­ Morano
	This subroutine was originally written for some reason.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strrpbrk

	Description:
	This subroutine is, of course, like the standard |strpbrk(3c)|
	except that length of the string to be tested can be given.
	If a test string length of <0 is given, then this subroutine
	acts just like |strpbrk(3c)|.

	Synopsis:
	char *strrpbrk(cchar *s,cchar *ss) noex

	Arguments:
	s		c-string to test
	ss		sub-string to test for

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstring>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"strx.h"


/* local defines */


/* exported variables */


/* exported subroutines */

char *strrpbrk(cchar *s,cchar *ss) noex {
	cint		n = strlen(s) ;
	bool		f = false ;
	char		*rsp ;
	rsp = (char *) (s + n) ;
	while (--rsp >= s) {
	    cint	ch = mkchar(*rsp) ;
	    f = (strchr(ss,ch) != nullptr) ;
	    if (f) break ;
	} /* end while */
	return (f) ? rsp : nullptr ;
}
/* end subroutine (strrpbrk) */


