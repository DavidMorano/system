/* dialtcp SUPPORT */
/* lang=C++20 */

/* subroutine to dial out to a machine using TCP */
/* version %I% last-modified %G% */

#define	CF_PROTO	0		/* need dynamic protocol number */
#define	CF_TRYINET	0		/* try_inet() ? */

/* revision history:

	= 1998-04-01, David A�D� Morano
	This subroutine was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	dialtcp

	Description:
	This subroutine will dial out to an INET host using the TCP
	protocol and the optional TCP port specified.

	Synopsis:
	int dialtcp(cc *hostname,cc *portspec,int af,int to,int opts) noex

	Arguments:
	hostname	host to dial to
	portspec	port specification to use
	af		address family
	to		to ('>0' means use one, '-1' means don't)
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
	Note that Sun-Solaris has a number (at least two) problems
	with their INET-TCP protocol stack code.  Their version of
	an asynchronous connect does not always work properly unless
	one attempts the connect a second time AFTER the socket is
	already writable (a 'poll(2)' return for output).  This
	problem is handled inside the call to 'uc_connecte(3uc)'.
	The other major problem with Sun-Solaris is that a connect
	attempt will fail to even be made for some reason and no
	bad error is returned from the 'u_connect(3xnet)' other
	than INPROGRESS (which is due to the connection being
	attempted asynchronously).  This latter problem is harder
	to handle.  We handle it by simply trying the whole shebang
	process again when we get an ultimate failure of TIMEDOUT.
	However, this generally also fails on Sun-Solaris since the
	internal state of the Solaris INET-TCP stack is somehow
	hosed.

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstring>
#include	<netdb.h>
#include	<usystem.h>
#include	<usupport.h>
#include	<hostinfo.h>
#include	<hostaddr.h>
#include	<hostent.h>
#include	<getproto.h>
#include	<getprotofamily.h>
#include	<sockaddress.h>
#include	<inetconv.h>
#include	<iserror.h>
#include	<openaddrinfo.h>
#include	<localmisc.h>

#include	"dialtcp.h"


/* local defines */

#ifndef	ADDRINFO
#define	ADDRINFO	struct addrinfo
#endif

#define	PROTONAME	"tcp"

#ifndef	ADDRBUFLEN
#define	ADDRBUFLEN	64
#endif

#ifndef	HEXBUFLEN
#define	HEXBUFLEN	100
#endif

#define	RETRIES		1		/* Sun-Solaris problem */

#define	SUBINFO		struct subinfo
#define	SUBINFO_FL	struct subinfo_flags


/* external subroutines */

extern "C" {
    extern int	getportnum(cchar *,cchar *) noex ;
    extern int	opensockaddr(int,int,int,SOCKADDR *,int) noex ;
}


/* external variables */


/* local typedefs */


/* local structures */

struct subinfo_flags {
	uint		address:1 ;
} ;

