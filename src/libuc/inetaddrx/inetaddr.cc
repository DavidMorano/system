/* inetaddr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* object to manipulate INET4 adresses */
/* version %I% last-modified %G% */

#define	CF_STARTSTR	0		/* start from a number-string */
#define	CF_STARTDOT	0		/* start from a dot-decimal string */

/* revision history:

	= 1998-02-14, David A­D­ Morano
	This little object module was first written.

	= 2020-05-17, David A­D­ Morano
	I added the |consteval| from C++20 on my little value
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
	int inetaddr_start(inetaddr *ip,cvoid *addr) noex
	int inetaddr_startstr(inetaddr *ip,cchar *addrp,int addrl) noex
	int inetaddr_startdot(inetaddr *ip,cchar *addrp,int addrl) noex

	Arguments:
	ip		object pointer
	addr		INADDR pointer
	addrp		c-string address pointer
	addrl		c-string address (raw-bytes) length
	addrl		c-string address (dot-decimal) length

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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |memcpy(3c)| */
#include	<netdb.h>
#include	<usystem.h>
#include	<uinet.h>
#include	<strwcpy.h>
#include	<inaddrbad.hh>
#include	<char.h>
#include	<mkchar.h>
#include	<localmisc.h>

#if	defined(CF_STARTSTR) && (CF_STARTSTR > 0)
#include	<cfnum.h>
#endif

#include	"inetaddr.h"

import libutil ;
import digtab ;

/* local defines */


/* imported namespaces */

using libu::ctdecui ;			/* subroutine (LIBU) */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr int		inet4addrlen = INET4ADDRLEN ;
constexpr int		alen = (inet4addrlen * 4) ;

constexpr in_addr_t	inaddrbad = mkinaddrbad() ;


/* exported variables */


/* exported subroutines */

int inetaddr_start(inetaddr *ip,cvoid *addr) noex {
	int		rs = SR_FAULT ;
	if (ip && addr) ylikely {
	    char *ep = charp(memcopy(ip->straddr,addr,inet4addrlen)) ;
	    rs = intconv(ep - ip->straddr) ;
	}
	return rs ;
}
/* end subroutine (inetaddr_start) */

#if	defined(CF_STARTSTR) && (CF_STARTSTR > 0)
int inetaddr_startstr(inetaddr *ip,cchar *addrp,int addrl) noex {
	int		rs = SR_FAULT ;
	if (ip && addrp) ylikely {
	    cchar	*ap = addrp ;
	    char	abuf[alen + 1] ;
	    rs = SR_OK ;
	    if (addrl < 0) addrl = lenstr(addrp) ;
	    while (CHAR_ISWHITE(*ap)) {
	        ap += 1 ;
	        addrl -= 1 ;
	    }
	    if (*ap == '\\') {
	        uint	uiw ;
	        rs = cfnumui(ap,addrl,&uiw) ;
	        ip->a.s_addr = htonl(uiw) ;
	    } else {
	        while ((addrl > 0) && CHAR_ISWHITE(ap[addrl - 1])) {
	            addrl -= 1 ;
	        }
	        if (addrl > 0) {
	            if (ap != addrp) {
	                strwcpy(abuf,ap,addrl) ;
	                ap = abuf ;
	            }
		    if (in_addr_t a ; (a = inet_addr(ap)) != inaddrbad) {
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
#endif /* CF_STARTSTR */

#if	defined(CF_STARTDOT) && (CF_STARTDOT > 0)
int inetaddr_startdot(inetaddr *ip,cchar *addrp,int addrl) noex {
	int		rs = SR_FAULT ;
	if (ip && addrp) ylikely {
	    cchar	*ap = addrp ;
	    char	abuf[(inet4addrlen* 4) + 1] ;
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
		if (in_addr_t a ; (a = inet_addr(ap)) != inaddrbad) {
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
#endif /* CF_STARTDOT */

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
	    constexpr int	rlenmin = ((inet4addrlen* 2) + 0) ;
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
	    constexpr int	rlenmin = ((inet4addrlen * 3) + 3) ;
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

int inetaddr::start(cvoid *addr) noex {
	return inetaddr_start(this,addr) ;
}

#if	defined(CF_STARTSTR) && (CF_STARTSTR > 0)
int inetaddr::startstr(cchar *sp,int sl) noex {
	return inetaddr_startstr(this,sp,sl) ;
}
#endif

#if	defined(CF_STARTDOT) && (CF_STARTDOT > 0)
int inetaddr::startdot(cchar *sp,int sl) noex {
	return inetaddr_startdot(this,sp,sl) ;
}
#endif

int inetaddr::gethexaddr(char *bp,int bl) noex {
	return inetaddr_gethexaddr(this,bp,bl) ;
}

int inetaddr::getdotaddr(char *bp,int bl) noex {
	return inetaddr_getdotaddr(this,bp,bl) ;
}


