/* getportnum SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get an INET port number */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getportnum

	Description:
	This subroutine tries to retrieve a port number given:
		a. a protocol name
		b. a service name

	Synopsis:
	int getportnum(cchar *protoname,cchar *portspec) noex

	Arguments:
	protoname	protocol name
	portspec	port specification to lookup 

	Return:
	>=0		port-number
	<0		error (syhstem-return)

	Notes:
	Notice that the port-name and service-name are given in
	that order (port-name fist followed by service-name).  This
	is the opposite order of (essentially) all other means
	(interfaces) of retrieving a network-service entry.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<netdb.h>
#include	<usystem.h>
#include	<getserv.h>		/* |getserv_name(3uc)| */
#include	<cfdec.h>
#include	<hasx.h>
#include	<localmisc.h>

#include	"getportnum.h"

import libutil ;

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

int getportnum(cchar *pn,cchar *ps) noex {
	int		rs = SR_FAULT ;
	int		port = -1 ;
	if (ps) ylikely {
	    rs = SR_INVALID ;
	    if (ps[0]) ylikely {
		cint	pl = lenstr(ps) ;
	        if (hasalldig(ps,pl)) {
	            rs = cfdeci(ps,pl,&port) ;
	        } /* end if */
	        if ((rs >= 0) && (port < 0) && (pn != nullptr)) {
	            if ((rs = getserv_name(ps,pn)) >= 0) {
	                port = rs ;
	            } /* end if (getserv_port) */
	        } /* end if */
	        if ((rs >= 0) && (port < 0)) {
	            rs = SR_NOTFOUND ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? port : rs ;
}
/* end subroutine (getportnum) */


