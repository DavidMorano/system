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
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<sys/socket.h>		/* POSIX® */
#include	<netinet/in.h>		/* POSIX® */
#include	<arpa/inet.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<netdb.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucfileop.h>		/* LIBUC */
#include	<sockaddress.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"dialuss.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	opensockaddr(int,int,int,SOCKADDR *,int) noex ;
} /* end */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int dialuss(cchar *pathname,int to,int opts) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		fd = -1 ;
	if (pathname) ylikely {
	    rs = SR_INVALID ;
	    if (pathname[0]) ylikely {
	        if (USTAT sb ; (rs = uc_stat(pathname,&sb)) >= 0) ylikely {
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
		        } /* end if (error) */
	            } else {
		        rs = SR_NOTSOCK ;
	            } /* end if */
	        } /* end if (stat) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (dialuss) */


