/* getmailgid SUPPORT */
/* lang=C++20 */

/* get the GID for the group 'mail' or the mail-spool area */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-20, David A­D­ Morano
	This was written to collect this code into one subroutines.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:

	Description:
	This subroutine returns a GID for a specified groupname.
	A default GID is returned if the groupname does not exist.

	Synopsis:
	int getmailgid(cchar *gname,gid_t gid) noex

	Arguments:
	gname		groupname to lookup
	gid		default GID if lookup fails

	Returns:
	-		GID found or the default GID

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<usystem.h>
#include	<getxid.h>		/* |getgid_group(3uc)| */
#include	<localmisc.h>

#include	"getmailgid.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* local variables */

constexpr gid_t		gidend = -1 ;


/* exported variables */


/* exported subroutines */

int getmailgid(cchar *gname,gid_t gid) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_FAULT ;
	if (gname) {
	    if ((rs = getgid_group(gname,-1)) == rsn) {
	        if (gid != gidend) {
	            rs = gid ;
	        } else {
	            rs = getgid() ;
	        }
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getmailgid) */


