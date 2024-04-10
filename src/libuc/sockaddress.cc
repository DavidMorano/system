/* sockaddress SUPPORT */
/* lang=C++20 */

/* manipulate socket addresses */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-13, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This little object allows for some basic manipulations of
	socket addresses.

	Important notes:
	The length of a UNIX® domain (AF_UNIX) socket address
	includes both the two bytes of the address family field (as
	everyone would expect) but it also includes the trailing
	NUL byte at the end of the filepath string.


*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<netinet/in.h>
#include	<climits>
#include	<cstring>		/* for |strlen(3c)| */
#include	<algorithm>		/* for |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<snwcpy.h>
#include	<strn.h>
#include	<getdig.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"sockaddress.h"


/* local defines */

#define	SA	SOCKADDRESS


/* imported namespaces */

using std::min ;
using std::max ;


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */

constexpr int		inet4addrlen = int(INET4ADDRLEN) ;
constexpr int		inet6addrlen = int(INET6ADDRLEN) ;
constexpr int		maxpath = SOCKADDRESS_NAMELEN ;


/* exported variables */


/* exported subroutines */

int sockaddress_start(SA *sap,int af,cvoid *vaddr,int port,uint flow) noex {
	return sockaddress_startaddr(sap,af,vaddr,-1,port,flow) ;
}
/* end subroutine (sockaddress_start) */

int sockaddress_startaddr(SA *sap,int af,cvoid *vaddr,int alen,
		int port,uint flow) noex {
	int		rs = SR_FAULT ;
	int		salen = 0 ;
	if (sap && vaddr) {
	    SOCKADDR		*x_sap ;
	    SOCKADDR_IN4	*inet4_sap ;
	    SOCKADDR_IN6	*inet6_sap ;
	    int		ml ;
	    cchar	*addr = charp(vaddr) ;
	    rs = SR_OK ;
	    switch (af) {
	    case AF_UNIX:
	        {
		    cint	plen = maxpath ;
		    char	*bp ;
	            x_sap = (SOCKADDR *) sap ;
	            sap->a_unspec.sa_family = (ushort) htons(af) ;
	            bp = strnwcpy(x_sap->sa_data,plen,addr,alen) ;
		    *bp = '\0' ;
	            salen = (sizeof(ushort) + (bp - x_sap->sa_data)) ;
	        }
	        break ;
	    case AF_INET4:
	        inet4_sap = (SOCKADDR_IN4 *) sap ;
	        salen = sizeof(SOCKADDR_IN4) ;
	        memset(inet4_sap,0,salen) ;
	        inet4_sap->sin_family = (ushort) htons(af) ;
	        inet4_sap->sin_port = (ushort) htons(port) ;
	        if (addr != NULL) {
	            ml = (alen >= 0) ? alen : sizeof(struct in_addr) ;
	            memcpy(&inet4_sap->sin_addr,addr,ml) ;
	        }
	        break ;
	    case AF_INET6:
	        inet6_sap = (SOCKADDR_IN6 *) sap ;
	        salen = sizeof(SOCKADDR_IN6) ;
	        memset(inet6_sap,0,salen) ;
	        inet6_sap->sin6_family = (ushort) htons(af) ;
	        inet6_sap->sin6_port = (ushort) htons(port) ;
	        inet6_sap->sin6_flowinfo = (uint) htonl(flow) ;
	        if (addr != NULL) {
	            ml = (alen >= 0) ? alen : INET6ADDRLEN ;
	            memcpy(&inet6_sap->sin6_addr,addr,ml) ;
	        }
	        break ;
	    default:
	        sap->a_unspec.sa_family = 0 ;
	        rs = SR_INVALID ;
	    } /* end switch */
	} /* end if (non-null) */
	return (rs >= 0) ? salen : rs ;
}
/* end subroutine (sockaddress_startaddr) */

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
	int		rs = SR_FAULT ;
	if (sap) {
	    rs = ntohs(sap->a_unspec.sa_family) & USHORT_MAX ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sockaddress_getaf) */

/* get the port for this sockaddress (based on address family type) */
int sockaddress_getport(SA *sap) noex {
	int		rs = SR_FAULT ;
	int		port = 0 ;
	if (sap) {
	    SOCKADDR_IN4	*inet4_sap ;
	    SOCKADDR_IN6	*inet6_sap ;
	    cint	af = ntohs(sap->a_unspec.sa_family) & USHORT_MAX ;
	    rs = SR_OK ;
	    switch (af) {
	    case AF_INET4:
	        inet4_sap = (SOCKADDR_IN4 *) sap ;
	        port = ntohs(inet4_sap->sin_port) ;
	        break ;
	    case AF_INET6:
	        inet6_sap = (SOCKADDR_IN6 *) sap ;
	        port = ntohs(inet6_sap->sin6_port) ;
	        break ;
	    default:
	        rs = SR_INVALID ;
	        break ;
	    } /* end switch */
	    port &= USHORT_MAX ;
	} /* end if (non-null) */
	return (rs >= 0) ? port : rs ;
}
/* end subroutine (sockaddress_getport) */

