/* getheaddr SUPPORT */
/* lang=C++20 */

/* subroutine to get a single host entry by its address */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getheaddr

	Deswcription:
	This subroutine is a platform independent subroutine to get
	an INET host entry by its INET address, but does it dumbly
	on purpose.

	Synopsis:
	int getheaddr(HOSTENT *hep,char *hbuf,int hlen,cvoid *addr) noex

	Arguments:
	- hep		pointer to 'hostent' structure
	- hbuf		host-entry buffer pointer
	- hlen		host-entry buffer length
	- addr		address to lookup

	Returns:
	>=0		host was found OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>
#include	<netdb.h>
#include	<usystem.h>
#include	<getxx.h>
#include	<snx.h>
#include	<localmisc.h>

#include	"getheaddr.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int getheaddr(ucentho *hep,char *hbuf,int hlen,cchar *ap) noex {
	int		rs = SR_FAULT ;
	if (hep && hbuf && ap) {
	    rs = SR_INVALID ;
	    if (ap[0]) {
	        cint	af = AF_INET ;
	        cint	al = szof(INADDR) ;
	        rs = getho_addr(hep,hbuf,hlen,af,ap,al) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getheaddr) */


