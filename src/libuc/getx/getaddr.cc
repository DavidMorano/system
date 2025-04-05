/* getaddr SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* get an address-family (and AF length) by name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getaddr{x}

	Description:
	This subroutines gets (retrieves) an address family index
	(whatever) given an address family name.

	Synopsis:
	int getaddrfamily(cchar *name) noex
	int getaddrlen(int af) noex
	int getaflen(int af) noex

	Arguments:
	name		name of the address family to lookup
	af		address-family number

	Returns:
	>=0		resulting address-family index
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<netdb.h>
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<nleadstr.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"getaddr.h"


/* local defines */

#ifndef	AFNAMELEN
#define	AFNAMELEN	12
#endif

#ifndef	INET4ADDRLEN
#define	INET4ADDRLEN	szof(in_addr_t)
#endif

#ifndef	INET6ADDRLEN
#define	INET6ADDRLEN	16
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct addrfamily {
	cchar		*name ;
	int		af ;
} ;


/* local variables */

constexpr addrfamily	addrfamilies[] = {
	{ "unspecified", AF_UNSPEC },
	{ "unix", AF_UNIX },
	{ "inet", AF_INET },
	{ "inet4", AF_INET4 },
	{ "inet6", AF_INET6 },
	{ nullptr, 0 }
} ;

static bufsizevar	maxpathlen(getbufsize_mp) ;

cint			afnamelen = AFNAMELEN ;

/* exported variables */


/* exported subroutines */

int getaddrfamily(cchar *name) noex {
	const addrfamily	*afs = addrfamilies ;
	int		m_max = 0 ;
	int		si = -1 ;
	int		cnamelen ;
	char		cname[afnamelen + 1] ;
	cnamelen = intconv(strwcpylc(cname,name,afnamelen) - cname) ;
	for (int i = 0 ; afs[i].name ; i += 1) {
	    cchar	*anp = afs[i].name ;
	    if (int m ; (m = nleadstr(anp,cname,cnamelen)) >= 2) {
	        if (m > m_max) {
	            m_max = m ;
	            si = i ;
	        }
	    } /* end if */
	} /* end for */
	return (si >= 0) ? afs[si].af : SR_AFNOSUPPORT ;
}
/* end subroutine (getaddrfamily) */

int getaddrlen(int af) noex {
	int		rs = SR_OK ;
        switch (af) {
        case AF_UNIX:
            rs = maxpathlen ;
            break ;
        case AF_INET4:
            rs = INET4ADDRLEN ;
            break ;
        case AF_INET6:
            rs = INET6ADDRLEN ;
            break ;
        default:
            rs = SR_AFNOSUPPORT ;
            break ;
        } /* end switch */
	return rs ;
}
/* end subroutine (getaddrlen) */


