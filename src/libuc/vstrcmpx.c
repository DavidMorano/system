/* vstrcmpx */
/* lang=C20 */

/* vstring key comparison (obverse and reverse) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	vstrcmp

	Description:
	This subroutine is suitable for uses such as with |qsort(3c)|
	or |bsearch(3c)| and |vecstr_finder(3dam)|.

	Arguments:
	int vstrcmp(cchar **,cchar **) noex

	Returns:
	-		three-way comparison value


	Name:
	vstrcmpr

	Description:
	This subroutine is suitable for uses such as with |qsort(3c)|
	or |bsearch(3c)| and |vecstr_finder(3dam)|.

	Arguments:
	int vstrcmpr(cchar **,cchar **) noex

	Returns:
	-		three-way comparison value

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<stdlib.h>
#include	<string.h>		/* <- for |strcmp(3c)| */
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<mkchar.h>

#include	"vstrcmpx.h"


/* local defines */


/* local typedefs */


/* external subroutines */


/* external variables */


/* forward references */

static int vstrcmpx(cchar **s1pp,cchar **s2pp) noex {
	int		rc = 0 ;
	cchar		*s1 = (cchar *) *s1pp ;
	cchar		*s2 = (cchar *) *s2pp ;
	if (s1 || s2) {
	    rc = 1 ;
	    if (s1) {
		rc = -1 ;
		if (s2) {
		    cint	ch1 = mkchar(*s1) ;
		    cint	ch2 = mkchar(*s2) ;
		    if ((rc = (ch1 - ch2)) == 0) {
		        rc = strcmp(s1,s2) ;
		    }
		}
	    }
	}
	return rc ;
}
/* end subroutine (vstrcmpx) */


/* local variables */


/* exported subroutines */

int vstrcmpo(cchar **e1p,cchar **e2p) noex { /* obverse */
	return vstrcmpx(e1p,e2p) ;
}
/* end subroutine (vstrcmpo) */

int vstrcmpr(cchar **e1p,cchar **e2p) noex { /* reverse */
	return vstrcmpx(e2p,e1p) ;
}
/* end subroutine (vstrcmpr) */


