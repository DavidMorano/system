/* openusd SUPPORT */
/* lang=C++20 */

/* open UNIX®-Socket-Datagram */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-07-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:

	Description:
	This subroutine opens a UNIX® socket of the datagram variety.

	Synopsis:
	int openusd(cchar *sfn,int of,mode_t om)

	Arguments:
	sfn		socket file (in the UNIX® file-system)
	of		open-mode (O_CREAT will create the socket)
	om		permissions on socket-file

	Returns:
	>=0		file-descriptor
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/socket.h>
#include	<cstring>
#include	<usystem.h>
#include	<sockaddress.h>
#include	<localmisc.h>


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	uc_joinus(int,SOCKADDR *,int,int,mode_t) noex ;
}


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int openusd(cchar *sfn,int of,mode_t om) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		fd = -1 ;
	if (sfn) {
	    rs = SR_INVALID ;
	    if (sfn[0] && (of >= 0)) {
	        cint	pf = PF_UNIX ;
	        cint	st = SOCK_DGRAM ;
	        if ((rs = uc_socket(pf,st,0)) >= 0) {
	            sockaddress	sa ;
	            cint	af = AF_UNIX ;
	            fd = rs ;
	            if ((rs = sockaddress_start(&sa,af,sfn,0,0)) >= 0) {
		        SOCKADDR	*sap = (SOCKADDR *) &sa ;
		        cint		sal = rs ;
		        {
		            rs = uc_joinus(fd,sap,sal,of,om) ;
		        }
	  	        rs1 = sockaddress_finish(&sa) ;
			if (rs >= 0) rs = rs1 ;
	            } /* end if (sockaddress) */
	            if (rs < 0) {
	                uc_close(fd) ;
		        fd = -1 ;
	            }
	        } /* end if (socket) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (openusd) */


