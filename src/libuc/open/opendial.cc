/* opendial SUPPORT */
/* lang=C++20 */

/* open a channel using a dialer */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was originally written.

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
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<sigman.h>
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
	    rs = SR_INVALID ;
	    if ((di >= 0) && (af >= 0)) {
	        cint	of = O_RDWR ;
		cmode	om = 0666 ;
	        cchar	*cp ;
	        switch (dialer) {
	        case opendialer_udp:
	            cp = portspec ;
	            if ((cp == np) || (cp[0] == '\0')) cp = svc ;
	            rs = dialudp(hostspec,cp,af,to,opts) ;
	            break ;
	        case opendialer_tcp:
	            cp = portspec ;
	            if ((cp == np) || (cp[0] == '\0')) cp = svc ;
	            rs = dialtcp(hostspec,cp,af,to,opts) ;
	            break ;
	        case opendialer_tcpmux:
	            rs = dialtcpmux(hostspec,portspec,af,svc,av,to,opts) ;
	            break ;
	        case opendialer_tcpnls:
	            rs = dialtcpnls(hostspec,portspec,af,svc,to,opts) ;
	            break ;
	        case opendialer_uss:
	            cp = portspec ;
	            if ((cp == np) || (cp[0] == '\0')) cp = svc ;
	            rs = dialuss(cp,to,opts) ;
	            break ;
	        case opendialer_ussmux:
	            rs = dialussmux(portspec,svc,av,to,opts) ;
	            break ;
	        case opendialer_ussnls:
	            rs = dialussnls(portspec,svc,to,opts) ;
	            break ;
	        case opendialer_ticotsord:
	            cp = portspec ;
	            if ((cp == np) || (cp[0] == '\0')) cp = svc ;
	            rs = dialticotsord(cp,-1,to,opts) ;
	            break ;
	        case opendialer_ticotsordnls:
	            rs = dialticotsordnls(portspec,-1,svc,to,opts) ;
	            break ;
	        case opendialer_pass:
	            cp = portspec ;
	            if ((cp == np) || (cp[0] == '\0')) cp = svc ;
	            rs = dialpass(cp,to,opts) ;
	            break ;
	        case opendialer_open:
	            cp = portspec ;
	            if ((cp == np) || (cp[0] == '\0')) cp = svc ;
	            rs = uc_openenv(cp,of,0666,ev,to) ;
	            break ;
	        case opendialer_prog:
	            cp = portspec ;
	            if ((cp == np) || (cp[0] == '\0')) cp = svc ;
	            rs = dialprog(cp,oflags,av,ev,np) ;
	            break ;
	        case opendialer_finger:
	            rs = dialfinger(hostspec,portspec,af,svc,av,to,opts) ;
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


