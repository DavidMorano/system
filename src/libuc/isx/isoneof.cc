/* isoneof SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* test is an integer is one of a group of integers */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	isOneOf

	Description:
	This subroutine determines if a value (given by the second
	argument) is present within the array (given by the first
	argument).

	Synopsis:
	int isOneOf(const int *rets,int rs) noex

	Arguments:
	rets		array of (constant) integers to check against
	rs		return-status from a file access

	Returns:
	TRUE		matched
	FALSE		did not match

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>

#include	"isoneof.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

bool isoneof(cint *a,int rs) noex {
	bool		f = false ;
	for (int i = 0 ; a[i] != 0 ; i += 1) {
	    f = (rs == a[i]) ;
	    if (f) break ;
	} /* end if */
	return f ;
}
/* end subroutine (isoneof) */


