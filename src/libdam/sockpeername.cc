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
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<netdb.h>
#include	<usystem.h>
#include	<uinet.h>		/* |INETXADDRLEN| */
#include	<mallocxx.h>
#include	<getbufsize.h>
#include	<getxx.h>
#include	<sockaddress.h>
#include	<hostent.h>
#include	<inetaddr.h>
#include	<sncpyx.h>
#include	<strwcpy.h>
#include	<isindomain.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"sockpeername.h"


/* local defines */


/* imported namespaces */


/* local typedefs */

typedef sockaddr *		sockaddrp ;


/* external subroutines */


/* external variables */


/* local structures */

struct vars {
	int		maxhostlen ;
	int		svcnamelen ;
} ;


/* forward references */

static int	mkvars() noex ;


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
	int proc_in4namex(hostent *) noex ;
	int proc_in4namedom(bool,hostent *) noex ;
	int proc_in4namecan(hostent *) noex ;
	int proc_in4addr(INADDR4 *) noex ;
    } ; /* end struct (suber) */
}

static vars		var ;


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
	int		sal = sizeof(sockaddress) ;
	int		rl = 0 ;
	if ((rs = u_getpeername(s,&sa,&sal)) >= 0) {
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
		    rs = SR_AFNOSUPPORT ;
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
	int		rl = 0 ;
	if (char *pbuf{} ; (rs = malloc_mp(&pbuf)) >= 0) {
	    cint	plen = rs ;
	    if ((rs = sockaddress_getaddr(sap,pbuf,plen)) >= 0) {
		rs = sncpyw(rbuf,rlen,pbuf,rs) ;
	        rl = rs ;
	    } /* end if (sockaddress_getaddr) */
	    rs = rsfree(rs,pbuf) ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? rl : rs ;
}
/* end method (suber::proc_unix) */

int suber::proc_in4(sockaddress *sap) noex {
	INADDR4		naddr{} ;
	cint		nalen = INET4ADDRLEN ;
	int		rs ;
	int		rl = 0 ;
	if ((rs = sockaddress_getaddr(sap,&naddr,nalen)) >= 0) {
	    if (char *hebuf{} ; (rs = malloc_ho(&hebuf)) >= 0) {
		ucentho		he ;
		cint		af = AF_INET4 ;
	        cint		helen = rs ;
		cchar		*na = charp(&naddr) ;
	    	if ((rs = getho_addr(&he,hebuf,helen,af,na,nalen)) >= 0) {
		    if ((rs = proc_in4name(&he)) > 0) {
		        rl = rs ;
		    } else if (rs == 0) {
		        rs = proc_in4addr(&naddr) ;
		        rl = rs ;
		    }
	        } else if (isNotPresent(rs)) {
		    rs = proc_in4addr(&naddr) ;
		    rl = rs ;
		}
		rs = rsfree(rs,hebuf) ;
	    } /* end if (m-a-f) */
	} /* end if (sockaddress_getaddr) */
	return (rs >= 0) ? rl : rs ;
}
/* end method (suber::proc_in4) */

int suber::proc_in4name(hostent *hep) noex {
	int		rs ;
	int		rl = 0 ;
	if ((rs = proc_in4namex(hep)) > 0) {
	    if (cchar *tp ; (tp = strchr(rbuf,'.')) != nullptr) {
		rbuf[tp - rbuf] = '\0' ;
	    }
	}
	return (rs >= 0) ? rl : rs ;
}
/* end method (suber::proc_in4name) */

int suber::proc_in4namex(hostent *hep) noex {
	int		rs = SR_OK ;
	int		rl = 0 ;
	if (dn) {
	    if (dn[0]) {
		if ((rs = proc_in4namedom(false,hep)) == 0) {
		    rs = proc_in4namecan(hep) ;
		}
		rl = rs ;
	    } else {
		rs = proc_in4namecan(hep) ;
		rl = rs ;
	    }
	} else {
		rs = proc_in4namecan(hep) ;
		rl = rs ;
	}
	return (rs >= 0) ? rl : rs ;
}
/* end method (suber::proc_in4namex) */

int suber::proc_in4namedom(bool fany,hostent *hep) noex {
	hostent_cur	hc ;
	int		rs ;
	int		rs1 ;
	int		rl = 0 ;
	if ((rs = hostent_curbegin(hep,&hc)) >= 0) {
	    cchar	*sp ;
	    while ((rs = hostent_curenumname(hep,&hc,&sp)) > 0) {
		if (fany || isindomain(sp,dn)) {
		    rs = sncpy(rbuf,rlen,sp) ;
		    rl = rs ;
	        }
		if (rl > 0) break ;
	    } /* end while */
	    rs1 = hostent_curend(hep,&hc) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (hostent) */
	return (rs >= 0) ? rl : rs ;
}
/* end method (suber::proc_in4namedom) */

#ifdef	COMMENT
int suber::proc_in4nameany(hostent *hep) noex {
	return proc_in4namedom(true,hep) ;
}
/* end method (suber::proc_in4nameany) */
#endif /* COMMENT */

int suber::proc_in4namecan(hostent *hep) noex {
	int		rs ;
	int		rl = 0 ;
	cchar		*sp ;
	if ((rs = hostent_getcanonical(hep,&sp)) > 0) {
	    rs = sncpy(rbuf,rlen,sp) ;
	    rl = rs ;
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	}
	return (rs >= 0) ? rl : rs ;
}
/* end method (suber::proc_in4namecan) */

int suber::proc_in4addr(INADDR4 *naddrp) noex {
	inetaddr	ia ;
	int		rs ;
	int		rs1 ;
	int		rl = 0 ;
	if ((rs = ia.start(naddrp)) >= 0) {
	    {
		rs = ia.getdotaddr(rbuf,rlen) ;
		rl = rs ;
	    }
	    rs1 = ia.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if */
	return (rs >= 0) ? rl : rs ;
}
/* end method (suber::proc_in4addr) */

namespace {
    struct in6er {
	cchar		*dn ;
	char		*rbuf ;
	int		rlen ;
	in6er(char *bp,int bl,cc *d) noex {
	    dn = d ;
	    rbuf = bp ;
	    rlen = bl ;
	} ;
	int operator () (sockaddress *) noex ;
    } ; /* end struct (in6er) */
}

int suber::proc_in6(sockaddress *sap) noex {
	static cint	rsv = mkvars() ;
	int		rs ;
	int		rl = 0 ;
	if ((rs = rsv) >= 0) {
	    in6er	io(rbuf,rlen,dn) ;
	    rs = io(sap) ;
	    rl = rs ;
	} /* end if (mkvars) */
	return (rs >= 0) ? rl : rs ;
}
/* end method (suber::proc_in6) */

int in6er::operator () (sockaddress *sap) noex {
	int		rs ;
	int		rl = 0 ;
	if ((rs = sockaddress_getlen(sap)) >= 0) {
	    sockaddr	*sp = sockaddrp(sap) ;
	    cint	sl = rs ;
	    if (char *sbuf{} ; (rs = malloc_nn(&sbuf)) >= 0) {
		cint	slen = rs ;
		cint	fl = 0 ;
	        if ((rs = uc_getnameinfo(sp,sl,rbuf,rlen,sbuf,slen,fl)) >= 0) {
		    rl = rs ;
		} /* end if (uc_getnameinfo) */
	        rs = rsfree(rs,sbuf) ;
	    } /* end if (m-a-f) */
	} /* end if (sockaddress_getlen) */
	return (rs >= 0) ? rl : rs ;
}
/* end method (in6er::operator) */

static int mkvars() noex {
	int		rs ;
	if ((rs = getbufsize(getbufsize_hn)) >= 0) {
	    var.maxhostlen = rs ;
	    if ((rs = getbufsize(getbufsize_nn)) >= 0) {
		var.svcnamelen = rs ;
	    }
	}
	return rs ;
}
/* end method (mkvars) */


