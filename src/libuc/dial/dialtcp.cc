/* dialtcp SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutine to dial out to a machine using TCP */
/* version %I% last-modified %G% */

#define	CF_PROTO	0		/* need dynamic protocol number */
#define	CF_TRYINET	0		/* try_inet() ? */

/* revision history:

	= 1998-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	dialtcp

	Description:
	This subroutine will dial out to an INET host using the TCP
	protocol and the optional TCP port specified.

	Synopsis:
	int dialtcp(cc *hn,cc *ps,int af,int to,int opts) noex

	Arguments:
	hn		host to dial to
	ps		port specification to use
	af		address family
	to		to ('>0' means use one, '-1' means do not)
	opts		any dial options

	Returns:
	<0		error in dialing
	>=0		file descriptor

	Notes:
	What is up with the 'to' argument?
	<0              use the system default to (xx minutes --
			whatever)
	==0             asynchronously span the connect and do not wait
			for it
	>0		use the to as it is

	Notes:
	Note that Sun-Solaris® has a number (at least two) problems
	with their INET-TCP protocol stack code.  Their version of
	an asynchronous connect does not always work properly unless
	one attempts the connect a second time AFTER the socket is
	already writable (a |poll(2)| return for output).  This
	problem is handled inside the call to |uc_connecte(3uc)|.
	The other major problem with Sun-Solaris® is that a connect
	attempt will fail to even be made for some reason and no
	bad error is returned from the |ut_connect(3xnet)| other
	than INPROGRESS (which is due to the connection being
	attempted asynchronously).  This latter problem is harder
	to handle.  We handle it by simply trying the whole shebang
	process again when we get an ultimate failure of TIMEDOUT.
	However, this generally also fails on Sun-Solaris® since the
	internal state of the Solaris® INET-TCP stack is somehow
	hosed.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<getproto.h>
#include	<getprotofamily.h>
#include	<getportnum.h>
#include	<hostinfo.h>
#include	<hostaddr.h>
#include	<hostent.h>
#include	<sockaddress.h>
#include	<inetconv.h>
#include	<iserror.h>
#include	<openaddrinfo.h>
#include	<localmisc.h>

#include	"dialtcp.h"

import libutil ;

/* local defines */

#define	PROTONAME	"tcp"

#ifndef	ADDRBUFLEN
#define	ADDRBUFLEN	64
#endif

#define	RETRIES		1		/* Sun-Solaris® problem */

#define	SUBINFO		struct subinfo
#define	SUBINFO_FL	struct subinfo_flags

#ifndef	CF_PROTO
#define	CF_PROTO	0		/* need dynamic protocol number */
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	opensockaddr(int,int,int,SOCKADDR *,int) noex ;
}


/* external variables */


/* local typedefs */


/* local structures */

struct subinfo_flags {
	uint		address:1 ;
} ;

struct subinfo {
	cchar		*hn ;
	cchar		*ps ;
	cchar		*pn ;
	SUBINFO_FL	fl ;
	int		count ;
	int		type ;
	int		proto ;
	int		af ;
	int		port ;
	int		to ;
	uchar		addrbuf[ADDRBUFLEN + 1] ;
} ;


/* forward references */

static int	subinfo_start(SUBINFO *,cchar *,cchar *,int) noex ;
static int	subinfo_proto(SUBINFO *) noex ;
static int	subinfo_svc(SUBINFO *) noex ;
static int	subinfo_finish(SUBINFO *) noex ;

static int	subinfo_addr(SUBINFO *,int) noex ;
static int	subinfo_try(SUBINFO *) noex ;
static int	subinfo_tryone(SUBINFO *) noex ;

static int	try_inet4(SUBINFO *) noex ;
static int	try_inet6(SUBINFO *) noex ;
static int	try_addr(SUBINFO *) noex ;
static int	try_inet(SUBINFO *) noex ;

static int	subinfo_makeconn(SUBINFO *,int,SOCKADDR *) noex ;


/* local variables */

