/* getdig SUPPORT */
/* lang=C++20 */

/* create a digit from a value */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-14, David A­D­ Morano
	This little object module was first written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getdig

	Description:
	Given a value we return a character digit (anything from
	base-2 up to base-64) corresponding to that value.

	Synopsis:
	int getdig(int v) noex 

	Argument:
	v		value that we want the corresponding digit for

	Returns:
	-		the value of the digit requested

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstring>		/* <- |strlen(3c)| */
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<ucvariables.hh>
#include	<localmisc.h>

#include	"getdig.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */

static const int	maxbase = strlen(sysword.w_digtab) ;


/* exported variables */


/* exported subroutines */

int getdig(int v) noex {
	int		dig = -1 ;
	if ((v >= 0) && (v < maxbase)) {
	    dig = sysword.w_digtab[v] ;
	}
	return dig ;
}
/* end subroutine (getdig) */


