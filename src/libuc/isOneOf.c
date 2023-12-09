/* isOneOf */
/* lang=C20 */

/* test is an integer is one of a group of integers */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was originally written.

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
	1		matched (TRUE)
	0		did not match (false)

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<localmisc.h>

#include	"isoneof.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int isOneOf(cint *a,int rs) noex {
	int		f = false ;
	for (int i = 0 ; a[i] != 0 ; i += 1) {
	    f = (rs == a[i]) ;
	    if (f) break ;
	} /* end if */
	return f ;
}
/* end subroutine (isOneOf) */


