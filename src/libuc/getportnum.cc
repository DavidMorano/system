/* getportnum SUPPORT */
/* lang=C++20 */

/* get an INET port number */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was originally written.

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

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<cstdlib>
#include	<cstring>		/* for |strlen(3c)| */
#include	<netdb.h>
#include	<usystem.h>
#include	<hasx.h>
#include	<cfdec.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */

extern "C" {
    extern int	getserv_name(cchar *,cchar *) noex ;
}


/* external variables */


/* local typedefs */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int getportnum(cchar *pn,cchar *ps) noex {
	int		rs = SR_FAULT ;
	int		port = -1 ;
	if (ps) {
	    rs = SR_INVALID ;
	    if (ps[0]) {
		cint	pl = strlen(ps) ;
	        if (hasalldig(ps,pl)) {
	            rs = cfdeci(ps,pl,&port) ;
	        } /* end if */
	        if ((rs >= 0) && (port < 0) && (pn != nullptr)) {
	            if ((rs = getserv_name(pn,ps)) >= 0) {
	                port = rs ;
	            } /* end if (getserv_name) */
	        } /* end if */
	        if ((rs >= 0) && (port < 0)) {
	            rs = SR_NOTFOUND ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? port : rs ;
}
/* end subroutine (getportnum) */


