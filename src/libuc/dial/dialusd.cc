/* dialusd SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutine to dial out to a UNIX® domain socket in data-gram mode */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	dialusd

	Description:
	This subroutine will dial out to an UNIX® domain socket
	datagram address.

	Synopsis:
	int dialusd(cchar *dst,int to,int dot) noex

	Arguments:
	dst		path to UNIX® destination domain socket to dial to
	to		timeout ('>=0' mean use one, '-1' means do not)
	dot		any dial options

	Returns:
	>=0		file descriptor
	<0		error in dialing (system-return)

	Notes:
	For listening for incomming connections on a UNIX® domain
	(file-system) socket, see the subroutine |openusd(3uc)|.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/socket.h>		/* POSIX® */
#include	<netinet/in.h>		/* POSIX® */
#include	<arpa/inet.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<netdb.h>		/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<sockaddress.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"dialusd.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int uc_socket(int,int,int) noex ;
    extern int uc_connect(int,cvoid *,int) noex ;
} /* end */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int dialusd(cchar *dst,int to,int) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		fd = -1 ;
	if (dst) ylikely {
	    rs = SR_INVALID ;
	    if (dst[0]) ylikely {
	        cint	pf = PF_UNIX ;
		cint	st = SOCK_DGRAM ;
		cint	proto = 0 ;
	        if ((rs = uc_socket(pf,st,proto)) >= 0) ylikely {
	            sockaddress	sa ;
	            cint	af = AF_UNIX ;
	            fd = rs ;
	            if ((rs = sockaddress_start(&sa,af,dst,0,0)) >= 0) {
	 	        SOCKADDR	*sap = (SOCKADDR *) &sa ;
	                int 		sal = rs ;
	                if (to > 0) {
	                    rs = uc_connect(fd,sap,sal) ;
		        } else {
	                    rs = uc_connect(fd,sap,sal) ;
			}
	                rs1 = sockaddress_finish(&sa) ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (sockaddress) */
	            if ((rs < 0) && (fd >= 0)) u_close(fd) ;
	        } /* end if (socket) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (dialusd) */


