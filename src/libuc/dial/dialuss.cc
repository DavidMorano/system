/* dialuss SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutine to dial over to a UNIX® domain socket in stream mode */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	dialuss

	Descrption:
	This subroutine will dial out to an UNIX® domain socket
	stream address.

	Synopsis:
	int dialuss(cchar *dst,int to,int dot) noex

	Arguments:
	dst		path to UNIX® domain socket to dial to
	to		to ('>=0' mean use one, '-1' means do not)
	dot		any dial options

	Returns:
	>=0		file descriptor
	<0		error in dialing (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<netdb.h>
#include	<usystem.h>
#include	<sockaddress.h>
#include	<localmisc.h>

#include	"dialuss.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	opensockaddr(int,int,int,SOCKADDR *,int) noex ;
}


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int dialuss(cchar *pathname,int to,int opts) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		fd = -1 ;
	if (pathname) {
	    rs = SR_INVALID ;
	    if (pathname[0]) {
	        if (USTAT sb ; (rs = uc_stat(pathname,&sb)) >= 0) {
	            if (S_ISSOCK(sb.st_mode)) {
		        sockaddress	sa ;
		        cint		af = AF_UNIX ;
	                cvoid		*vp = voidp(pathname) ;
	                if ((rs = sockaddress_start(&sa,af,vp,0,0)) >= 0) {
	                    SOCKADDR	*sap = (SOCKADDR *) &sa ;
		            cint	pf = PF_UNIX ;
		            cint	st = SOCK_STREAM ;
		            cint	proto = 0 ;
		            if ((rs = opensockaddr(pf,st,proto,sap,to)) >= 0) {
	                        fd = rs ;
			        rs = dialopts(fd,opts) ;
		            }
	                    rs1 = sockaddress_finish(&sa) ;
		            if (rs >= 0) rs = rs1 ;
	                } /* end if (sockaddress) */
		        if ((rs < 0) && (fd >= 0)) {
		            u_close(fd) ;
		            fd = -1 ;
		        }
	            } else {
		        rs = SR_NOTSOCK ;
	            } /* end if */
	        } /* end if (stat) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (dialuss) */