const bool		f_proto = CF_PROTO ;
const bool		f_tryinet = CF_TRYINET ;


/* exported variables */


/* exported subroutines */

int dialtcp(cchar *hn,cchar *ps,int af,int to,int opts) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		fd = -1 ;
	if (hn && ps) {
	    rs = SR_INVALID ;
	    if (hn[0] && ps[0]) {
	        SUBINFO		si, *sip = &si ;
	        if (to == 0) to = 1 ;
	        if ((rs = subinfo_start(sip,hn,ps,to)) >= 0) {
	            if ((rs = subinfo_proto(sip)) >= 0) {
	                if ((rs = subinfo_svc(sip)) >= 0) {
	                    if ((rs = subinfo_addr(sip,af)) >= 0) {
	                        if ((rs = subinfo_try(sip)) >= 0) {
	                            fd = rs ;
	                            rs = dialopts(fd,opts) ;
	                        }
	                        if ((rs < 0) && (sip->count == 0)) {
	                            rs = SR_HOSTUNREACH ;
			        }
	                    } /* end if (subinfo_addr) */
	                } /* end if (subinfo_svc) */
	            } /* end if (subinfo_proto) */
	            rs1 = subinfo_finish(sip) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (subinfo) */
	        if ((rs < 0) && (fd >= 0)) {
		    uc_close(fd) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (dialtcp) */


/* local subroutines */

static int subinfo_start(SUBINFO *sip,cchar *hn,cchar *ps,int to) noex {
	int		rs = SR_FAULT ;
	if (sip) {
	    rs = memclear(sip) ;
	    sip->hn = hn ;
	    sip->ps = ps ;
	    sip->pn = PROTONAME ;
	    sip->type = SOCK_STREAM ;
	    sip->to = to ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (subinfo_start) */

static int subinfo_finish(SUBINFO *sip) noex {
	int		rs = SR_FAULT ;
	if (sip) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (subinfo_finish) */

static int subinfo_proto(SUBINFO *sip) noex {
	int		rs = SR_OK ;
	if_constexpr (f_proto) {
	    cchar	*pn = sip->pn ;
	    if ((rs = getproto_name(pn,-1)) >= 0) {
	        sip->proto = rs ;
	    }
	} else {
	    sip->proto = IPPROTO_TCP ;
	}
	return rs ;
}
/* end subroutine (subinfo_proto) */

static int subinfo_svc(SUBINFO *sip) noex {
	int		rs ;
	cchar		*pn = sip->pn ;
	cchar		*ps = sip->ps ;
	if ((rs = getportnum(pn,ps)) >= 0) {
	    sip->port = rs ;
	}
	return rs ;
}
/* end subroutine (subinfo_svc) */

static int subinfo_addr(SUBINFO *sip,int af) noex {
	cint		addrlen = ADDRBUFLEN ;
	int		rs ;
	cchar		*hn = sip->hn ;
	if ((rs = inetpton(sip->addrbuf,addrlen,af,hn,-1)) >= 0) {
	    af = rs ;
	    sip->fl.address = true ;
	} else if (rs == SR_INVALID) {
	    rs = SR_OK ;
	}
	sip->af = af ;
	return (rs >= 0) ? af : rs ;
}
/* end subroutine (subinfo_addr) */

static int subinfo_try(SUBINFO *sip) noex {
	int		rs ;
	if ((rs = subinfo_tryone(sip)) == SR_TIMEDOUT) {
	    int		c = RETRIES ;
	    while ((c-- > 0) && (rs == SR_TIMEDOUT)) {
	        rs = subinfo_tryone(sip) ;
	    } /* end while */
	}
	return rs ;
}
/* end subroutine (subinfo_try) */

static int subinfo_tryone(SUBINFO *sip) noex {
	int		rs = SR_HOSTUNREACH ;
	int		rs1 ;
	int		af = sip->af ;
	int		fd = -1 ;
	if (sip->fl.address) {
	    rs = try_addr(sip) ;
	    fd = rs ;
	} else {
	    /* first try IPv4 addresses */
	    if ((isFailConn(rs) || (sip->count == 0)) && 
	        ((af == AF_UNSPEC) || (af == AF_INET4))) {
	        rs1 = try_inet4(sip) ;
	        fd = rs1 ;
	        if (rs1 != SR_NOTFOUND) rs = rs1 ;
	    } /* end if (IPv4) */
	    /* now try IPv6 addresses */
	    if ((isFailConn(rs) || (sip->count == 0)) && 
	        ((af == AF_UNSPEC) || (af == AF_INET6))) {
	        rs1 = try_inet6(sip) ;
	        fd = rs1 ;
	        if (rs1 != SR_NOTFOUND) rs = rs1 ;
	    } /* end if (IPv6) */
	    if_constexpr (f_tryinet) {
	        if (isFailConn(rs) || (sip->count == 0)) {
	            rs1 = try_inet(sip) ;
	            fd = rs1 ;
	            if (rs1 != SR_NOTFOUND) rs = rs1 ;
	        } /* end if (any address family) */
	    } /* end if_constexpr (f_tryinet) */
	} /* end if */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (subinfo_tryone) */

static int try_inet4(SUBINFO *sip) noex {
	cint		af = AF_INET4 ;
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	if (hostinfo hi ; (rs = hostinfo_start(&hi,af,sip->hn)) >= 0) {
	    sockaddress	server ;
	    cint	pf = PF_INET4 ;
	    uchar	da[2] = {} ; /* dummy address */
	    if ((rs = sockaddress_start(&server,af,da,sip->port,0)) >= 0) {
		hostinfo_cur	hc ;
	        if ((rs = hostinfo_curbegin(&hi,&hc)) >= 0) {
		    SOCKADDR	*sap ;
	            int		c = 0 ;
		    cuchar	*ap ;
	            while ((rs = hostinfo_curenumaddr(&hi,&hc,&ap)) > 0) {
	                sockaddress_putaddr(&server,ap) ;
	                c += 1 ;
	                sip->count += 1 ;
	                sap = (SOCKADDR *) &server ;
	                rs = subinfo_makeconn(sip,pf,sap) ;
	                fd = rs ;
	                if (fd >= 0) break ;
	                if ((rs < 0) && (rs != SR_PFNOSUPPORT)) break ;
	            } /* end while */
	            rs1 = hostinfo_curend(&hi,&hc) ;
		    if (rs >= 0) rs = rs1 ;
	            if ((rs >= 0) && (c == 0)) rs = SR_HOSTUNREACH ;
	        } /* end if (cursor) */
	        rs1 = sockaddress_finish(&server) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (sockaddress) */
	    rs1 = hostinfo_finish(&hi) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (hostinfo) */
	if ((rs < 0) && (fd >= 0)) u_close(fd) ;
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (try_inet4) */

static int try_inet6(SUBINFO *sip) noex {
	HOSTENT		*hep ;
	cint		af = AF_INET6 ;
	int		rs ;
	int		rs1 ;
	int		flags ;
	int		fd = -1 ;
	flags = AI_ADDRCONFIG ;
	if ((rs = uc_getipnodebyname(&hep,sip->hn,af,flags)) >= 0) {
	    sockaddress	server ;
	    cint	pf = PF_INET6 ;
	    uchar	dummy[2] = {} ;
	    if ((rs = sockaddress_start(&server,af,dummy,sip->port,0)) >= 0) {
	        hostent_cur	hc ;
	        if ((rs = hostent_curbegin(hep,&hc)) >= 0) {
	    	    SOCKADDR	*sap ;
	            int		c = 0 ;
	    	    cuchar	*ap ;
	            while ((rs = hostent_curenumaddr(hep,&hc,&ap)) > 0) {
	                sockaddress_putaddr(&server,ap) ;
	                c += 1 ;
	                sip->count += 1 ;
	                sap = (SOCKADDR *) &server ;
	                rs = subinfo_makeconn(sip,pf,sap) ;
	                fd = rs ;
	                if (fd >= 0) break ;
	                if ((rs < 0) && (rs != SR_PFNOSUPPORT)) break ;
	            } /* end while */
	            rs1 = hostent_curend(hep,&hc) ;
		    if (rs >= 0) rs = rs1 ;
	            if ((rs >= 0) && (c == 0)) rs = SR_HOSTUNREACH ;
	        } /* end if (cursor) */
	        rs1 = sockaddress_finish(&server) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (sockaddress) */
	    rs1 = uc_hostentfree(hep) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (uc_getipnodebyname) */
	if ((rs < 0) && (fd >= 0)) {
	    uc_close(fd) ;
	}
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (try_inet6) */

static int try_addr(SUBINFO *sip) noex {
	int		rs ;
	int		rs1 ;
	int		fd = 0 ;
	if ((rs = getprotofamily(sip->af)) >= 0) {
	    sockaddress		server ;
	    cint		pf = rs ;
	    cint		port = sip->port ;
	    uchar		*ap = (uchar *) sip->addrbuf ;
	    if ((rs = sockaddress_start(&server,sip->af,ap,port,0)) >= 0) {
	        SOCKADDR	*sap = (SOCKADDR *) &server ;
		{
	            sip->count += 1 ;
	            rs = subinfo_makeconn(sip,pf,sap) ;
	            fd = rs ;
		}
	        rs1 = sockaddress_finish(&server) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (sockaddress) */
	    if ((rs < 0) && (fd >= 0)) u_close(fd) ;
	} /* end if (getprotofamily) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (try_addr) */

static int try_inet(SUBINFO *sip) noex {
	ADDRINFO	hint{} ;
	ADDRINFO	*aip{} ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		fd = -1 ;
	hint.ai_protocol = sip->proto ;
	if (sip->af >= 0) {
	    if ((rs = getprotofamily(sip->af)) >= 0) {
	        hint.ai_family = rs ;	/* documentation says use PF! */
	    }
	}
	/* do the spin */
	if (rs >= 0) {
	    cchar	*hn = sip->hn ;
	    cchar	*ps = sip->ps ;
	    if (hostaddr ha ; (rs = hostaddr_start(&ha,hn,ps,&hint)) >= 0) {
	        hostaddr_cur	cur ;
	        if ((rs = hostaddr_curbegin(&ha,&cur)) >= 0) {
	            int		c = 0 ;
	            while ((rs1 = hostaddr_curenum(&ha,&cur,&aip)) >= 0) {
	                cint	aif = aip->ai_family ; /* PF */
		        bool	f = false ;
	                f = f || (hint.ai_family == 0) ;
	                f = f || (hint.ai_family == aif) ;
	                f = f || (aif == 0) ;
		        if (f) {
			    cint	to = sip->to ;
	                    c += 1 ;
	                    sip->count += 1 ;
			    rs = openaddrinfo(aip,to) ;
	                    fd = rs ;
	                } /* end if (protocol match) */
	                if (fd >= 0) break ;
	                if ((rs < 0) && (rs != SR_PFNOSUPPORT)) break ;
	            } /* end while */
	            if ((rs >= 0) && (rs1 != SR_NOTFOUND)) rs = rs1 ;
	            rs1 = hostaddr_curend(&ha,&cur) ;
		    if (rs >= 0) rs = rs1 ;
	            if ((rs >= 0) && (c == 0)) rs = SR_HOSTUNREACH ;
	        } /* end if (cursor) */
	        rs1 = hostaddr_finish(&ha) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (initialized host addresses) */
	    if ((rs < 0) && (fd >= 0)) u_close(fd) ;
	} /* end if (ok) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (try_inet) */

/* make a connection (with protocol family and socket-address) */
static int subinfo_makeconn(SUBINFO *sip,int pf,SOCKADDR *sap) noex {
	cint	st = sip->type ;
	cint	proto = sip->proto ;
	cint	to = sip->to ;
	return opensockaddr(pf,st,proto,sap,to) ;
}
/* end subroutine (subinfo_makeconn) */


