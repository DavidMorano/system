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
	int getheaddr(cchar *addr,HOSTENT *hep,char *hbuf,int hlen) noex

	Arguments:
	- addr		address to lookup
	- hep		pointer to 'hostent' structure
	- hbuf		user supplied hbuffer to hold result
	- hlen	length of user supplied hbuffer

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

typedef ucentho *	ucenthop ;


/* local typedefs */


/* external subroutines */

extern "C" {
    int	getheaddr(HOSTENT *,char *,int,cchar *) noex ;
}


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int getheaddr(HOSTENT *hep,char *hbuf,int hlen,cchar *ap) noex {
	cint		af = AF_INET ;
	int		rs = SR_FAULT ;
	if (hep && hbuf && ap) {
	    ucentho	*hop = ucenthop(hep) ;
	    cint	al = sizeof(struct in_addr) ;
	    rs = getho_addr(hop,hbuf,hlen,af,ap,al) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getheaddr) */


