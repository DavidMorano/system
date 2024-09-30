/* strnfoldcmp SUPPORT */
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
	strnfoldcmp

	Description:
	This subroutine performs a c-string comparisonn using folded
	case.  This is almost identical to |strncmp(3c)| except
	that the comparison is doing using folded case.

	Synopsis:
	int strnfoldcmp(cchar *e1p,cchar *e2p,int n) noex

	Arguments:
	e1p		first string
	e2p		second string
	n		maximum number of characters to compare

	Returns:
	>0		the second key is greater than the first
	0		the keys of the strings are equal
	<0		the first key is less than the second

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<toxc.h>
#include	<localmisc.h>

#include	"strnxcmp.h"


/* local defines */


/* external subroutines */


/* external variables */


/* forward references */


/* local subroutines */


/* local variables */


/* exported variables */


/* exported subroutines */

int strnfoldcmp(cchar *e1p,cchar *e2p,int n) noex {
	int		rc = 0 ;
	for (int i = 0 ; (i < n) && *e1p && *e2p ; i += 1) {
	    rc = tofc(*e1p++) - tofc(*e2p++) ;
	    if (rc) break ;
	} /* end for */
	return rc ;
}
/* end subroutine (strnfoldcmp) */


