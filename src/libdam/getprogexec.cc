/* getprogexec SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the program execution name */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-03-01, David A­D­ Morano
	This code was originally written as a KSH built-in command.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getprogexec

	Synopsis:
	int getprogexec(char *ebuf,int elen) noex

	Arguments:
	ebuf		buffer to store result
	elen		length of supplied retult buffer

	Returns:
	>=0		length if result
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<sncpy.h>
#include	<localmisc.h>

#include	"getprogexec.h"
#include	"getexecname.h"


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

int getprogexec(char *ebuf,int elen) noex {
	int		rs = SR_FAULT ;
	if (ebuf) {
	    rs = SR_NOSYS ;
	    ebuf[0] = '\0' ;
	    if (cchar *en ; (en = getexecname()) != nullptr) {
	        rs = sncpy1(ebuf,elen,en) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getprogexec) */


