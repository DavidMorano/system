/* opendial SUPPORT */
/* lang=C++20 */

/* open a channel using a dialer */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	opendial

	Description:
	We brought several dialers together here to make it more
	convenient to allow for a range of dialers to be used for
	connecting to some service.

	Synopsis:
	int opendial(int dialer,int af,int *hn,cc *ps,cc *svc,
		mainv av,mainv ev,int to,int opts) noex

	Arguments:
	dialer		dialer to use
	af		socket address-family
	hn		host (string)
	ps		port (string)
	svc		service (string)
	av		argument string vector
	ev		argument string vector
	opts		options
	to		time-out

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<netdb.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<dial.h>
#include	<localmisc.h>

#include	"opendial.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int opendial(int di,int af,cc *hn,cc *ps,cc *svc,
		mainv av,mainv ev,int to,int opts) noex {
	int		rs = SR_FAULT ;
	if (hn) {
	    cnullptr	np{} ;
	    rs = SR_INVALID ;
	    if ((di >= 0) && (af >= 0)) {
	        cint	of = O_RDWR ;
		cmode	om = 0666 ;
	        cchar	*cp ;
	        switch (di) {
	        case opendialer_udp:
	            cp = ps ;
	            if ((cp == np) || (cp[0] == '\0')) cp = svc ;
	            rs = dialudp(hn,cp,af,to,opts) ;
	            break ;
	        case opendialer_tcp:
	            cp = ps ;
	            if ((cp == np) || (cp[0] == '\0')) cp = svc ;
	            rs = dialtcp(hn,cp,af,to,opts) ;
	            break ;
	        case opendialer_tcpmux:
	            rs = dialtcpmux(hn,ps,af,svc,av,to,opts) ;
	            break ;
	        case opendialer_tcpnls:
	            rs = dialtcpnls(hn,ps,af,svc,to,opts) ;
	            break ;
	        case opendialer_uss:
	            cp = ps ;
	            if ((cp == np) || (cp[0] == '\0')) cp = svc ;
	            rs = dialuss(cp,to,opts) ;
	            break ;
	        case opendialer_ussmux:
	            rs = dialussmux(ps,svc,av,to,opts) ;
	            break ;
	        case opendialer_ussnls:
	            rs = dialussnls(ps,svc,to,opts) ;
	            break ;
	        case opendialer_ticotsord:
	            cp = ps ;
	            if ((cp == np) || (cp[0] == '\0')) cp = svc ;
	            rs = dialticotsord(cp,-1,to,opts) ;
	            break ;
	        case opendialer_ticotsordnls:
	            rs = dialticotsordnls(ps,-1,svc,to,opts) ;
	            break ;
	        case opendialer_pass:
	            cp = ps ;
	            if ((cp == np) || (cp[0] == '\0')) cp = svc ;
	            rs = dialpass(cp,to,opts) ;
	            break ;
	        case opendialer_open:
	            cp = ps ;
	            if ((cp == np) || (cp[0] == '\0')) cp = svc ;
	            rs = uc_openenv(cp,of,om,ev,to) ;
	            break ;
	        case opendialer_prog:
	            cp = ps ;
	            if ((cp == np) || (cp[0] == '\0')) cp = svc ;
	            rs = dialprog(cp,of,av,ev,np) ;
	            break ;
	        case opendialer_finger:
	            rs = dialfinger(hn,ps,af,svc,av,to,opts) ;
	            break ;
	        default:
	            rs = SR_NOSYS ;
	            break ;
	        } /* end switch */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (opendial) */


