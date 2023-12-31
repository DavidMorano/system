/* openaddrinfo SUPPORT */
/* lang=C++20 */

/* open address-information */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A�D� Morano
	This subroutine was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	openaddrinfo

	Description:
	We open a connection the an address specified by an ADDRINFO
	structure.

	Synopsis:
	int openaddrinfo(ADDRINFO *aip,int to) noex

	Arguments:
	aip		pointer to address-information (ADDRINFO) structure
	to		optional time-out

	Returns:
	>=0		file descriptor
	<0		error in dialing (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstdlib>
#include	<cstring>
#include	<netdb.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	"openaddrinfo.h"


/* local defines */

#ifndef	ADDRINFO
#define	ADDRINFO	struct addrinfo
#endif


/* external subroutines */


/* external variables */


/* local typedefs */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int openaddrinfo(ADDRINFO *aip,int to) noex {
	cint		pf = aip->ai_family ;
	cint		st = aip->ai_socktype ;
	cint		pt = aip->ai_protocol ;
	int		rs ;
	int		fd = -1 ;
	if ((rs = uc_socket(pf,st,pt)) >= 0) {
	    SOCKADDR	*sap = aip->ai_addr ;
	    cint	sal = aip->ai_addrlen ;
	    fd = rs ;
	    if (to >= 0) {
	        rs = uc_connecte(fd,sap,sal,to) ;
	    } else {
	        rs = u_connect(fd,sap,sal) ;
	    }
	    if (rs < 0) {
	        uc_close(fd) ;
	    }
	} /* end if (u_socket) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (openaddrinfo) */


