/* uc_getipnodeby */
/* lang=C20 */

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
	uc_freehostent

	Description:
	This subroutine is a platform independent subroutine to
	get an INET host address entry, but does it dumbly on purpose.

	Synopsis:
	int uc_getipnodebyname(HOSTENT **rpp,cc *hostname,int af,int fl) noex
	int uc_freehostent(HOSTENT *hep) noex

	Arguments:
	- hepp		pointer to get a HOSTENT pointer
	- hostname	hostname to lookup
	- af		desired address family
	- fl		optional flags

	Returns:
	0		host was found OK
	SR_FAULT	address fault
	SR_TIMEDOUT	request timed out (bad network someplace)
	SR_NOTFOUND	host could not be found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<stdlib.h>
#include	<string.h>
#include	<netdb.h>
#include	<usystem.h>
#include	<usupport.h>
#include	<localmisc.h>


/* local defines */

#define	TO_GET		3


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* exported subroutines */

int uc_getipnodebyname(HOSTENT **hepp,cchar *hostname,int af,int fl) noex {
	int		rs = SR_FAULT ;
	if (hostname) {
	    HOSTENT	*lp = nullptr ;
	    int		herr = 0 ;
	    int		i = 0 ;
	    for (i = 0 ; i < TO_GET ; i += 1) {
	        errno = 0 ;
	        lp = getipnodebyname(hostname,af,fl,&herr) ;
	        if ((lp != nullptr) || (herr != TRY_AGAIN)) break ;
	        msleep(1000) ;
	    } /* end for */
	    if (i >= TO_GET) {
	        rs = SR_TIMEDOUT ;
	    } else if (lp == nullptr) {
	        rs = SR_NOTFOUND ;
	    }
	    if (hepp) {
	        *hepp = (rs >= 0) ? lp : nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_getipnodebyname) */

int uc_freehostent(HOSTENT *hep) noex {
	int		rs = SR_FAULT ;
	if (hep) {
	    rs = SR_OK ;
	    freehostent(hep) ;
	}
	return rs ;
}
/* end subroutine (uc_freehostent) */


