/* sfbaselib SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* string-find a base-library name */
/* version %I% last-modified %G% */


/* revision history:

	= 2017-12-08, David A­D­ Morano
        The subroutine was extracted from where it was first developed.

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sfbaselib

	Description:
	This subroutine finds a the base-library name (part) from
	a given string.

	Synopsis:
	int sfbaselib(cchar *sp,int sl,cchar **rpp) noex

	Arguments:
	sp	supplied string to test
	sl	length of supplied string to test
	rpp	pointer to store result "thing" pointer

	Returns:
	>=0	length of result "thing" 
	<0	error

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstring>		/* <- for |strncmp(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<estrings.h>

#include	"sfx.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sfbaselib(cchar *pnp,int pnl,cchar **rpp) noex {
	int		cl ;
	cchar		*cp ;
	cchar		*tp ;
	if ((cl = sfbasename(pnp,pnl,&cp)) > 0) {
	    pnp = cp ;
	    pnl = cl ;
	}
	if ((tp = strnrchr(pnp,pnl,'.')) != nullptr) {
	    pnl = (tp-pnp) ;
	}
	if ((pnl > 3) && (strncmp(pnp,"lib",3) == 0)) {
	    pnp += 3 ;
	    pnl -= 3 ;
	}
	if (rpp) *rpp = (char *) pnp ;
	return pnl ;
}
/* end subroutine (sfbaselib) */


