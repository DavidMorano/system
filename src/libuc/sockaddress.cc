/* sockaddress SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* manipulate socket addresses */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-01-13, David A­D­ Morano
	This object module was originally written.

	= 2025-07-17, David A­D­ Morano
	I added comments below about my review of this code for
	C++20 conformance.

*/

/* Copyright © 1998,2025 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	sockaddress

	Description:
	This little object allows for some basic manipulations of
	socket addresses.  This code was pretty much born out of
	the frustration of dealing with different "socket-address"
	types (structures), and strives to provode some (minimal)
	uninimity in dealing with socket addresses.

	Notes:
	1. The length of a UNIX® domain (AF_UNIX) socket address
	includes both the byte(s) of the address family field (as
	everyone would expect) but it also includes the trailing
	NUL byte at the end of the filepath string.  Some operating
	systems, like Syhstem-V and Solaris®, can handle a length
	that does not include a trailing NUL character.  I applaud
	their thoughful handling of this detail.
	2. One of the (most) annoying aspect of the SOCKADDR object
	is that it is different in some of its most fundamental
	ways with regard to the address-family memver value.  On
	BSD and BSD-derived systems, the address-family is only a
	single byte long.  But on System-V systems or derived
	systems, that same address-family is two bytes long.  Who
	is correct (or more correct) here, I do not know.  But I
	do know that this makes for a (relatively) painful hnalding
	of this value when one (like me) tries to make an object
	like this (supposedly an alias of a sort to the SOCKADDR
	object) portable across different operating systems.

	Updated-note:
	2025-07-17: I am reviewing this code for C++20 compliance.
	I did not find any non-conforming code with respect to
	C++20.  I do note that there is some code below that is
	likely not required and is otherwise contradictory.  For
	example, the handling of the "port" value in both INET4 and
	INET6 socket address types is (or should be) unambiguously
	a two-byte quantity and should never be a one-byte quantity.
	But there was provision below that it could be (possibly)
	a one-byte quantiy.  Although I do not think that one-byte
	code will ever be used (at least for the "port" value) it
	is not (currently) doing any harm, as far as I can tell.
	But I think in the future I should clean up the code and
	remove the provision for a one-byte "port" value.  I am
	leaving it for now (I might clean this up in the future).

	Assumpyions:
	1. The address-family value is located at the same place in
	the object across all address families.
	2. The port value is located at the same place in all of the
	INET related (INET4 and INET6) objects.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strncpy(3c)| + |stpncpy(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>		/* INET stuff */
#include	<endian.h>		/* |htoni(3u)| + |ntohi(3u)| */
#include	<sncpyx.h>		/* |sncpy(3uc)| */
#include	<strn.h>		/* |strncpybc(3uc)| */
#include	<cthexstr.h>		/* |cthexstr(3uc)| */
#include	<mkchar.h>
#include	<localmisc.h>

#include	"sockaddress.h"

import libutil ;
import digtab ;				/* |getdig(3u)| */

/* local defines */

#define	SA	sockaddress


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef const void	cv ;


/* external subroutines */


/* local structures */

namespace {
    struct sa : sockaddress {
	int loadunix(int af,cv *,int,uint) noex ;
	int loadinet4(int af,cv *,int,uint) noex ;
	int loadinet6(int af,cv *,int,uint) noex ;
	int afget() noex ;
	int afput(int) noex ;
	int portput(int) noex ;
    } ; /* end struct (sa) */
} /* end namespace */


/* forward references */


/* local variables */

constexpr int		inet4addrlen = int(INET4ADDRLEN) ;
constexpr int		inet6addrlen = int(INET6ADDRLEN) ;
constexpr int		maxpath = SOCKADDRESS_MAXPATH ;


/* exported variables */


/* exported subroutines */

int sockaddress_start(SA *sap,int af,cvoid *va,int port,uint flow) noex {
    	SOCKADDRESS	*hop = sap ;
	sa		*extp = (sa *) sap ;
	int		rs = SR_FAULT ;
	int		salen = 0 ; /* return-value */
	if (sap && va) {
	    memclear(hop) ;
	    switch (af) {
	    case AF_UNIX:
		rs = extp->loadunix(af,va,port,flow) ;
		break ;
	    case AF_INET4:
		rs = extp->loadinet4(af,va,port,flow) ;
		break ;
	    case AF_INET6:
		rs = extp->loadinet6(af,va,port,flow) ;
		break ;
	    default:
		rs = SR_NOTSUP ;
		break ;
	    } /* end switch */
	    salen = rs ;
	} /* end if (non-null) */
	return (rs >= 0) ? salen : rs ;
} /* end subroutine (sockaddress_start) */

