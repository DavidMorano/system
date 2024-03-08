/* getcname SUPPORT */
/* lang=C++20 */

/* subroutine to get a canonical hostname */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This program was originally written.

	= 2017-05-06, David A.D. Morano
	This rewrote this from scratch to use the HOSTINFO object.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine is used to get a canonical INET hostname
	for a supplied name. Note carefully that the returned
	hostname, if any, may NOT be a name that can be traslated
	into a good INET address. In other words, this subroutine
	defines its own definition of a "canonical" name and that
	definition does NOT necessarily include the fact that the
	resulting name can be translated into a good INET address.
	If you want a name that is guaranteed to be translatable
	into a valid INET address, then you want to investigate the
	subroutine GETEHOSTNAME (Get Entry HostName).

	Synopsis:
	int getcname(cchar *name,char *hostname) noex

	Arguments:
	name		name to lookup
	hostname	if not NULL, a buffer to hold the resulting hostname

	Returns:
	>=0		<name> had a valid INET address
	<0		<name> did not have a valid address (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<usystem.h>
#include	<hostinfo.h>
#include	<getbufsize.h>
#include	<sncpyx.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* external variables */


/* local typedefs */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int getcname(cchar *name,char *hbuf) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (name && hbuf) {
	    if ((rs = getbufsize(getbufsize_hn)) >= 0) {
	        hostinfo	hi ;
	        cint		af = AF_UNSPEC ;
	        cint		hlen = rs ;
	        if ((rs = hostinfo_start(&hi,af,name)) >= 0) {
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
/* end subroutine (getcname) */


