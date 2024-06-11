/* nlspeername SUPPORT */
/* lang=C++20 */

/* try to find the peername for an NLS (XTI) connection */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subrotuine tries to find the peername for the given
	NLS (TLI) address.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
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
#include	<sockaddress.h>
#include	<inetaddr.h>
#include	<strwcpy.h>
#include	<cfhex.h>
#include	<cfhexs.h>
#include	<isindomain.h>
#include	<localmisc.h>

#include	"nlspeername.h"


/* local defines */

#ifndef	INETXADDRLEN
#define	INETXADDRLEN	sizeof(struct in_addr)
#endif

#ifndef	PEERNAMELEN
#define	PEERNAMELEN	MAX(MAXHOSTNAMELEN,MAXPATHLEN)
#endif


/* external subroutines */

extern "C" {
    extern int	getheaddr(cvoid *,HOSTENT *,char *,int) noex ;
}


/* external variables */


/* forward references */

static int	nlspeername_unix(char *,cchar *,cchar *,int) noex ;
static int	nlspeername_inet4(char *,cchar *,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int nlspeername(cchar *addr,cchar *dn,char *pn) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (addr && dn && pn) {
	    cint	al = strlen(addr) ;
	    rs = SR_INVALID ;
	    pn[0] = '\0' ;
	    if (al >= 16) {
	        if (int af ; (rs = cfhexi(addr,4,&af)) >= 0) {
	            if (af == AF_UNIX) {
	                rs = nlspeername_unix(pn,dn,addr,al) ;
			len = rs ;
	            } else if (af == AF_INET4) {
	                rs = nlspeername_inet4(pn,dn,addr,al) ;
			len = rs ;
	            } /* end if (recognized the address family) */
	        } /* end if (cfhexi) */
	    } /* end if (good) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (nlspeername) */


/* local subroutines */

static int nlspeername_unix(char *pn,cchar *dn,cchar *addr,int al) noex {
	cint		nlen = MAXHOSTNAMELEN ;
	int		rs = SR_OK ;
	int		len = 0 ;
	(void) dn ;
	al -= 4 ;
	if ((al / 2) <= nlen) {
	    uchar	*upn = ucharp(pn) ;
	    rs = cfhexs((addr + 4),al,upn) ;
	    len = rs ;
	} else {
	    rs = SR_TOOBIG ;
	}
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (nlspeername_unix) */

static int nlspeername_inet4(char *pn,cchar *dn,cchar *ap,int al) noex {
	uint		inetaddr ;
	cint		adv = 8 ;
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	al -= adv ;
	if ((rs = cfhexui(ap+adv,al,&inetaddr)) >= 0) {
	    HOSTENT_CUR	hc ;
	    cint	family = rs ;
	    char	*hebuf{} ;
	    if ((rs = malloc_ho(&hebuf)) >= 0) {
	        HOSTENT		he ;
		cint		helen = rs ;
	        if ((rs = getheaddr(&inetaddr,&he,hebuf,helen)) >= 0) {
	            if ((rs = hostent_getaf(&he)) >= 0) {
	                cint	af = rs ;
	                if (af == family) {
	                    cint	nlen = MAXHOSTNAMELEN ;
	                    cchar	*cp ;
	                    if ((rs >= 0) && dn) {
	                        if ((rs = hostent_curbegin(&he,&hc)) >= 0) {
				    auto	henum = hostent_enumname ;
	                            cchar	*hp ;
	                            while ((rs = henum(&he,&hc,&hp)) > 0) {
	                                if (isindomain(hp,dn)) {
	                                    cp = strwcpy(pn,he.h_name,nlen) ;
	                                    len = (cp - pn) ;
	                                    break ;
	                                }
	                            } /* end while */
	                            rs1 = hostent_curend(&he,&hc) ;
				    if (rs >= 0) rs = rs1 ;
	                        } /* end if */
	                    } /* end if (have a domain name) */
	                    if (rs >= 0) {
	                        if ((pn[0] == '\0') && (he.h_name != NULL)) {
	                            cp = strwcpy(pn,he.h_name,nlen) ;
	                            len = (cp - pn) ;
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


