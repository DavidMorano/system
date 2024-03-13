/* sninetaddr SUPPORT */
/* lang=C++20 */

/* make string version of INET addresses */
/* version %I% last-modified %G% */

#define	CF_CTHEXUC	1		/* use |cthexuc(3dam)| */

/* revision history:

	= 1998-11-01, David A�D� Morano
	This subroutine was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sninetaddr

	Description:
	Create and copy a string repreentation of an Internet address
	(either IP4 or IP5) into the destination.

	Synopsis:
	int sninetaddr(char *dbuf,int dlen,int af,char *addr) noex
	
	Arguments:
	dbuf		destination string buffer
	dlen		destination string buffer length
	af		address-family
	addr		address buffer

	Returns:
	>=0		number of bytes in result
	<0		error

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<netinet/in.h>
#include	<string.h>
#include	<usystem.h>
#include	<inetaddr.h>
#include	<sncpyx.h>
#include	<strdcpyx.h>
#include	<cthex.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"snx.h"


/* local defines */

#ifndef	INET4ADDRLEN
#define	INET4ADDRLEN	sizeof(in_addr_t)
#endif

#ifndef	INET6ADDRLEN
#define	INET6ADDRLEN	16
#endif

#ifndef	INETXADDRLEN
#define	INETXADDRLEN	MAX(INET4ADDRLEN,INET6ADDRLEN)
#endif /* INETXADDRLEN */

#ifndef	INET4_ADDRSTRLEN
#define	INET4_ADDRSTRLEN	16
#endif

#ifndef	INET6_ADDRSTRLEN
#define	INET6_ADDRSTRLEN	46	/* Solaris� says this is 46! */
#endif

#ifndef	INETX_ADDRSTRLEN
#define	INETX_ADDRSTRLEN	MAX(INET4_ADDRSTRLEN,INET6_ADDRSTRLEN)
#endif

#ifndef	DIGBUFLEN
#define	DIGBUFLEN		10
#endif

#ifndef	CF_CTHEXUC
#define	CF_CTHEXUC		0
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int snunix(char *,int,cchar *) noex ;
static int sninet4(char *,int,cchar *) noex ;
static int sninet6(char *,int,cchar *) noex ;


/* local variables */

constexpr bool		f_cthexuc = CF_CTHEXUC ;


/* exported variables */


/* exported subroutines */

int sninetaddr(char *dbuf,int dlen,int af,cchar *addr) noex {
	int		rs = SR_FAULT ;
	if (dbuf && addr) {
	    switch (af) {
	    case AF_UNIX:
	        rs = snunix(dbuf,dlen,addr) ;
	        break ;
	    case AF_INET4:
	        rs = sninet4(dbuf,dlen,addr) ;
	        break ;
	    case AF_INET6:
	        rs = sninet6(dbuf,dlen,addr) ;
	        break ;
	    default:
	        rs = SR_AFNOSUPPORT ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sninetaddr) */


/* local subroutines */

static int snunix(char *dbuf,int dlen,cchar *addr) noex {
	int		rs ;
	if constexpr (f_cthexuc) {
	    rs = strdcpy1(dbuf,dlen,addr) - dbuf ;
	} else {
	    rs = sncpy1(dbuf,dlen,addr) ;
	}
	return rs ;
}
/* end subroutine (snunix) */

static int sninet4(char *dbuf,int dlen,cchar *addr) noex {
	INETADDR	ia ;
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if ((rs = inetaddr_start(&ia,addr)) >= 0) {
	    {
	        rs = inetaddr_getdotaddr(&ia,dbuf,dlen) ;
		len = rs ;
	    }
	    rs1 = inetaddr_finish(&ia) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (inetaddr) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (sninet4) */

static int sninet6(char *dbuf,int dlen,cchar *addr) noex {
	cint		astrlen = INETX_ADDRSTRLEN ;
	int		rs = SR_OK ;
	int		pl = 0 ;
	if ((dlen < 0) || (dlen >= astrlen)) {
	    uint	uch ;
	    cint	diglen = DIGBUFLEN ;
	    cint	n = INET6ADDRLEN ;
	    char	digbuf[DIGBUFLEN+1] ;
	    for (int i = 0 ; (rs >= 0) && (i < n) ; i += 1) {
		uch = mkchar(addr[i]) ;
		if constexpr (f_cthexuc) {
	            rs = cthexuc(digbuf,diglen,uch) ; /* cannot fail! */
	            if ((i > 0) && ((i & 1) == 0)) dbuf[pl++] = ':' ;
	            dbuf[pl++] = digbuf[0] ;
	            dbuf[pl++] = digbuf[1] ;
		} else {
	            rs = cthexui(digbuf,diglen,uch) ; /* cannot fail! */
	            if ((i > 0) && ((i & 1) == 0)) dbuf[pl++] = ':' ;
	            dbuf[pl++] = digbuf[6] ;
	            dbuf[pl++] = digbuf[7] ;
		} /* end if-constexpr */
	    } /* end for */
	} else {
	    rs = SR_OVERFLOW ;
	}
	dbuf[pl] = '\0' ;
	return (rs >= 0) ? pl : rs ;
}
/* end subroutine (sninet6) */


