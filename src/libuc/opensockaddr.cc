/* opensockaddr SUPPORT */
/* lang=C++20 */

/* open a connection to a socket by a SOCKADDR */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	opensockaddr

	Description:
	We open a connection to something by a SOCKADDR.

	Synopsis:
	int opensockaddr(int pf,int st,int proto,SOCKADDR *sap,int to) noex
	
	Arguments:
	pf		protocol family
	st		socket type
	proto		protocol
	sap		socket-address
	to		time-out

	Returns:
	>=0		file descriptor
	<0		error in dialing (system-return)

	What is up with the 'to' argument?
	>0		use the to as it is
	==0             asynchronously span the connect and do not wait
			for it
	<0              use the system default to (xx minutes --
			whatever)

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/socket.h>
#include	<unistd.h>
#include	<usystem.h>
#include	<sockaddress.h>
#include	<localmisc.h>

#include	"opensockaddr.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local typedefs */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int opensockaddr(int pf,int st,int proto,SOCKADDR *sap,int to) noex {
	int		rs = SR_FAULT ;
	int		fd = -1 ;
	if (sap) {
	    rs = SR_INVALID ;
	    if ((pf >= 0) && (st >= 0) && (proto >= 0)) {
	        if ((rs = u_socket(pf,st,proto)) >= 0) {
	            SOCKADDRESS	*saddrp = (SOCKADDRESS *) sap ;
	            fd = rs ;
	            if ((rs = sockaddress_getlen(saddrp)) > 0) {
	                cint	sal = rs ;
	                if (to >= 0) {
	                    rs = uc_connecte(fd,sap,sal,to) ;
	                } else {
	                    rs = u_connect(fd,sap,sal) ;
	                }
	            } /* end if (sockaddress_getlen) */
	            if ((rs < 0) && (fd >= 0)) {
	                u_close(fd) ;
	            }
	        } /* end if (u_socket) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (opensockaddr) */


