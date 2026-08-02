/* getaddr SUPPORT */
/* charset=ISO8859-1 */
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
#include	<sys/types.h>		/* POSIX® */
#include	<sys/socket.h>		/* POSIX® */
#include	<netdb.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<uinet.h>		/* LIBU |AF_{x}(3u)| */
#include	<bufsizevar.hh>		/* LIBUC */
#include	<nleadstr.h>		/* LIBUC */
#include	<strwcpy.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"getaddr.h"


/* local defines */

#ifndef	AFNAMELEN
#define	AFNAMELEN	12
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct addrfamily {
	cchar		*name ;
	int		af ;
} ; /* end struct */


/* forward references */


/* local variables */

constexpr addrfamily	addrfamilies[] = {
	{ "unspec",	AF_UNSPEC },
	{ "unix",	AF_UNIX },
	{ "inet",	AF_INET },
	{ "inet4",	AF_INET4 },
	{ "inet6",	AF_INET6 },
	{ nullptr,	0 }
} ; /* end array */

static bufsizevar	maxpathlen(bufsize_mp) ;

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
} /* end subroutine (getaddrfamily) */

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
} /* end subroutine (getaddrlen) */