int sa::loadunix(int af,cv *va,int,uint) noex {
	int		salen{} ;
	afput(af) ;
	{
	    cchar *addr = ccharp(va) ;
	    char  *dst = a_unix.sun_path ;
	    {
	        char *bp = strncpybc(dst,addr,maxpath) ;
	        *bp = '\0' ;
	        salen = (szof(ushort) + intconv(bp - dst)) ;
	    } /* end block */
	} /* end block */
	return salen ;
} /* end method (sa::loadunix) */

int sa::loadinet4(int af,cv *va,int port,uint) noex {
    	int		rs = SR_INVALID ;
	cint		salen = szof(SOCKADDR_IN4) ;
	if (port >= 0) {
	    afput(af) ;
	    portput(port) ;
	    if (va) {
	        cint	alen = szof(in4_addr_t) ;
	        memcopy(&a_in4.sin_addr,va,alen) ;
	    }
	} /* end if (valid) */
	return (rs >= 0) ? salen : rs ;
} /* end method (sa::loadinet4) */

int sa::loadinet6(int af,cv *va,int port,uint flow) noex {
    	int		rs = SR_INVALID ;
	cint		salen = szof(SOCKADDR_IN6) ;
	if (port >= 0) {
	    afput(af) ;
	    portput(port) ;
	    a_in6.sin6_flowinfo = htoni(flow) ;
	    if (va) {
	         cint	alen = szof(in6_addr_t) ;
	         memcopy(&a_in6.sin6_addr,va,alen) ;
	    }
	} /* end if (valid) */
	return (rs >= 0) ? salen : rs ;
} /* end method (sa::loadinet6) */

