/* strwhite SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find the next white space character in a string */
/* version %I% last-modified %G% */

#define	CF_STRBRK	0		/* use |strbrk(3c)| */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strwhite

	Description:
	This subroutine will scan a string and return the first
	white-space-like character found.  This subroutine is really
	just a short-cut for something like:

		char *strpbrk(cchar *s," \t\v\f\r\n")

	Synopsis:
	char *strwhite(cchar *s) noex

	Arguments:
	s		string to search in

	Returns:
	NULL		if no white space was found
	!= NULL		the pointer to the first white space character

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strpbrk(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<char.h>		/* |char_iswhite(3uc)| */
#include	<mkchar.h>
#include	<localmisc.h>

#include	"strx.h"


/* local defines */

#ifndef	CF_STRBRK
#define	CF_STRBRK	0		/* use |strbrk(3c)| */
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

cbool		f_strbrk = CF_STRBRK ;


/* exported variables */


/* exported subroutines */

char *strwhite(cchar *s) noex {
    	char		*rsp = nullptr ;
	if (s) {
	    if_constexpr (f_strbrk) {
		rsp = strpbrk(s," \t\v\f\r\n") ;
	    } else {
	        for (int ch ; ((ch = mkchar(*s))) ; s += 1) {
		    if (char_iswhite(ch) || (ch == 'n')) break ;
	        }
	    } /* end if_constexpr (f_strbrk) */
	    rsp = charp(s) ;
	} /* end if (non-null) */
	return rsp ;
}
/* end subroutine (strwhite) */


