/* inetaddr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* object to manipulate INET4 adresses */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-14, David A­D­ Morano
	This little object module was first written.

	= 2020-05-17, David A­D­ Morano
	I added the |constexpr| from C++20 on my little value
	generator, and |constexpr| on the static variables.  Is
	everyone doing this sort of maintenance on their old code
	now (to be "hip" with the times)? Just wondering.  Correct,
	this did not add a lot of value to the code. As a small
	side note: This code is pretty much obsolete now, so this
	small change was mostly just in vain (I guess).

*/

/* Copyright © 1998,2020 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	inetaddr

	Description:
	This little object allows for some common manipulations on
	INET4 addresses.  I only handle INET4 addresses with this
	object, so it is now a little bit out-of-date with the
	spread of INET6 addresses (now-a-days).  This code is based
	on code I wrote back in the 1990s when INET6 had not yet
	come fully into its own).

	Synopsis:
	int inetaddr_start(inetaddr *ip,inetaddrs at,cvoid *ap,int al) noex

	Arguments:
	ip		object pointer
	at		address-type
	ap		c-string address pointer
	al		c-string address length

	Return:
	>=0		OK
	<0		error (system-return)

	Notes:
	1. I am using the (semi-secret) hack from the LIBU library
	of the |ctdecu{x}(3u)| subroutine.  I am doing this in order
	to avoid excessive dependency on the LIBUC facility |ctx(3uc)|.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>		/* |inet_addr(3c)| + |in_addr_t| */
#include	<netdb.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usupport.h>		/* |cfdec(3u)| */
#include	<uinet.h>
#include	<strwcpy.h>
#include	<inaddrbad.hh>
#include	<cfnum.h>
#include	<mkchar.h>
#include	<char.h>
#include	<localmisc.h>

#include	"inetaddr.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/digtab.ccm"

import libutil ;			/* |lenstr(3u)| */
import digtab ;

/* local defines */


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using libu::ctdecui ;			/* subroutine (LIBU) */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int inetaddr_startbin(inetaddr *ip,cchar *addrp,int addrl) noex ;
local int inetaddr_startstr(inetaddr *ip,cchar *addrp,int addrl) noex ;
local int inetaddr_startdot(inetaddr *ip,cchar *addrp,int addrl) noex ;


/* local variables */

constexpr int		inet4addrlen = INET4ADDRLEN ;
constexpr int		inet6addrlen = INET6ADDRLEN ;

constexpr int		str4len = (INET4ADDRLEN * 4) ;
constexpr int		str6len = (INET6ADDRLEN * 20) ;

constexpr in_addr_t	inaddrbad = mkinaddrbad() ;


/* exported variables */


/* exported subroutines */

int inetaddr_start(inetaddr *ip,inetaddrs at,cvoid *abuf,int alen) noex {
	int		rs = SR_FAULT ;
	if (ip && abuf) ylikely {
	    cchar *abufs = charp(abuf) ;
	    rs = SR_INVALID ;
	    if ((at >= 0) && (at < inetaddr_overlast)) {
		switch (at) {
		case inetaddr_bin:
		    rs = inetaddr_startbin(ip,abufs,alen) ;
		    break ;
		case inetaddr_str:
		    rs = inetaddr_startstr(ip,abufs,alen) ;
		    break ;
		case inetaddr_dot:
		    rs = inetaddr_startdot(ip,abufs,alen) ;
		    break ;
		default:
		    rs = SR_INVALID ;
		    break ;
		} /* end switch */
	    } /* end if (valid) */
	} /* end if (non-null) */
	(void) alen ;
	return rs ;
}
/* end subroutine (inetaddr_start) */

local int inetaddr_startbin(inetaddr *ip,cchar *addrp,int addrl) noex {
    	int		rs = SR_FAULT ;
	if (ip) {
	    cint ml = (addrl >= 0) ? min(addrl,inet4addrlen) : inet4addrlen ;
	    char *ep = charp(memcopy(ip->straddr,addrp,ml)) ;
	    rs = intconv(ep - ip->straddr) ;
	}
	return rs ;
} /* end subroutine (inetaddr_startbin) */

