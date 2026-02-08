/* strfoldcmp SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* string comparison w/ folded case */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strfoldcmp

	Description:
	This subroutine performs a c-string comparisonn using folded
	case.  This is almost identical to |strncasecmp(3c)| except
	that the comparison is done using folded case.

	Synopsis:
	int strfoldcmp(cchar *e1p,cchar *e2p) noex

	Arguments:
	e1p		first string
	e2p		second string

	Returns:
	>0		first c-string is greater than the second
	==0		both c-strings are equal
	<0		first c-string is less than the second

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<mkchar.h>
#include	<char.h>
#include	<localmisc.h>

#include	"strfoldcmp.h"


/* local defines */

#define	TOFC(ch)	CHAR_TOFC(ch)


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local subroutines */


/* local variables */


/* exported variables */


/* exported subroutines */

int strfoldcmp(cchar *e1p,cchar *e2p) noex {
	int		rc = 0 ;
	if (e1p && e2p) {
	    for (int i = 0 ; *e1p && *e2p ; i += 1) {
	        cint ch1 = TOFC(*e1p++) ;
	        cint ch2 = TOFC(*e2p++) ;
	        rc = ch1 - ch2 ;
	        if (rc) break ;
	    } /* end for */
	} /* end if (non-null) */
	return rc ;
}
/* end subroutine (strfoldcmp) */


