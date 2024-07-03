/* sfbasename SUPPORT */
/* lang=C++20 */

/* get the base file name out of a path */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-17, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sfbasename

	Description:
	This routine returns the length of the base name portion
	of the given path string.

	Synopsis:
	int sfbasename(cchar *sp,int sl,cchar **rpp) noex

	Arguments:
	sp	given path string
	sl	length of given path string (can be -1)
	rpp	result pointer of beginning of found string

	Returns:
	>0	length of found string
	==0	not found (no base-name)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>

#include	"sfx.h"


/* local defines */


/* exported variables */


/* exported subroutines */

int sfbasename(cchar *sp,int sl,cchar **rpp) noex {
	int		si ;
	if (sl < 0) sl = strlen(sp) ;
	while ((sl > 1) && (sp[sl - 1] == '/')) {
	    sl -= 1 ;
	}
	for (si = sl ; si > 0 ; si -= 1) {
	    if (sp[si - 1] == '/') break ;
	}
	if ((sl == 1) && (si == 1) && (sp[0] == '/')) {
	    si -= 1 ;
	}
	if (rpp) {
	    *rpp = (sp + si) ;
	}
	return (sl - si) ;
}
/* end subroutine (sfbasename) */


