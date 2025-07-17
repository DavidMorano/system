/* nlspeername SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* try to find the peername for an NLS (XTI) connection */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	nlspeername

	Description:
	This subrotuine tries to find the peername for the given
	NLS (TLI) address.

	Synopsis:
	int nlspeername(cchar *addr,cchar *dn,char *pn) noex

	Arguments:
	addr		address (in ASCII hexadecimal) of peer
	dn		INET domain-name
	pn		result buffer pointer

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<netdb.h>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<bfile.h>
#include	<field.h>
#include	<hostent.h>
#include	<gethe.h>
#include	<sockaddress.h>
#include	<inetaddr.h>
#include	<strwcpy.h>
#include	<cfhex.h>
#include	<cfhexstr.h>
#include	<isindomain.h>
#include	<localmisc.h>

#include	"nlspeername.h"

import libutil ;

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	int		maxhostlen ;
	operator int () noex ;
    } ;
} /* end namespace */


/* forward references */

static int	nlspeername_unix(char *,cchar *,cchar *,int) noex ;
static int	nlspeername_inet4(char *,cchar *,cchar *,int) noex ;


/* local variables */

static vars	var ;


/* exported variables */


/* exported subroutines */

int nlspeername(cchar *addr,cchar *dn,char *pn) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (addr && pn) {
	    cint	al = lenstr(addr) ;
	    rs = SR_INVALID ;
	    pn[0] = '\0' ;
	    if (al >= 16) {
		static cint	rsv = var ;
		if ((rs = rsv) >= 0) {
	            if (int af ; (rs = cfhexi(addr,4,&af)) >= 0) {
			switch (af) {
	                case AF_UNIX:
	                    rs = nlspeername_unix(pn,dn,addr,al) ;
			    len = rs ;
			    break ;
			case AF_INET4:
	                    rs = nlspeername_inet4(pn,dn,addr,al) ;
			    len = rs ;
			    break ;
	                } /* end switch (recognized the address family) */
	            } /* end if (cfhexi) */
		} /* end if (vars) */
	    } /* end if (good) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (nlspeername) */


/* local subroutines */

static int nlspeername_unix(char *pn,cchar *dn,cchar *addr,int al) noex {
	cint		nlen = var.maxhostlen ;
	cint		adv = 4 ;
	int		rs = SR_OK ;
	int		len = 0 ;
	(void) dn ;
	al -= adv ;
	if ((al / 2) <= nlen) {
	    uchar	*upn = ucharp(pn) ;
	    rs = cfhexstr((addr + adv),al,upn) ;
	    len = rs ;
	} else {
	    rs = SR_TOOBIG ;
	}
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (nlspeername_unix) */

static int nlspeername_inet4(char *pn,cchar *dn,cchar *ap,int al) noex {
	uint		uv ;
	cint		adv = 8 ;
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	(void) al ;
	if ((rs = cfhexui((ap + adv),8,&uv)) >= 0) {
	    cint	family = int(uv) ;
	    if (char *hebuf{} ; (rs = malloc_ho(&hebuf)) >= 0) {
	        ucentho		he ;
		cint		helen = rs ;
	        if ((rs = getheaddr(&he,hebuf,helen,ap)) >= 0) {
	            if ((rs = hostent_getaf(&he)) >= 0) {
	                cint	af = rs ;
	                if (af == family) {
	                    cint	nlen = var.maxhostlen ;
	                    cchar	*cp ;
	                    if (dn) {
	    			hostent_cur	hc ;
	                        if ((rs = hostent_curbegin(&he,&hc)) >= 0) {
				    auto	henum = hostent_curenumname ;
	                            cchar	*hp ;
	                            while ((rs = henum(&he,&hc,&hp)) > 0) {
	                                if (isindomain(hp,dn)) {
	                                    cp = strwcpy(pn,he.h_name,nlen) ;
	                                    len = intconv(cp - pn) ;
	                                    break ;
	                                }
	                            } /* end while */
	                            rs1 = hostent_curend(&he,&hc) ;
				    if (rs >= 0) rs = rs1 ;
	                        } /* end if */
	                    } /* end if (have a domain name) */
	                    if (rs >= 0) {
	                        if ((pn[0] == '\0') && he.h_name) {
	                            cp = strwcpy(pn,he.h_name,nlen) ;
	                            len = intconv(cp - pn) ;
	                        }
	                    } /* end if (ok) */
	                } else {
	                    rs = SR_NOTFOUND ;
			}
	            } /* end if (gethostent_af) */
	        } /* end if (getheaddr) */
	        rs1 = uc_free(hebuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a,-f) */
	} /* end if (cfhexui) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (nlspeername_inet4) */

vars::operator int () noex {
	int		rs ;
	if ((rs = getbufsize(getbufsize_hn)) >= 0) {
	    var.maxhostlen = rs ;
	}
	return rs ;
} /* end method (vars::operator) */


