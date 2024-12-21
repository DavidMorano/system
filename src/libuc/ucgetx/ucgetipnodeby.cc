/* ucgetipnodeby SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* subroutine to get a single host entry by name (raw) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_getipnodebyname
	uc_getipnodebyaddr
	uc_freehostent
	uc_hostentfree

	Description:
	This subroutine is a platform independent subroutine to
	get an INET host address entry, but does it dumbly on purpose.

	Synopsis:
	int uc_getipnodebyname(HE **rpp,cc *hostname,int af,int fl) noex
	int uc_getipnodebyaddr(HE **rpp,cvoid *ap,int al,int af) noex
	int uc_freehostent(HE *hep) noex
	int uc_ehostentfree(HE *hep) noex

	Arguments:
	- hepp		pointer to get a HOSTENT pointer
	- hostname	hostname to lookup
	- ap		address pointer
	- al		address length
	- af		desired address family
	- fl		optional flags

	Returns:
	0		host was found OK
	SR_FAULT	address fault
	SR_TIMEDOUT	request timed out (bad network someplace)
	SR_NOTFOUND	host could not be found
	SR_PROTO	this really means some unknownerror happened 

	Notes:
	Some special errors returned by the underlying library calls are:
		TRY_AGAIN	means to try the operation again
		HOST_NOT_FOUND	means that the host (entry) was not found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netdb.h>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */

#define	TO_GET		3


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_getipnodebyname(HOSTENT **hepp,cchar *hostname,int af,int fl) noex {
	int		rs = SR_FAULT ;
	HOSTENT		*lp = nullptr ;
	if (hostname) {
	    rs = SR_INVALID ;
	    if (af >= 0) {
	        int	herr = 0 ;
		int	i ; /* used-afterwards */
	        for (i = 0 ; i < TO_GET ; i += 1) {
	            errno = 0 ;
	            lp = getipnodebyname(hostname,af,fl,&herr) ;
	            if ((lp != nullptr) || (herr != TRY_AGAIN)) break ;
	            msleep(1000) ;
	        } /* end for */
	        if (i >= TO_GET) {
	            rs = SR_TIMEDOUT ;
	        } else if (lp == nullptr) {
		    if (herr == HOST_NOT_FOUND) {
	                rs = SR_NOTFOUND ;
		    } else {
			rs = SR_PROTO ;
		    }
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	if (hepp) {
	    *hepp = (rs >= 0) ? lp : nullptr ;
	}
	return rs ;
}
/* end subroutine (uc_getipnodebyname) */

int uc_getipnodebyaddr(HOSTENT **hepp,cvoid *ap,int al,int af) noex {
	csize		asz = size_t(al) ;
	int		rs = SR_FAULT ;
	HOSTENT		*lp = nullptr ;
	if (ap) {
	    rs = SR_INVALID ;
	    if (af >= 0) {
	        int	herr = 0 ;
		int	i ; /* used-afterwards */
	        for (i = 0 ; i < TO_GET ; i += 1) {
	            errno = 0 ;
	            lp = getipnodebyaddr(ap,asz,af,&herr) ;
	            if ((lp != nullptr) || (herr != TRY_AGAIN)) break ;
	            msleep(1000) ;
	        } /* end for */
	        if (i >= TO_GET) {
	            rs = SR_TIMEDOUT ;
	        } else if (lp == nullptr) {
		    if (herr == HOST_NOT_FOUND) {
	                rs = SR_NOTFOUND ;
		    } else {
			rs = SR_PROTO ;
		    }
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	if (hepp) {
	    *hepp = (rs >= 0) ? lp : nullptr ;
	}
	return rs ;
}
/* end subroutine (uc_getipnodebyaddr) */

int uc_hostentfree(HOSTENT *hep) noex {
	int		rs = SR_FAULT ;
	if (hep) {
	    rs = SR_OK ;
	    freehostent(hep) ;
	}
	return rs ;
}
/* end subroutine (uc_hostentfree) */


