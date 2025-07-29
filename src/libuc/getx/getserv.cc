/* getserv SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get a network service number (port) given protocol-name and service-name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getserv_name

	Description:
	Get a service number (a port number really) given a protocol
	name and a service name.  Remember that each protocol can
	have its own port associated with any given service name.
	So therefore each service name can have a separate port
	depending on what protocol name it is associated with.

	Synopsis:
	int getserv_name(cc *svc,cc *pn) noex

	Arguments:
	svc		service name
	pn		protocol name

	Returns:
	>=0		port number
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<getxx.h>
#include	<mallocxx.h>
#include	<localmisc.h>

#include	"getserv.h"


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

int getserv_name(cchar *svc,cchar *pn) noex {
	int		rs = SR_FAULT ;
	int		port = 0 ;
	if (svc && pn) ylikely {
	    rs = SR_INVALID ;
	    if (pn[0] && svc[0]) ylikely {
	        if (char *svbuf ; (rs = malloc_sv(&svbuf)) >= 0) ylikely {
	            if (ucentsv sv ; (rs = sv.getnam(svbuf,rs,svc,pn)) >= 0) {
			uint16_t	sport = uint16_t(sv.s_port) ;
	                port = int(ntohs(sport)) ;
	            }
		    rs = rsfree(rs,svbuf) ;
	        } /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? port : rs ;
}
/* end subroutine (getserv_name) */


