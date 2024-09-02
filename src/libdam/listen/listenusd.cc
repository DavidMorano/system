/* listenusd SUPPORT */
/* lang=C++20 */

/* subroutine to listen on a UNIX® socket DGRAM port */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	listenusd

	Description:
	This subroutine listens on a UNIX® socket datagram (USD)
	for incoming messages.

	Synopsis:
	int listenusd(cchar *portspec,mode_t om,int lopts) noex

	Arguments:
	portspec	file-path to listen to
	om		open-mode
	lopts		0=nothing, 1=reuse-addr

	Returns:
	>=0		file-descriptor of UNIX-domain socket
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<netdb.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<sockaddress.h>
#include	<localmisc.h>

#include	"listenusd.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int listenusd(cchar *ps,mode_t om,int lopts) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		s = 0 ;
	if (ps) {
	    rs = SR_INVALID ;
	    if (ps[0]) {
	        cint	pf = PF_UNIX ;
	        cint	st = SOCK_DGRAM ;
	        if ((rs = u_socket(pf,st,0)) >= 0) {
	            s = rs ;
	            if (lopts & 1) {
	                cint	so = SO_REUSEADDR ;
	                cint	isize = sizeof(int) ;
	                int	one = 1 ;
	                rs = u_setsockopt(s,SOL_SOCKET,so,&one,isize) ;
	            }
	            if (rs >= 0) {
	                sockaddress	sa ;
	                cint		af = AF_UNIX ;
	                if ((rs = sockaddress_start(&sa,af,ps,0,0)) >= 0) {
	                    SOCKADDR	*sap = (SOCKADDR *) &sa ;
	                    int		sal = rs ;
	                    u_unlink(ps) ;
	                    if ((rs = u_bind(s,sap,sal)) >= 0) {
	                        om &= S_IAMB ;
	                        rs = u_chmod(ps,om) ;
	                    }
	                    rs1 = sockaddress_finish(&sa) ;
		            if (rs >= 0) rs = rs1 ;
	                } /* end if (sockaddress) */
	            } /* end if (ok) */
	            if (rs < 0) {
	                u_close(s) ;
	            }
	        } /* end if (socket) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? s : rs ;
}
/* end subroutine (listenusd) */


