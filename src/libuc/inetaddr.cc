/* inetaddr SUPPORT */
/* lang=C++20 */

/* object to manipulate INET adresses */
/* version %I% last-modified %G% */


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

	This little object allows for some common manipulations on
	INET4 addresses.  I only handle INET4 addresses with this
	object, so it is now a little bit out-of-date with the
	spread of INET6 addresses (now-a-days).  This code is based
	on code I wrote back in the 1990s when INET6 had not yet
	come fully into its own).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<netdb.h>
#include	<usystem.h>
#include	<usupport.h>
#include	<ctdec.h>
#include	<char.h>
#include	<mkchar.h>
#include	<cfnum.h>
#include	<strwcpy.h>
#include	<uinet.h>
#include	<inaddrbad.hh>
#include	<localmisc.h>

#include	"inetaddr.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	getdig(int) noex ;
}


/* local typedefs */


/* local structures */


/* forward references */


/* local variables */

static constexpr int		inet4addrlen = INET4ADDRLEN ;

static constexpr in_addr_t	inaddrbad = mkinaddrbad() ;


/* exported subroutines */

int inetaddr_start(inetaddr *ip,cvoid *addr) noex {
	int		rs = SR_FAULT ;
	if (ip && addr) {
	    rs = SR_OK ;
	    memclear(ip) ;
	}
	return rs ;
}
/* end subroutine (inetaddr_start) */

int inetaddr_startstr(inetaddr *ip,cchar *addrp,int addrl) noex {
	int		rs = SR_FAULT ;
	if (ip && addrp) {
	    cchar	*ap = addrp ;
	    char	abuf[(inet4addrlen* 4) + 1] ;
	    rs = SR_OK ;
	    if (addrl < 0) addrl = strlen(addrp) ;
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
		    in_addr_t	a ;
	            if (ap != addrp) {
	                strwcpy(abuf,ap,addrl) ;
	                ap = abuf ;
	            }
		    if ((a = inet_addr(ap)) != inaddrbad) {
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
}
/* end subroutine (inetaddr_startstr) */

int inetaddr_startdot(inetaddr *ip,cchar *addrp,int addrl) noex {
	int		rs = SR_FAULT ;
	if (ip && addrp) {
	    cchar	*ap = addrp ;
	    char	abuf[(inet4addrlen* 4) + 1] ;
	    rs = SR_OK ;
	    if (addrl < 0) addrl = strlen(addrp) ;
	    while (CHAR_ISWHITE(*ap)) {
	        ap += 1 ;
	        addrl -= 1 ;
	    }
	    while ((addrl > 0) && CHAR_ISWHITE(ap[addrl-1])) {
	        addrl -= 1 ;
	    }
	    if (addrl > 0) {
		in_addr_t	a ;
	        if (ap != addrp) {
	            strwcpy(abuf,ap,addrl) ;
	            ap = abuf ;
	        }
		if ((a = inet_addr(ap)) != inaddrbad) {
	            ip->a.s_addr = a ;
		} else {
	            rs = SR_INVALID ;
	        }
	    } else {
	        rs = SR_INVALID ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (inetaddr_startdot) */

int inetaddr_finish(inetaddr *ip) noex {
	int		rs = SR_FAULT ;
	if (ip) {
	    rs = SR_OK ;
	    ip->a = {} ;
	}
	return rs ;
}
/* end subroutine (inetaddr_finish) */

int inetaddr_gethexaddr(inetaddr *ip,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		j = 0 ;
	if (ip && rbuf) {
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
	if (ip && rbuf) {
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
	    rl = (bp - rbuf) ;
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (inetaddr_getdotaddr) */


