/* wswcpynarrow SUPPORT */
/* lang=C++20 */

/* copy a narrow source string to a wide-string recipient */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano

	Originally written for Rightcore Network Services.


*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	wswcpynarrow

	Description:
	We copy a narrow source string to a wide-string destination.

	Synopsis:
	int wswcpynarrow(wchar_t *wsp,cchar *sp,int sl) noex

	Arguments:
	dp	wide-string buffer that receives the copy
	sp	the source narrow-string that is to be copied
	sl	the maximum length of the source string to be copied

	Returns:
	>=0	number of characters copied
	<0	error (buffer overflow?)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<stddef.h>		/* presumable for 'wchar_t' type */
#include	<usystem.h>
#include	<mkchar.h>
#include	<localmisc.h>


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

int wswcpynarrow(wchar_t *rarr,cchar *sp,int sl) noex {
	int		c ;
	for (c = 0 ; (c < sl) && sp[c] ; c += 1) {
	    cint	ch = mkchar(sp[c]) ;
	    rarr[c] = ch ;
	}
	rarr[c] = '\0' ;
	return c ;
}
/* end subroutine (wswcpynarrow) */