int sockaddress_getflow(SA *sap,uint *rp) noex {
	int		rs = SR_FAULT ;
	if (sap) {
	    SOCKADDR_IN6	*inet6_sap ;
	    uint		flow = 0 ;
	    cint		af = ntohs(sap->a_unspec.sa_family) ;
	    switch (af) {
	    case AF_INET4:
	        rs = SR_INVALID ;
	        break ;
	    case AF_INET6:
	        inet6_sap = (SOCKADDR_IN6 *) sap ;
	        flow = ntohs(inet6_sap->sin6_flowinfo) ;
	        break ;
	    default:
	        rs = SR_INVALID ;
	        break ;
	    } /* end switch */
	    if (rp) *rp = flow ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sockaddress_getflow) */

/* get the address for this sockaddress (based on address type) */
int sockaddress_getaddr(SA *sap,char *abuf,int alen) noex {
	int		rs = SR_FAULT ;
	if (sap && abuf) {
	    SOCKADDR_UN		*unix_sap ;
	    SOCKADDR_IN4	*inet4_sap ;
	    SOCKADDR_IN6	*inet6_sap ;
	    cint		af = ntohs(sap->a_unspec.sa_family) ;
	    cchar		*sp ;
	    switch (af) {
	    case AF_UNIX:
	        unix_sap = (SOCKADDR_UN *) sap ;
	        sp = unix_sap->sun_path ;
	        if (alen < 0) alen = maxpath ;
	        rs = snwcpy(abuf,alen,sp,maxpath) ;
	        break ;
	    case AF_INET4:
	        inet4_sap = (SOCKADDR_IN4 *) sap ;
	        if (alen >= inet4addrlen) {
	            rs = inet4addrlen ;
	            memcpy(abuf,&inet4_sap->sin_addr,inet4addrlen) ;
	        } else {
	            rs = SR_TOOBIG ;
	        }
	        break ;
	    case AF_INET6:
	        inet6_sap = (SOCKADDR_IN6 *) sap ;
	        if (alen >= inet4addrlen) {
	            rs = inet6addrlen ;
	            memcpy(abuf,&inet6_sap->sin6_addr,inet6addrlen) ;
	        } else {
	            rs = SR_TOOBIG ;
	        }
	        break ;
	    default:
	        rs = SR_INVALID ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sockaddress_getaddr) */

int sockaddress_getscope(SA *sap,uint *rp) noex {
	int		rs = SR_FAULT ;
	if (sap) {
	    SOCKADDR_IN6	*inet6_sap ;
	    uint		v = 0 ;
	    cint		af = ntohs(sap->a_unspec.sa_family) ;
	    rs = SR_OK ;
	    switch (af) {
	    case AF_INET4:
	        rs = SR_INVALID ;
	        break ;
	    case AF_INET6:
	        inet6_sap = (SOCKADDR_IN6 *) sap ;
	        v = ntohs(inet6_sap->sin6_scope_id) ;
	        break ;
	    default:
	        rs = SR_INVALID ;
	        break ;
	    } /* end switch */
	    if (rp) *rp = v ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sockaddress_getscope) */

int sockaddress_getextra(SA *sap,uint *rp) noex {
	int		rs = SR_FAULT ;
	if (sap) {
	    SOCKADDR_IN6	*inet6_sap ;
	    uint		v = 0 ;
	    cint		af = ntohs(sap->a_unspec.sa_family) ;
	    switch (af) {
	    case AF_INET4:
	        rs = SR_INVALID ;
	        break ;
	    case AF_INET6:
	        {
		    uint	*ip ;
	            inet6_sap = (SOCKADDR_IN6 *) sap ;
		    ip = &inet6_sap->sin6_scope_id ;
		    ip += sizeof(uint) ;
	            v = ntohs(*ip) ;
	        }
	        break ;
	    default:
	        rs = SR_INVALID ;
	        break ;
	    } /* end switch */
	    if (rp) *rp = v ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sockaddress_getextra) */

/* get the entire socket address (the TLI address) in HEXADECIMAL */
int sockaddress_gethex(SA *sap,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		j = 0 ;
	if (sap && rbuf) {
	    cint	af = ntohs(sap->a_unspec.sa_family) ;
	    int		v ;
	    int		salen = 0 ;
	    rs = SR_OK ;
	    switch (af) {
	    case AF_UNIX:
	        {
	            char	*ap = charp(sap) ;
	            salen = MIN((strlen(ap + 2) + 2),sizeof(SOCKADDRESS)) ;
	        }
	        break ;
	    case AF_INET4:
	        salen = sizeof(SOCKADDR_IN4) ;
	        break ;
	    case AF_INET6:
	        salen = sizeof(SOCKADDR_IN6) ;
	        break ;
	    default:
	        rs = SR_NOTFOUND ;
	        break ;
	    } /* end switch */
	    if ((rs >= 0) && (rlen >= 0) && (rlen < ((salen * 2) + 1))) {
	        rs = SR_TOOBIG ;
	    }
	    if (rs >= 0) {
	        int	i ; /* used-afterwards */
	        for (i = 0 ; i < salen ; i += 1) {
	            v = mkchar(sap->str[i]) ;
	            rbuf[j++] = getdig((v >> 4) & 15) ;
	            rbuf[j++] = getdig((v >> 0) & 15) ;
	        } /* end for */
	        rbuf[j++] = '\0' ;
	    } /* end if (ok) */
	} /* end if (non-null) */
	return (rs >= 0) ? j : rs ;
}
/* end subroutine (sockaddress_gethex) */

/* get the entire socket address length (like for a TPI address) */
int sockaddress_getlen(SA *sap) noex {
	int		rs = SR_FAULT ;
	int		salen = 0 ;
	if (sap) {
	    cint	af = ntohs(sap->a_unspec.sa_family) ;
	    rs = SR_OK ;
	    switch (af) {
	    case AF_UNIX:
	        {
	            cchar	*ap = charp(sap) ;
	            salen = MIN((strlen(ap+2) + 3),sizeof(SA)) ;
	        }
	        break ;
	    case AF_INET4:
	        salen = sizeof(SOCKADDR_IN4) ;
	        break ;
	    case AF_INET6:
	        salen = sizeof(SOCKADDR_IN6) ;
	        break ;
	    default:
	        rs = SR_NOTFOUND ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return (rs >= 0) ? salen : rs ;
}
/* end subroutine (sockaddress_getlen) */

int sockaddress_getaddrlen(SA *sap) noex {
	int		rs = SR_FAULT ;
	int		addrlen = 0 ;
	if (sap) {
	    cint	af = ntohs(sap->a_unspec.sa_family) ;
	    rs = SR_OK ;
	    switch (af) {
	    case AF_UNIX:
	        {
	            char	*ap = charp(sap) ;
	            addrlen = (strlen(ap + 2)) ;
	        }
	        break ;
	    case AF_INET4:
	        addrlen = inet4addrlen ;
	        break ;
	    case AF_INET6:
	        addrlen = inet6addrlen ;
	        break ;
	    default:
	        rs = SR_NOTFOUND ;
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
	    sap->a_unspec.sa_family = htons(af) ;
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sockaddress_putaf) */

/* put a port number in there */
int sockaddress_putport(SA *sap,int port) noex {
	int		rs = SR_FAULT ;
	if (sap) {
	    SOCKADDR_IN4	*inet4_sap ;
	    SOCKADDR_IN6	*inet6_sap ;
	    cint		af = ntohs(sap->a_unspec.sa_family) ;
	    rs = SR_OK ;
	    switch (af) {
	    case AF_UNIX:
	        rs = SR_INVALID ;
	        break ;
	    case AF_INET4:
	        inet4_sap = (SOCKADDR_IN4 *) sap ;
	        inet4_sap->sin_port = htons(port) ;
	        break ;
	    case AF_INET6:
	        inet6_sap = (SOCKADDR_IN6 *) sap ;
	        inet6_sap->sin6_port = htons(port) ;
	        break ;
	    default:
	        rs = SR_NOTFOUND ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sockaddress_putport) */

/* put a transport-specific-part address */
int sockaddress_putaddr(SA *sap,cvoid *vaddr) noex {
	int		rs = SR_FAULT ;
	if (sap && vaddr) {
	    SOCKADDR		*x_sap ;
	    SOCKADDR_IN4	*inet4_sap ;
	    SOCKADDR_IN6	*inet6_sap ;
	    cint		af = ntohs(sap->a_unspec.sa_family) ;
	    cchar		*addr = charp(vaddr) ;
	    rs = SR_OK ;
	    switch (af) {
	    case AF_UNIX:
	        x_sap = (SOCKADDR *) sap ;
	        {
	            cint	plen = maxpath ;
		    char	*bp ;
	            bp = strnwcpy(x_sap->sa_data,plen,addr,-1) ;
		    *bp = '\0' ;
		    rs = (bp - x_sap->sa_data) ;
	        }
	        break ;
	    case AF_INET4:
	        inet4_sap = (SOCKADDR_IN4 *) sap ;
	        rs = inet4addrlen ;
	        memcpy(&inet4_sap->sin_addr,addr,rs) ;
	        break ;
	    case AF_INET6:
	        inet6_sap = (SOCKADDR_IN6 *) sap ;
	        rs = inet6addrlen ;
	        memcpy(&inet6_sap->sin6_addr,addr,rs) ;
	        break ;
	    default:
	        rs = SR_NOTFOUND ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sockaddress_putaddr) */


