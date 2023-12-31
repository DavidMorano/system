/* dialusd SUPPORT */
/* lang=C++20 */

/* subroutine to dial over to a UNIX� domain socket in data-gram mode */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A�D� Morano
	This subroutine was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	dialusd

	Description:
	This subroutine will dial out to an UNIX� domain socket
	datagram address.

	Synopsis:
	int dialusd(cchar *dst,int to,int dot) noex

	Arguments:
	dst		path to UNIX� destination domain socket to dial to
	to		timeout ('>=0' mean use one, '-1' means don't)
	dot		any dial options

	Returns:
	>=0		file descriptor
	<0		error in dialing (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/poll.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<netdb.h>
#include	<cstdlib>
#include	<cstring>
#include	<ctime>
#include	<usystem.h>
#include	<sockaddress.h>
#include	<localmisc.h>


/* local defines */

#define	BUFLEN		(8 * 1024)


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int dialusd(cchar *dst,int to,int) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		fd = -1 ;
	if (dst) {
	    rs = SR_INVALID ;
	    if (dst[0]) {
	        cint	pf = PF_UNIX ;
	        cint	af = AF_UNIX ;
		cint	st = SOCK_DGRAM ;
		cint	proto = 0 ;
	        if ((rs = u_socket(pf,st,proto)) >= 0) {
	            sockaddress	sa ;
	            fd = rs ;
	            if ((rs = sockaddress_start(&sa,af,dst,0,0)) >= 0) {
	 	        SOCKADDR	*sap = (SOCKADDR *) &sa ;
	                int 		sal = rs ;
	                if (to > 0) {
	                    rs = u_connect(fd,sap,sal) ;
		        } else {
	                    rs = u_connect(fd,sap,sal) ;
			}
	                rs1 = sockaddress_finish(&sa) ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (sockaddress) */
	            if ((rs < 0) && (fd >= 0)) u_close(fd) ;
	        } /* end if (socket) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (dialusd) */


