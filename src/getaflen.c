/* getaflen */
/* lang=C++20 */

/* get the length of a socket address based on its address-family */
/* version %I% last-modified %G% */


/* revision history:

	= 2005-02-03, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2005 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

        This subroutine divines the length of a socket address from an
        address-family.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<limits.h>
#include	<cstring>
#include	<usystem.h>
#include	<uinet.h>
#include	<bufsizevar.hh>
#include	<localmisc.h>

#include	"getaf.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static bufsizevar		maxpathlen(getbufsize_mp) ;


/* exported subroutines */

int getaflen(int af) noex {
	int		rs = SR_OK ;
	int		alen = SR_AFNOSUPPORT ;
	switch (af) {
	case AF_UNIX:
	    if ((rs = maxpathlen) >= 0) {
	        alen = rs ;
	    }
	    break ;
	case AF_INET4:
	    alen = INET4ADDRLEN ;
	    break ;
	case AF_INET6:
	    alen = INET6ADDRLEN ;
	    break ;
	} /* end switch */
	return (rs >= 0) ? alen : rs ;
}
/* end subroutine (getaflen) */

int getaddrlen(int af) noex {
	return getaflen(af) ;
}
/* end subroutine (getaddrlen) */


