/* getchostname SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* get a canonical hostname */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written for Rightcore Network
	Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getchostname

	Description:
	This subroutine is used to get a canonical INET hostname
	for a supplied name.  Note carefully that the returned
	hostname, if any, may NOT be a name that can be translated
	into a good INET address.  In other words, this subroutine
	uses its own definition of a "canonical" name and that
	definition does NOT necessarily include the fact that the
	resulting name can be translated into a good INET address.
	If you want a name that is guaranteed to be translatable
	into a valid INET address, then you want to investigate the
	subroutine GETEHOSTNAME (Get Entry HostName).  Having said
	that the resuling name is not guaranteed to be translatable,
	a good translation facility will generally figure out that
	the given name is something that can be translated given
	the existing host information.

	Synopsis:
	int getchostname(char *cbuf,cchar *name) noex

	Arguments:
	cbuf		result buffer (length assumed to be MAXHOSTNAMELEN)
	name		name to lookup

	Returns:
	>=0		<name> had a valid INET address
	<0		<name> did not have a valid address (system-return)


	Name:
	getcname

	Synopsis:
	int getcname(char *cbuf,cchar *tname) noex

	Arguments:
	cbuf		buffer to hold the resulting hostname
	name		name to lookup

	Returns:
	>=0		<name> had a valid INET address
	<0		<name> did not have a valid address (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<sncpyx.h>
#include	<hostinfo.h>

#include	"getchostname.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int getchostname(char *hbuf,cchar *name) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (name && hbuf) {
	    if ((rs = getbufsize(getbufsize_hn)) >= 0) {
	        cint		af = AF_UNSPEC ;
	        cint		hlen = rs ;
	        if (hostinfo hi ; (rs = hostinfo_start(&hi,af,name)) >= 0) {
	            cchar	*cnp{} ;
	            if ((rs = hostinfo_getcanonical(&hi,&cnp)) >= 0) {
		        rs = sncpy1(hbuf,hlen,cnp) ;
		        len = rs ;
	            }
	            rs1 = hostinfo_finish(&hi) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (hostinfo) */
	    } /* end if (getbufsize) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getchostname) */


