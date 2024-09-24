/* sockpeername SUPPORT */
/* lang=C++20 */

/* get a peer host name if there is one */
/* version %I% last-modified %G% */


/* revision history:

	= 1986-07-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sockpeername

	Description:
	This subroutine will take an INET socket and a local domain
	name and find the hostname of the remote end of the socket.

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
#include	<csignal>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<netdb.h>
#include	<usystem.h>
#include	<uinet.h>		/* |INETXADDRLEN| */
#include	<mallocxx.h>
#include	<getbufsize.h>
#include	<getxx.h>
#include	<hostent.h>
#include	<sockaddress.h>
#include	<inetaddr.h>
#include	<sncpyx.h>
#include	<strwcpy.h>
#include	<isindomain.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"sockpeername.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

namespace {
    struct suber {
	cchar		*dn ;
	char		*rbuf ;
	int		rlen ;
	suber(char *rb,int rl,cc *adn) noex : rbuf(rb), rlen(rl), dn(adn) { } ;
	int operator () (int) noex ;
	int proc_unix(sockaddress *) noex ;
	int proc_in4(sockaddress *) noex ;
	int proc_in6(sockaddress *) noex ;
	int proc_in4name(hostent *) noex ;
	int proc_in4addr(INADDR *) noex ;
    } ; /* end struct (suber) */
}


/* exported variables */


/* exported subroutines */

int sockpeername(int s,char *rbuf,int rlen,cc *dn) noex {
	int		rs = SR_FAULT ;
	if (rbuf) {
	    rs = SR_BADFD ;
	    rbuf[0] = '\0' ;
	    if (s >= 0) {
		rs = SR_INVALID ;
		if (rlen > 0) {
		    suber	so(rbuf,rlen,dn) ;
		    rs = so(s) ;
		}
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sockpeername) */


/* local subroutines */

int suber::operator () (int s) noex {
	sockaddress	sa ;
	int		rs ;
	int		rs1 ;
	int		alen = sizeof(sockaddress) ;
	int		rl = 0 ;
	if ((rs = u_getpeername(s,&sa,&alen)) >= 0) {
	    if ((rs = sockaddress_getaf(&sa)) >= 0) {
		cint	af = rs ;
		switch (af) {
		case AF_UNIX:
		    rs = proc_unix(&sa) ;
		    rl = rs ;
		    break ;
		case AF_INET4:
		    rs = proc_in4(&sa) ;
		    rl = rs ;
		    break ;
		case AF_INET6:
		    rs = proc_in6(&sa) ;
		    rl = rs ;
		    break ;
		default:
		    rs = SR_NOSYS ;
		    break ;
		} /* end switch */
	    } /* end if (sockaddress_getsf) */
	    rs1 = sockaddress_finish(&sa) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (u_getpeername) */
	return (rs >= 0) ? rl : rs ;
}
/* end method (suber::operator) */
		
int suber::proc_unix(sockaddress *sap) noex {
	int		rs ;
	int		rs1 ;
	int		rl = 0 ;
	char		*pbuf{} ;
	if ((rs = malloc_mp(&pbuf)) >= 0) {
	    cint	plen = rs ;
	    if ((rs = sockaddress_getaddr(sap,pbuf,plen)) >= 0) {
		rs = sncpyw(rbuf,rlen,pbuf,rs) ;
	        rl = rs ;
	    } /* end if (sockaddress_getaddr) */
	    rs1 = uc_free(pbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? rl : rs ;
}
/* end method (suber::proc_unix) */

int suber::proc_in4(sockaddress *sap) noex {
	INADDR		naddr{} ;
	cint		nalen = INET4ADDRLEN ;
	int		rs ;
	int		rs1 ;
	int		rl = 0 ;
	if ((rs = sockaddress_getaddr(sap,&naddr,nalen)) >= 0) {
	    if (char *hebuf{} ; (rs = malloc_ho(&hebuf)) >= 0) {
		ucentho		he ;
		cint		af = AF_INET4 ;
	        cint		helen = rs ;
		cchar		*na = charp(&naddr) ;
	    	if ((rs = getho_addr(&he,hebuf,helen,af,na,nalen)) >= 0) {
		    rs = proc_in4name(&he) ;
		    rl = rs ;
	        } else if (isNotPresent(rs)) {
		    rs = proc_in4addr(&naddr) ;
		    rl = rs ;
		}
		rs1 = uc_free(hebuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (sockaddress_getaddr) */
	return (rs >= 0) ? rl : rs ;
}
/* end method (suber::proc_in4) */

int suber::proc_in4name(hostent *hep) noex {
	int		rs = SR_OK ;
	int		rl = 0 ;
	(void) hep ;
	return (rs >= 0) ? rl : rs ;
}
/* end method (suber::proc_in4name) */

int suber::proc_in4addr(INADDR *naddrp) noex {
	inetaddr	ia ;
	int		rs ;
	int		rs1 ;
	int		rl = 0 ;
	if ((rs = inetaddr_start(&ia,naddrp)) >= 0) {
	    {
		rs = inetaddr_getdotaddr(&ia,rbuf,rlen) ;
		rl = rs ;
	    }
	    rs1 = inetaddr_finish(&ia) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if */
	return (rs >= 0) ? rl : rs ;
}
/* end method (suber::proc_in4addr) */

#ifdef	COMMENT

	        if (domainname != NULL) {
		    hostent_cur		hc ;
	            if ((rs = hostent_curbegin(&he,&hc)) >= 0) {
			cchar	*sp ;
	                while ((rs = hostent_curenumname(&he,&hc,&sp)) > 0) {
	                    if (isindomain(sp,domainname)) break ;
	                } /* end while */
	                hostent_curend(&he,&hc) ;
		    } /* end if (hostent) */
	        } else {
	            rs = SR_HOSTUNREACH ;
		}
	        if (rs >= 0) {
		    cchar	*tp ;
	            strwcpy(peername,np,nlen) ;
	            if ((tp = strchr(peername,'.')) != NULL) {
			peername[tp-peername] = '\0' ;
		    }
	        } else {
	            hostent_getcanonical(&he,&np) ;
	            strwcpy(peername,np,nlen) ;
	        } /* end if */

#else

int suber::proc_in6(sockaddress *) noex {
	return SR_OK ;
}

#endif /* COMMENT */