int sockaddress_finish(SA  *sap) noex {
	int		rs = SR_FAULT ;
	if (sap) {
	    sap->a_unspec.sa_family = 0 ;
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sockaddress_finish) */

/* get the address family type for this sockaddress */
int sockaddress_getaf(SA *sap) noex {
	sa		*extp = (sa *) sap ;
	int		rs = SR_FAULT ;
	if (sap) {
	    rs = extp->afget() ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sockaddress_getaf) */

/* get the port for this sockaddress (based on address family type) */
int sockaddress_getport(SA *sap) noex {
	sa		*extp = (sa *) sap ;
	int		rs = SR_FAULT ;
	int		port = 0 ;
	if (sap) {
	    cint	af = extp->afget() ;
	    rs = SR_OK ;
	    switch (af) {
	    case AF_UNIX:
		rs = SR_INVALID ;
		break ;
	    case AF_INET4:
		{
	            SOCKADDR_IN4 *inet4_sap = (SOCKADDR_IN4 *) sap ;
	            port = ntohs(inet4_sap->sin_port) ;
		}
	        break ;
	    case AF_INET6:
		{
	            SOCKADDR_IN6 *inet6_sap = (SOCKADDR_IN6 *) sap ;
	            port = ntohs(inet6_sap->sin6_port) ;
		}
	        break ;
	    default:
	        rs = SR_NOTSUP ;
	        break ;
	    } /* end switch */
	    port &= USHORT_MAX ;
	} /* end if (non-null) */
	return (rs >= 0) ? port : rs ;
}
/* end subroutine (sockaddress_getport) */

/* get the address for this sockaddress (based on address type) */
int sockaddress_getaddr(SA *sap,char *abuf,int alen) noex {
	sa		*extp = (sa *) sap ;
	int		rs = SR_FAULT ;
	if (sap && abuf) {
	    cint	af = extp->afget() ;
	    cchar	*sp ;
	    rs = SR_OK ;
	    switch (af) {
	    case AF_UNIX:
		{
	            SOCKADDR_UN	*unix_sap = (SOCKADDR_UN *) sap ;
	            sp = unix_sap->sun_path ;
	            if (alen < 0) alen = maxpath ;
	            rs = sncpy(abuf,alen,sp) ;
		}
	        break ;
	    case AF_INET4:
		{
	            SOCKADDR_IN4 *inet4_sap = (SOCKADDR_IN4 *) sap ;
	            if (alen >= inet4addrlen) {
	                rs = inet4addrlen ;
	                memcopy(abuf,&inet4_sap->sin_addr,inet4addrlen) ;
	            } else {
	                rs = SR_TOOBIG ;
	            }
		}
	        break ;
	    case AF_INET6:
		{
	            SOCKADDR_IN6 *inet6_sap = (SOCKADDR_IN6 *) sap ;
	            if (alen >= inet4addrlen) {
	                rs = inet6addrlen ;
	                memcopy(abuf,&inet6_sap->sin6_addr,inet6addrlen) ;
	            } else {
	                rs = SR_TOOBIG ;
	            }
		}
	        break ;
	    default:
	        rs = SR_NOTSUP ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sockaddress_getaddr) */

int sockaddress_getflow(SA *sap,uint *rp) noex {
	sa		*extp = (sa *) sap ;
	int		rs = SR_FAULT ;
	if (sap) {
	    cint	af = extp->afget() ;
	    uint	flow = 0 ;
	    rs = SR_OK ;
	    switch (af) {
	    case AF_UNIX:
	    case AF_INET4:
	        rs = SR_INVALID ;
	        break ;
	    case AF_INET6:
		{
	    	    SOCKADDR_IN6*inet6_sap = (SOCKADDR_IN6 *) sap ;
	            flow = ntohi(inet6_sap->sin6_flowinfo) ;
		}
	        break ;
	    default:
	        rs = SR_NOTSUP ;
	        break ;
	    } /* end switch */
	    if (rp) *rp = flow ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sockaddress_getflow) */

int sockaddress_getscope(SA *sap,uint *rp) noex {
	sa		*extp = (sa *) sap ;
	int		rs = SR_FAULT ;
	if (sap) {
	    cint	af = extp->afget() ;
	    uint	v = 0 ;
	    rs = SR_OK ;
	    switch (af) {
	    case AF_UNIX:
	    case AF_INET4:
	        rs = SR_INVALID ;
	        break ;
	    case AF_INET6:
		{
	            SOCKADDR_IN6 *inet6_sap = (SOCKADDR_IN6 *) sap ;
	            v = ntohi(inet6_sap->sin6_scope_id) ;
		}
	        break ;
	    default:
	        rs = SR_NOTSUP ;
	        break ;
	    } /* end switch */
	    if (rp) *rp = v ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sockaddress_getscope) */

int sockaddress_getextra(SA *sap,uint *rp) noex {
	sa		*extp = (sa *) sap ;
	int		rs = SR_FAULT ;
	if (sap) {
	    cint	af = extp->afget() ;
	    uint	v = 0 ;
	    rs = SR_OK ;
	    switch (af) {
	    case AF_UNIX:
	    case AF_INET4:
	        rs = SR_INVALID ;
	        break ;
	    case AF_INET6:
	        {
	            SOCKADDR_IN6 *inet6_sap = (SOCKADDR_IN6 *) sap ;
	            v = ntohi(inet6_sap->sin6_scope_id) ;
	        }
	        break ;
	    default:
	        rs = SR_NOTSUP ;
	        break ;
	    } /* end switch */
	    if (rp) *rp = v ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sockaddress_getextra) */

/* get the entire socket address (the TLI address) in HEXADECIMAL */
int sockaddress_gethex(SA *sap,char *rbuf,int rlen) noex {
	sa		*extp = (sa *) sap ;
	int		rs = SR_FAULT ;
	int		j = 0 ; /* return-value */
	if (sap && rbuf) {
	    cint	af = extp->afget() ;
	    int		salen = 0 ;
	    rs = SR_OK ;
	    switch (af) {
	    case AF_UNIX:
		{
	            SOCKADDR_UN	*unix_sap = (SOCKADDR_UN *) sap ;
	            {
	                cchar	*ap = unix_sap->sun_path ;
	                salen = min((2 + lenstr(ap) + 1),szof(SA)) ;
	            }
		} /* end block */
	        break ;
	    case AF_INET4:
	        salen = szof(SOCKADDR_IN4) ;
	        break ;
	    case AF_INET6:
	        salen = szof(SOCKADDR_IN6) ;
	        break ;
	    default:
	        rs = SR_NOTSUP ;
	        break ;
	    } /* end switch */
	    if (rs >= 0) {
		if (rlen >= (salen * 2)) {
	            for (int i = 0 ; i < salen ; i += 1) {
	                cint v = mkchar(sap->str[i]) ;
	                rbuf[j++] = getdig((v >> 4) & 15) ;
	                rbuf[j++] = getdig((v >> 0) & 15) ;
	            } /* end for */
	            rbuf[j] = '\0' ;
	        } else {
	            rs = SR_TOOBIG ;
	        } /* end if (ok) */
	    } /* end if (ok) */
	} /* end if (non-null) */
	return (rs >= 0) ? j : rs ;
}
/* end subroutine (sockaddress_gethex) */

/* get the entire socket address length (like for a TPI address) */
int sockaddress_getlen(SA *sap) noex {
	sa		*extp = (sa *) sap ;
	int		rs = SR_FAULT ;
	int		salen = 0 ;
	if (sap) {
	    cint	af = extp->afget() ;
	    rs = SR_OK ;
	    switch (af) {
	    case AF_UNIX:
		{
	            SOCKADDR_UN	*unix_sap = (SOCKADDR_UN *) sap ;
	            {
	                cchar	*ap = unix_sap->sun_path ;
	                salen = min((2 + lenstr(ap) + 1),szof(SA)) ;
	            }
		} /* end block */
	        break ;
	    case AF_INET4:
	        salen = szof(SOCKADDR_IN4) ;
	        break ;
	    case AF_INET6:
	        salen = szof(SOCKADDR_IN6) ;
	        break ;
	    default:
	        rs = SR_NOTSUP ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return (rs >= 0) ? salen : rs ;
}
/* end subroutine (sockaddress_getlen) */

int sockaddress_getaddrlen(SA *sap) noex {
	sa		*extp = (sa *) sap ;
	int		rs = SR_FAULT ;
	int		addrlen = 0 ;
	if (sap) {
	    cint	af = extp->afget() ;
	    rs = SR_OK ;
	    switch (af) {
	    case AF_UNIX:
		{
	            SOCKADDR_UN	*unix_sap = (SOCKADDR_UN *) sap ;
	            {
	                cchar	*ap = unix_sap->sun_path ;
	                addrlen = lenstr(ap) ;
	            }
		} /* end block */
	        break ;
	    case AF_INET4:
	        addrlen = inet4addrlen ;
	        break ;
	    case AF_INET6:
	        addrlen = inet6addrlen ;
	        break ;
	    default:
	        rs = SR_NOTSUP ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return (rs >= 0) ? addrlen : rs ;
}
/* end subroutine (sockaddress_getaddrlen) */

/* write the address family field */
int sockaddress_putaf(SA *sap,int af) noex {
	int		rs = SR_FAULT ;
	if (sap) {
	    constexpr int	sz = szof(sap->a_unspec.sa_family) ;
	    if_constexpr (sz == 1) {
	        uchar	uaf = uchar(af) ;
	        sap->a_unspec.sa_family = uaf ;
	    } else if_constexpr (sz == 2) {
	        ushort	uaf = ushort(af) ;
	        sap->a_unspec.sa_family = htons(uaf) ;
	    } else {
	        sap->a_unspec.sa_family = 0 ;
	    }
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sockaddress_putaf) */

/* put a port number in there */
int sockaddress_putport(SA *sap,int port) noex {
	sa		*extp = (sa *) sap ;
	int		rs = SR_FAULT ;
	ushort		uport = ushort(port) ;
	if (sap) {
	    cint	af = extp->afget() ;
	    rs = SR_OK ;
	    switch (af) {
	    case AF_UNIX:
	        rs = SR_INVALID ;
	        break ;
	    case AF_INET4:
		{
	            SOCKADDR_IN4 *inet4_sap = (SOCKADDR_IN4 *) sap ;
	            inet4_sap->sin_port = htons(uport) ;
		}
	        break ;
	    case AF_INET6:
		{
	            SOCKADDR_IN6 *inet6_sap = (SOCKADDR_IN6 *) sap ;
		    inet6_sap->sin6_port = htons(uport) ;
		}
	        break ;
	    default:
	        rs = SR_NOTSUP ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sockaddress_putport) */

/* put a transport-specific-part address */
int sockaddress_putaddr(SA *sap,cvoid *vaddr) noex {
	sa		*extp = (sa *) sap ;
	int		rs = SR_FAULT ;
	if (sap && vaddr) {
	    cint	af = extp->afget() ;
	    cchar	*addr = charp(vaddr) ;
	    rs = SR_OK ;
	    switch (af) {
	    case AF_UNIX:
		{
	            SOCKADDR_UN *un_sap = (SOCKADDR_UN *) sap ;
		    {
		        char *bp = strnwcpy(un_sap->sun_path,maxpath,addr) ;
		        *bp = '\0' ;
		        rs = intconv(bp - un_sap->sun_path) ;
		    }
	        }
	        break ;
	    case AF_INET4:
		{
	            SOCKADDR_IN4 *inet4_sap = (SOCKADDR_IN4 *) sap ;
	            rs = inet4addrlen ;
	            memcopy(&inet4_sap->sin_addr,addr,rs) ;
		}
	        break ;
	    case AF_INET6:
		{
	            SOCKADDR_IN6 *inet6_sap = (SOCKADDR_IN6 *) sap ;
	            rs = inet6addrlen ;
	            memcopy(&inet6_sap->sin6_addr,addr,rs) ;
		}
	        break ;
	    default:
	        rs = SR_NOTSUP ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sockaddress_putaddr) */


/* local subroutines */

int sa::afget() noex {
    	constexpr int	sz = szof(a_unspec.sa_family) ;
    	int		af = 0 ;
    	if_constexpr (sz == 1) {
	    af = (a_unspec.sa_family & UCHAR_MAX) ;
	} else if_constexpr (sz == 2) {
	    af = ntohs(a_unspec.sa_family) & USHORT_MAX ;
	}
	return af ;
} /* end method (sa::afget) */

int sa::afput(int af) noex {
    	constexpr int	sz = szof(a_unspec.sa_family) ;
    	if_constexpr (sz == 1) {
	    a_unspec.sa_family = uchar(af) ;
	} else if_constexpr (sz == 2) {
	    ushort uaf = ushort(af) ;
	    a_unspec.sa_family = htons(uaf) ;
	}
	return SR_OK ;
} /* end method (sa::afput) */

int sa::portput(int port) noex {
    	constexpr int	sz = szof(a_in4.sin_port) ;
    	if_constexpr (sz == 1) {
	    a_in4.sin_port = uchar(port) ;
	} else if_constexpr (sz == 2) {
	    ushort uport = ushort(port) ;
	    a_in4.sin_port = htons(uport) ;
	}
	return SR_OK ;
} /* end method (sa::portput) */

int sockaddress::start(int af,cvoid *va,int port,uint flow) noex {
	return sockaddress_start(this,af,va,port,flow) ;
}

sockaddress::operator int () noex {
    	return sockaddress_getlen(this) ;
}

static void sockaddress_init(sockaddress *op,const sockaddress *other) noex {
    	cint	msz = SOCKADDRESS_STRSIZE ;
    	memcopy(op->str,other->str,msz) ;
	{
	    op->getaf	(op,sockaddressmem_getaf) ;
	    op->len	(op,sockaddressmem_len) ;
	    op->finish	(op,sockaddressmem_finish) ;
	}
} /* end subroutine (sockaddress_init) */

sockaddress::sockaddress(const sockaddress &o) noex {
    	sockaddress_init(this,&o) ;
} /* end method (sockaddress::ctor) */

sockaddress &sockaddress::operator = (const sockaddress &o) noex {
    	sockaddress_init(this,&o) ;
	return *this ;
} /* end method (sockaddress::assignment) */

sockaddress_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case sockaddressmem_getaf:
	        rs = sockaddress_getaf(op) ;
	        break ;
	    case sockaddressmem_len:
	        rs = sockaddress_getlen(op) ;
	        break ;
	    case sockaddressmem_addrlen:
	        rs = sockaddress_getaddrlen(op) ;
	        break ;
	    case sockaddressmem_finish:
	        rs = sockaddress_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (sockaddress_co::operator) */