local int inetaddr_startstr(inetaddr *ip,cchar *addrp,int addrl) noex {
	int		rs = SR_FAULT ;
	if (ip && addrp) ylikely {
	    cchar	*ap = addrp ;
	    char	abuf[str4len + 1] ;
	    rs = SR_OK ;
	    if (addrl < 0) addrl = lenstr(addrp) ;
	    while (CHAR_ISWHITE(*ap)) {
	        ap += 1 ;
	        addrl -= 1 ;
	    }
	    if (*ap == '\\') {
	        if (uint uiw ; (rs = cfnumui(ap,addrl,&uiw)) >= 0) {
	            ip->a.s_addr = htonl(uiw) ;
		}
	    } else {
	        while ((addrl > 0) && CHAR_ISWHITE(ap[addrl - 1])) {
	            addrl -= 1 ;
	        }
	        if (addrl > 0) {
	            if (ap != addrp) {
	                strwcpy(abuf,ap,addrl) ;
	                ap = abuf ;
	            }
		    if (in4_addr_t a ; (a = inet_addr(ap)) != inaddrbad) {
	                ip->a.s_addr = a ;
		    } else {
	                rs = SR_INVALID ;
		    }
	        } else {
	            rs = SR_INVALID ;
	        }
	    } /* end if */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (inetaddr_startstr) */

local int inetaddr_startdot(inetaddr *ip,cchar *addrp,int addrl) noex {
	int		rs = SR_FAULT ;
	if (ip && addrp) ylikely {
	    cchar	*ap = addrp ;
	    char	abuf[(inet4addrlen * szof(in4_addr_t)) + 1] ;
	    rs = SR_OK ;
	    if (addrl < 0) addrl = lenstr(addrp) ;
	    while (CHAR_ISWHITE(*ap)) {
	        ap += 1 ;
	        addrl -= 1 ;
	    }
	    while ((addrl > 0) && CHAR_ISWHITE(ap[addrl-1])) {
	        addrl -= 1 ;
	    }
	    if (addrl > 0) {
	        if (ap != addrp) {
	            strwcpy(abuf,ap,addrl) ;
	            ap = abuf ;
	        }
		if (in4_addr_t a ; (a = inet_addr(ap)) != inaddrbad) {
	            ip->a.s_addr = a ;
		} else {
	            rs = SR_INVALID ;
	        }
	    } else {
	        rs = SR_INVALID ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (inetaddr_startdot) */

int inetaddr_finish(inetaddr *ip) noex {
	int		rs = SR_FAULT ;
	if (ip) ylikely {
	    rs = SR_OK ;
	    ip->a = {} ;
	}
	return rs ;
}
/* end subroutine (inetaddr_finish) */

int inetaddr_gethexaddr(inetaddr *ip,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		j = 0 ;
	if (ip && rbuf) ylikely {
	    cint	rlenmin = ((inet4addrlen * 2) + 0) ;
	    rs = SR_OK ;
	    rbuf[0] = '\0' ;
	    if ((rlen < 0) || (rlen >= rlenmin)) {
	        for (int i = 0 ; i < inet4addrlen ; i += 1) {
	            cuint	v = mkchar(ip->straddr[i]) ;
	            rbuf[j++] = getdig((v >> 4) & 0xf) ;
	            rbuf[j++] = getdig((v >> 0) & 0xf) ;
	        } /* end for */
	    } else {
	        rs = SR_OVERFLOW ;
	    }
	    rbuf[j] = '\0' ;
	} /* end if (non-null) */
	return (rs >= 0) ? j : rs ;
}
/* end subroutine (inetaddr_gethexaddr) */

int inetaddr_getdotaddr(inetaddr *ip,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ;
	if (ip && rbuf) ylikely {
	    cint	rlenmin = ((inet4addrlen * 3) + 3) ;
	    char	*bp = rbuf ;
	    rbuf[0] = '\0' ;
	    rs = SR_OK ;
	    if ((rlen < 0) || (rlen >= rlenmin)) {
	        uint	v ;
	        for (int i = 0 ; i < inet4addrlen ; i += 1) {
	            if (i > 0) *bp++ = '.' ;
	            v = mkchar(ip->straddr[i]) ;
	            rs = ctdecui(bp,3,v) ;
	            bp += rs ;
	            if (rs < 0) break ;
	        } /* end for */
	    } else {
	        rs = SR_OVERFLOW ;
	    }
	    *bp = '\0' ;
	    rl = intconv(bp - rbuf) ;
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (inetaddr_getdotaddr) */

inetaddr_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case inetaddrmem_finish:
	        rs = inetaddr_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (inetaddr_co::operator) */

int inetaddr::start(inetaddrs at,cvoid *addr,int alen) noex {
	return inetaddr_start(this,at,addr,alen) ;
}

int inetaddr::gethexaddr(char *bp,int bl) noex {
	return inetaddr_gethexaddr(this,bp,bl) ;
}

int inetaddr::getdotaddr(char *bp,int bl) noex {
	return inetaddr_getdotaddr(this,bp,bl) ;
}


