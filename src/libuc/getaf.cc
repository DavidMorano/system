/* getaf SUPPORT */
/* lang=C++20 */

/* get an address-family by name */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-10-14, David A­D­ Morano
	This was written to get a roughly standardized subroutine
	to handle both IPv4 and IPv6. Note that the order of the
	AF list isn't in the order to the definitions of the defines.
	Rather, since searching is linear (probably the fastest
	way), the order is such that the most popular AFs are near
	the top!

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getaf

	Description:
	These subroutines allow for lookup of either an address-family
	name or an address-family number into a database of name-number
	pairs.  The database is not dynamic, but rather compiled
	into this module.  A dynamic database is really only possible
	if we know where the stupid real '<sys/socket.h>' file is
	located (since we would need to read and process that file
	to get the name-number pairs).  This problem really illustrates
	the stupidity of the whole BSD "socket" API and only goes
	to show the far superiority of the AT&T TLI (or XTI) API.

	Synopsis:
	int getaf(cchar *np,int nl) neox

	Arguments:
	np		name of the address family to lookup
	nl		length of name

	Returns:
	>=0		resulting address-family index
	<0		error


	Name:
	getaflen

	Description:
        This subroutine divines the length of a socket address from an
        address-family.

	Synopsis:
	int getaflen(int af) noex

	Arguments:
	af		address-family

	Return:
	>=0		length of address in address-family
	<0		error

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<cstring>
#include	<usystem.h>
#include	<uinet.h>
#include	<cfdec.h>
#include	<hasx.h>
#include	<strdcpy.h>
#include	<bufsizevar.hh>
#include	<localmisc.h>

#include	"getaf.h"


/* local defines */

#ifndef	AFNAMELEN
#define	AFNAMELEN	12		/* maximum of length in table below */
#endif

#define	ADDRFAM		struct addrfam
#define	CADDRFAM	const ADDRFAM


/* external subroutines */

extern "C" {
    extern int	nleadstr(cchar *,cchar *,int) noex ;
}


/* local structures */

struct addrfam {
	cchar		*name ;
	int		af ;
} ;


/* local variables */

static const struct addrfam	addrfamilies[] = {
	{ "unix", AF_UNIX },
	{ "inet", AF_INET },
	{ "inet4", AF_INET4 },
	{ "inet6", AF_INET6 },
#if	defined(AF_IMPLINK)
	{ "implink", AF_IMPLINK },
#endif
#if	defined(AF_PUP)
	{ "pup", AF_PUP },
#endif
#if	defined(AF_CHAOS)
	{ "chaos", AF_CHAOS },
#endif
#if	defined(AF_NS)
	{ "ns", AF_NS },
#endif
#if	defined(AF_NBS)
	{ "nbs", AF_NBS },
#endif
#if	defined(AF_ECMA)
	{ "ecma", AF_ECMA },
#endif
#if	defined(AF_DATAKIT)
	{ "datakit", AF_DATAKIT },
#endif
#if	defined(AF_CCITT)
	{ "ccitt", AF_CCITT },
#endif
#if	defined(AF_SNA)
	{ "sna", AF_SNA },
#endif
#if	defined(AF_DECnet)
	{ "decnet", AF_DECnet },
#endif
#if	defined(AF_DLI)
	{ "dli", AF_DLI },
#endif
#if	defined(AF_LAT)
	{ "lat", AF_LAT },
#endif
#if	defined(AF_HYLINK)
	{ "hylink", AF_HYLINK },
#endif
#if	defined(AF_APPLETALK)
	{ "appletalk", AF_APPLETALK },
#endif
#if	defined(AF_NIT)
	{ "nit", AF_NIT },
#endif
#if	defined(AF_802)
	{ "802", AF_802 },
#endif
#if	defined(AF_OSI)
	{ "osi", AF_OSI },
#endif
#if	defined(AF_X25)
	{ "x25", AF_X25 },
#endif
#if	defined(AF_OSINET)
	{ "osinet", AF_OSINET },
#endif
#if	defined(AF_GOSIP)
	{ "gosip", AF_GOSIP },
#endif
#if	defined(AF_IPX)
	{ "ipx", AF_IPX },
#endif
#if	defined(AF_ROUTE)
	{ "route", AF_ROUTE },
#endif
#if	defined(AF_LINK)
	{ "link", AF_LINK },
#endif
#if	defined(AF_KEY)
	{ "key", AF_KEY },
#endif
#if	defined(AF_NCA)
	{ "nca", AF_NCA },
#endif
#if	defined(AF_LOCAL)
	{ "local", AF_LOCAL },
#endif
#if	defined(AF_ISDN)
	{ "isdn", AF_ISDN },
#endif
#if	defined(AF_SYSTEM)
	{ "system", AF_SYSTEM },
#endif
#if	defined(AF_NETBIOS)
	{ "netbios", AF_NETBIOS },
#endif
#if	defined(AF_NDRV)
	{ "ndrv", AF_NDRV },
#endif
	{ "unspecified", AF_UNSPEC },
	{ nullptr, 0 }
} ;

static bufsizevar	maxpathlen(getbufsize_mp) ;


/* exported subroutines */

int getaf(cchar *np,int nl) noex {
	int		rs = SR_FAULT ;
	if (np) {
	    rs = SR_INVALID ;
	    if ((nl > 0) && np[0]) {
	        if (hasalldig(np,nl)) {
	            int		v ;
	            if ((rs = cfdeci(np,nl,&v)) >= 0) {
		        rs = v ;
	            }
	        } else {
	            cint	alen = AFNAMELEN ;
	            int		al ;
	            char	abuf[AFNAMELEN + 1] ;
	            if ((al = (strdcpy1w(abuf,alen,np,nl)-abuf)) > 0) {
	                CADDRFAM	*afs = addrfamilies ;
	                cint		n = 2 ;
	                int		i ;
	                int		m ;
	                for (i = 0 ; afs[i].name ; i += 1) {
	                    cchar	*anp = afs[i].name ;
			    bool	f = true ;
	                    m = nleadstr(anp,abuf,al) ;
	                    f = f && (m == al) ;
			    f = f && ((m >= n) || (anp[m] == '\0')) ;
			    if (f) break ;
	                } /* end for */
	                rs = (afs[i].name) ? afs[i].af : SR_AFNOSUPPORT ;
	            } /* end if */
	        } /* end if (digit or string) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getaf) */

int getaflen(int af) noex {
	int		rs = SR_OK ;
	int		alen = SR_AFNOSUPPORT ;
	switch (af) {
	case AF_UNIX:
	    if ((rs = maxpathlen) >= 0) {
	        alen = rs ;
	    }
	    break ;
	case AF_INET4:
	    alen = INET4ADDRLEN ;
	    break ;
	case AF_INET6:
	    alen = INET6ADDRLEN ;
	    break ;
	} /* end switch */
	return (rs >= 0) ? alen : rs ;
}
/* end subroutine (getaflen) */

int getaddrlen(int af) noex {
	return getaflen(af) ;
}
/* end subroutine (getaddrlen) */

/* reads out as: str-af-name */
cchar *strafname(int af) noex {
	const ADDRFAM	*afs = addrfamilies ;
	int		i ;
	for (i = 0 ; afs[i].name ; i += 1) {
	    if (afs[i].af == af) break ;
	} /* end for */
	return (afs[i].name) ? afs[i].name : "unknown" ;
}
/* end subroutine (strafname) */


