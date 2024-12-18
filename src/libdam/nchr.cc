/* nchr SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* count number of a specified character in a string */
/* version %I% last-modified %G% */

#define	CF_STRNCHR	0		/* use |strnchr(3dam)| */

/* revision history:

	= 1998-10-10, David A­D­ Morano
	This subroutine was modeled from an original assembly
	language version.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	nchr

	Description:
	This subroutine counts the occurences of a specified character
	in a string.

	Synopsis:
	int nchr(cchar *sp,int sl,int sch) noex

	Arguments:
	sp	string to test
	sl	length of string to test
	sch	character to search for (and count up)

	Returns:
	-	number of occurences of the character in the string

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<strn.h>
#include	<char.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"nchr.h"


/* local defines */

#ifndef	CF_STRNCHR
#define	CF_STRNCHR	0		/* use |strnchr(3dam)| */
#endif


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr bool		f_strnchr = CF_STRNCHR ;


/* exported variables */


/* exported subroutines */

int nchr(cchar *sp,int sl,int sch) noex {
	int		n = 0 ;
	if_constexpr (f_strnchr) {
	    cchar	*tp ;
	    if (sl < 0) sl = strlen(sp) ;
	    while ((tp = strnchr(sp,sl,sch)) != nullptr) {
	        n += 1 ;
	        sl -= ((tp+1)-sp) ;
	        sp = (tp+1) ;
	    } /* end while */
	} else {
	    while (sl-- && sp[0]) {
	       cint	ch = mkchar(sp[0]) ;
	       if (ch == sch) n += 1 ;
	       sp += 1 ;
	    } /* end while */
	} /* end if_constexpr (f_strnchr) */
	return n ;
}
/* end subroutine (nchr) */


