/* isindomain SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutine to determine if the name is in the given domain */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written for Rightcore Network
	Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	isindomain

	Description:
	This subroutine determines if a given hostname is in the
	specified domain or not.

	Synopsis:
	int isindomain(cchar *nodename,cchar *domainname) noex

	Arguments:
	nodename	if not NULL, a buffer to hold the resulting hostname
	domainname	if not NULL, the domain name to check against

	Returns:
	TRUE		it is an INET address
	FALSE		it is not

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<strings.h>		/* BSD-header for |strcasecmp(3c)| */
#include	<usystem.h>
#include	<localmisc.h>

#include	"isindomain.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

bool isindomain(cchar *name,cchar *domainname) noex {
	bool		f = true ;
	if (cchar *tp{} ; (tp = strchr(name,'.')) != nullptr) {
	    f = false ;
	    if (tp[1]) {
		f = (strcasecmp((tp + 1),domainname) == 0) ;
	    } /* end if */
	} /* end if */
	return f ;
}
/* end subroutine (isindomain) */


