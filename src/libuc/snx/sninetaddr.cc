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
	snxtilook(3uc)
	sninetaddr(3uc)
	snabbrname(3uc)
	snabbrerr(3uc)
	snsigabbr(3uc)
	snshellunder(3uc)
	snfilemode(3uc)
	sntid(3uc)
	snrealname(3uc)
	snloadavg(3uc)
	snkeyval(3uc)
	snwvprintf(3uc)
	snwprintf(3uc)
	snkeyval(3uc)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD |nullptr_t| */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<uinet.h>		/* LIBU |AF_INET4| */
#include	<inetaddr.h>		/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<strdcpyx.h>		/* LIBUC */
#include	<cthex.h>		/* LIBUC */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU |MAX| + |HEXBUFLEN| */

#include	"snx.h"


/* local defines */

#ifndef	CF_CTHEXUC
#define	CF_CTHEXUC		0
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int snunix	(char *,int,cchar *) noex ;
local int sninet4	(char *,int,cchar *) noex ;
local int sninet6	(char *,int,cchar *) noex ;


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
} /* end subroutine (sninetaddr) */


/* local subroutines */

local int snunix(char *dbuf,int dlen,cchar *addr) noex {
	int		rs ;
	if_constexpr (f_cthexuc) {
	    rs = intconv(strdcpy(dbuf,dlen,addr) - dbuf) ;
	} else {
	    rs = sncpy(dbuf,dlen,addr) ;
	}
	return rs ;
} /* end subroutine (snunix) */

local int sninet4(char *dbuf,int dlen,cchar *addr) noex {
    	con inetaddrs	at = inetaddr_str ;
	int		rs ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
	if (inetaddr ia ; (rs = ia.start(at,addr)) >= 0) ylikely {
	    {
	        rs = ia.getdotaddr(dbuf,dlen) ;
		len = rs ;
	    }
	    rs1 = ia.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (inetaddr) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (sninet4) */

local int sninet6(char *dbuf,int dlen,cchar *addr) noex {
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
} /* end subroutine (sninet6) */


