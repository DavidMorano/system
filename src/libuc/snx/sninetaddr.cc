/* sninetaddr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make string version of INET addresses */
/* version %I% last-modified %G% */

#define	CF_CTHEXUC	1		/* use |cthexuc(3uc)| */

/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

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
	<0		error (system-return)

	See-also:
	snfsflags(3uc)
	snopenflags(3uc)
	snpollflags(3uc)
	snxtilook(3uc)
	sninetaddr(3uc)
	snsigabbr(3uc)
	snabbr(3uc)
	snshellunder(3uc)
	snfilemode(3uc)
	sntid(3uc)
	snerrabbr(3uc)
	snrealname(3uc)
	snloadavg(3uc)
	snkeyval(3uc)
	snwvprintf(3uc)
	snwprintf(3uc)
	snkeyval(3uc)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)|+ |max(3c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<uinet.h>		/* |AF_INET4| */
#include	<inetaddr.h>
#include	<sncpyx.h>
#include	<strdcpyx.h>
#include	<cthex.h>
#include	<mkchar.h>
#include	<localmisc.h>		/* |MAX| + |HEXBUFLEN| */

#include	"snx.h"


/* local defines */

#ifndef	INET4ADDRLEN
#define	INET4ADDRLEN		szof(in_addr_t)
#endif

#ifndef	INET6ADDRLEN
#define	INET6ADDRLEN		16
#endif

#ifndef	INETXADDRLEN
#define	INETXADDRLEN		MAX(INET4ADDRLEN,INET6ADDRLEN)
#endif /* INETXADDRLEN */

#ifndef	INET4_ADDRSTRLEN
#define	INET4_ADDRSTRLEN	16
#endif

#ifndef	INET6_ADDRSTRLEN
#define	INET6_ADDRSTRLEN	46	/* Solaris® says this is 46! */
#endif

#ifndef	INETX_ADDRSTRLEN
#define	INETX_ADDRSTRLEN	MAX(INET4_ADDRSTRLEN,INET6_ADDRSTRLEN)
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

constexpr int		diglen = HEXBUFLEN ;

constexpr bool		f_cthexuc = CF_CTHEXUC ;


/* exported variables */


/* exported subroutines */

int sninetaddr(char *dbuf,int dlen,int af,cchar *addr) noex {
	int		rs = SR_FAULT ;
	if (dbuf && addr) ylikely {
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
	if_constexpr (f_cthexuc) {
	    rs = intconv(strdcpy(dbuf,dlen,addr) - dbuf) ;
	} else {
	    rs = sncpy(dbuf,dlen,addr) ;
	}
	return rs ;
}
/* end subroutine (snunix) */

static int sninet4(char *dbuf,int dlen,cchar *addr) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
	if (inetaddr ia ; (rs = ia.start(addr)) >= 0) ylikely {
	    {
	        rs = ia.getdotaddr(dbuf,dlen) ;
		len = rs ;
	    }
	    rs1 = ia.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (inetaddr) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (sninet4) */

static int sninet6(char *dbuf,int dlen,cchar *addr) noex {
	cint		astrlen = INETX_ADDRSTRLEN ;
	int		rs = SR_OK ;
	int		pl = 0 ; /* return-value */
	if ((dlen < 0) || (dlen >= astrlen)) ylikely {
	    cint	n = INET6ADDRLEN ;
	    char	digbuf[diglen + 1] ;
	    for (int i = 0 ; (rs >= 0) && (i < n) ; i += 1) {
		uint	uch = mkchar(addr[i]) ;
		if_constexpr (f_cthexuc) {
	            rs = cthexuc(digbuf,diglen,uch) ; /* cannot fail! */
	            if ((i > 0) && ((i & 1) == 0)) dbuf[pl++] = ':' ;
	            dbuf[pl++] = digbuf[0] ;
	            dbuf[pl++] = digbuf[1] ;
		} else {
	            rs = cthexui(digbuf,diglen,uch) ; /* cannot fail! */
	            if ((i > 0) && ((i & 1) == 0)) dbuf[pl++] = ':' ;
	            dbuf[pl++] = digbuf[6] ;
	            dbuf[pl++] = digbuf[7] ;
		} /* end if_constexpr */
	    } /* end for */
	} else {
	    rs = SR_OVERFLOW ;
	}
	dbuf[pl] = '\0' ;
	return (rs >= 0) ? pl : rs ;
}
/* end subroutine (sninet6) */