struct subinfo {
	cchar		*hostname ;
	cchar		*portspec ;
	cchar		*protoname ;
	SUBINFO_FL	f ;
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

#if	CF_TRYINET
static int	try_inet(SUBINFO *) noex ;
#endif

static int	subinfo_makeconn(SUBINFO *,int,SOCKADDR *) noex ;


/* local variables */


/* exported subroutines */

/* ARGSUSED */
int dialtcp(cchar *hostname,cchar *portspec,int af,int to,int opts) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		fd = -1 ;
	if (hostname && portspec) {
	    rs = SR_INVALID ;
	    if (hostname[0] && portspec[0]) {
	        SUBINFO		si, *sip = &si ;
	        if (to == 0) to = 1 ;
	        if ((rs = subinfo_start(sip,hostname,portspec,to)) >= 0) {
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
	            if ((rs < 0) && (fd >= 0)) {
		        uc_close(fd) ;
	            }
	        } /* end if (subinfo) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (dialtcp) */


/* local subroutines */

static int subinfo_start(SUBINFO *sip,cchar *hn,cchar *ps,int to) noex {
	int		rs = SR_FAULT ;
	if (sip) {
	    memclear(sip) ;
	    sip->hostname = hn ;
	    sip->portspec = ps ;
	    sip->protoname = PROTONAME ;
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
#if	CF_PROTO
	{
	    int		rs ;
	    cchar	*pn = sip->protoname ;
	    if ((rs = getproto_name(pn,-1)) >= 0) {
	        sip->proto = rs ;
	    }
	}
#else
	sip->proto = IPPROTO_TCP ;
#endif /* CF_PROTO */
	return rs ;
}
/* end subroutine (subinfo_proto) */

static int subinfo_svc(SUBINFO *sip) noex {
	int		rs ;
	cchar		*pn = sip->protoname ;
	cchar		*ps = sip->portspec ;
	if ((rs = getportnum(pn,ps)) >= 0) {
	    sip->port = rs ;
	}
	return rs ;
}
/* end subroutine (subinfo_svc) */

static int subinfo_addr(SUBINFO *sip,int af) noex {
	cint		addrlen = ADDRBUFLEN ;
	int		rs ;
	cchar		*hn = sip->hostname ;
	if ((rs = inetpton(sip->addrbuf,addrlen,af,hn,-1)) >= 0) {
	    af = rs ;
	    sip->f.address = true ;
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
	if (sip->f.address) {
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
#if	CF_TRYINET
	    if (isFailConn(rs) || (sip->count == 0)) {
	        rs1 = try_inet(sip) ;
	        fd = rs1 ;
	        if (rs1 != SR_NOTFOUND) rs = rs1 ;
	    } /* end if (any address family) */
#endif /* CF_TRYINET */
	} /* end if */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (subinfo_tryone) */

static int try_inet4(SUBINFO *sip) noex {
	HOSTINFO	hi ;
	HOSTINFO_CUR	hc ;
	cint		af = AF_INET4 ;
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	if ((rs = hostinfo_start(&hi,af,sip->hostname)) >= 0) {
	    SOCKADDRESS	server ;
	    cint	pf = PF_INET4 ;
	    uchar	da[2] = {} ; /* dummy address */
	    if ((rs = sockaddress_start(&server,af,da,sip->port,0)) >= 0) {
	        if ((rs = hostinfo_curbegin(&hi,&hc)) >= 0) {
		    SOCKADDR	*sap ;
	            int		c = 0 ;
		    int		al ;
		    cuchar	*ap ;
	            while (rs >= 0) {
	                al = hostinfo_enumaddr(&hi,&hc,&ap) ;
	                if (al == SR_NOTFOUND) break ;
	                rs = al ;
	                if (rs < 0) break ;
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
	if ((rs = uc_getipnodebyname(&hep,sip->hostname,af,flags)) >= 0) {
	    SOCKADDRESS	server ;
	    cint	pf = PF_INET6 ;
	    uchar	dummy[2] = {} ;
	    if ((rs = sockaddress_start(&server,af,dummy,sip->port,0)) >= 0) {
	        hostent_cur	hc ;
	        if ((rs = hostent_curbegin(hep,&hc)) >= 0) {
	    	    SOCKADDR	*sap ;
	            int		c = 0 ;
	    	    cuchar	*ap ;
	            while ((rs1 = hostent_enumaddr(hep,&hc,&ap)) >= 0) {
	                sockaddress_putaddr(&server,ap) ;
	                c += 1 ;
	                sip->count += 1 ;
	                sap = (SOCKADDR *) &server ;
	                rs = subinfo_makeconn(sip,pf,sap) ;
	                fd = rs ;
	                if (fd >= 0) break ;
	                if ((rs < 0) && (rs != SR_PFNOSUPPORT)) break ;
	            } /* end while */
	            if ((rs >= 0) && (rs1 != SR_NOTFOUND)) rs = rs1 ;
	            rs1 = hostent_curend(hep,&hc) ;
		    if (rs >= 0) rs = rs1 ;
	            if ((rs >= 0) && (c == 0)) rs = SR_HOSTUNREACH ;
	        } /* end if (cursor) */
	        rs1 = sockaddress_finish(&server) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (sockaddress) */
	    rs1 = uc_freehostent(hep) ;
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
	    SOCKADDRESS		server ;
	    cint		pf = rs ;
	    cint		port = sip->port ;
	    uchar		*ap = (uchar *) sip->addrbuf ;
	    if ((rs = sockaddress_start(&server,sip->af,ap,port,0)) >= 0) {
	        SOCKADDR	*sap ;
	        sip->count += 1 ;
	        sap = (SOCKADDR *) &server ;
	        rs = subinfo_makeconn(sip,pf,sap) ;
	        fd = rs ;
	        rs1 = sockaddress_finish(&server) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (sockaddress) */
	} /* end if (getprotofamily) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (try_addr) */

#if	CF_TRYINET
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
	    hostaddr	ha ;
	    cchar	*hn = sip->hostname ;
	    cchar	*ps = sip->portspec ;
	    if ((rs = hostaddr_start(&ha,hn,ps,&hint)) >= 0) {
	        hostaddr_cur	cur ;
	        if ((rs = hostaddr_curbegin(&ha,&cur)) >= 0) {
	            int		c = 0 ;
	            while ((rs1 = hostaddr_enum(&ha,&cur,&aip)) >= 0) {
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
#endif /* CF_TRYINET */

/* make a connection (with protocol family and socket-address) */
static int subinfo_makeconn(SUBINFO *sip,int pf,SOCKADDR *sap) noex {
	cint	st = sip->type ;
	cint	proto = sip->proto ;
	cint	to = sip->to ;
	int	rs ;
	rs = opensockaddr(pf,st,proto,sap,to) ;
	return rs ;
}
/* end subroutine (subinfo_makeconn) */

#if	COMMENT
static int makeint(const void *addr) noex {
	int		hi = 0 ;
	uchar		*us = (uchar *) addr ;
	hi |= (MKCHAR(us[0]) << 24) ;
	hi |= (MKCHAR(us[1]) << 16) ;
	hi |= (MKCHAR(us[2]) << 8) ;
	hi |= (MKCHAR(us[3]) << 0) ;
	return hi ;
}
#endif /* COMMENT */


