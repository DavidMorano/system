/* nettime SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* program to get time from a network time server host */
/* version %I% last-modified %G% */

#define	CF_USEUDP	1		/* use UDP */
#define	CF_UDPMUX	1		/* pretend using UDPMUX */
#define	CF_SOLARIS	1		/* broken Solaris */
#define	CF_CONNECTUDP	0		/* make UDP connection */

/* revision history:

	= 2009-04-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 2009 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getnettime

	Description:
	This subroutine will get the time-of-day from a time server
	specified by a hostname given on the command line.  The
	subroutine tries to connect to a TCP listener on the time
	server and will read 4 bytes out of the socket.  These four
	bytes, when organized as a long word in network byte order,
	represent the time in seconds since Jan 1, 1900.  We will
	subtract the value "86400 * ((365 * 70) + 17)" to get the
	time in seconds since Jan 1, 1970 (the UNIX epoch).

	Synopsis:
	int getnettime(nettime *ntp,int proto,int af,cc *hn,cc *svc,int to)

	Arguments:
	ntp		pointer to result structure
	proto		protocol number ( UDP, TCP )
	af		address family
	hn		host-name
	svc		service-name
	to		timeout

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/time.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<climits>		/* |UCHAR_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<netdb.h>
#include	<usystem.h>
#include	<getprotofamily.h>
#include	<dialtcp.h>
#include	<hostaddr.h>
#include	<sockaddress.h>
#include	<vechand.h>
#include	<sbuf.h>
#include	<cthex.h>
#include	<sncpyx.h>
#include	<localmisc.h>		/* |MAXPATHLEN| */

#include	"nettime.h"


/* local defines */

#ifndef	ADDRBUFLEN
#define	ADDRBUFLEN	(MAXPATHLEN + 20)
#endif

#define	NETBUFLEN	1024

#define	NETTIMEROLL	3576712516UL

#define	LARGETIME	(60 * 22)
#define	SMALLTIME	(50)

#define	EPOCHDIFF	(86400 * ((365 * 70) + 17))

#define	NETTIMELEN	4

#ifndef	NYEARS_CENTURY
#define	NYEARS_CENTURY	100
#endif

#ifndef	INETSVC_TIME
#define	INETSVC_TIME	"time"
#endif

#define	NTRIES		2

#define	UDPARGS		struct udpargs


/* external subroutines */


/* external variables */


/* local structures */

struct udpargs {
	cchar		*hostname ;
	cchar		*svc ;
	nettime		*ntp ;
	TIMEVAL		*nsp ;
	TIMEVAL		*nep ;
	int		proto ;
	int		pf ;
	int		af ;
	int		fd ;
	int		to ;
	int		c ;
} ; /* end struct (udpargs) */


/* forward references */

static int	nettime_tcp(NETTIME *,int,cchar *,cchar *,int) noex ;
static int	nettime_udp(NETTIME *,int,cchar *,cchar *,int) noex ;
static int	nettime_udptrythem(UDPARGS *,char *) noex ;
static int	nettime_udptrysome(UDPARGS *,char *,
			vechand *,HOSTADDR *,int) noex ;
static int	nettime_udptryone(UDPARGS *,char *,
			ADDRINFO *) noex ;
static int	nettime_udptryoner(UDPARGS *,char *,
			ADDRINFO *,int) noex ;

static uint64_t	gettime_inet(cchar *) noex ;

static uint64_t	utime_timeval(TIMEVAL *) noex ;
static uint64_t	utime_tcpcalc(uint64_t,uint64_t) noex ;
static uint64_t	utime_udpcalc(uint64_t,uint64_t) noex ;

static int	tv_loadusec(TIMEVAL *,int64_t) noex ;

static int	vechand_already(vechand *,void *) noex ;

static int	connagain(int) noex ;

static int	isaddrsame(cvoid *,cvoid *) noex ;


/* local variables */

constexpr int	connagains[] = {
	SR_NOENT,
	SR_HOSTUNREACH,
	SR_HOSTDOWN,
	SR_CONNREFUSED,
	SR_NOPROTOOPT,
	SR_PROTONOSUPPORT,
	SR_AFNOSUPPORT,
	SR_TIMEDOUT,
	0
} ;

constexpr int		tslen = TIMEBUFLEN ;
constexpr int		onemillion = 1000000 ;

constexpr cchar		inetsvc[] = INETSVC_TIME ;


/* exported variables */


/* exported subroutines */

int getnettime(nettime *ntp,int proto,int af,cc *hostname,cc *svc,int to) noex {
	int		rs = SR_FAULT ;
	int		f_got = false ;
	if (ntp && hostname) {
	    rs = SR_INVALID ;
	    memclear(ntp) ;
	    if ((proto >= 0) && (af >= 0) && hostname[0]) {
	        bool	f = ((proto == IPPROTO_UDP) || (proto <= 0)) ;
		if ((svc == nullptr) || (svc[0] == '\0')) {
		    svc = inetsvc ;
		}
	        if ((! f_got) && f) {
	            rs = nettime_udp(ntp,af,hostname,svc,to) ;
	            f_got = (rs > 0) ;
	        } /* end if */
	        f = ((proto == IPPROTO_TCP) || (proto <= 0)) ;
	        if ((! f_got) && f && ((rs >= 0) || connagain(rs))) {
	            rs = nettime_tcp(ntp,af,hostname,svc,to) ;
	            f_got = (rs > 0) ;
	        } /* end if */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? f_got : rs ;
}
/* end subroutine (nettime) */


/* local subroutines */

static int nettime_udp(nettime *ntp,int af,cc *hostname,cc *svc,int to) noex {
	TIMEVAL		netstart ;
	TIMEVAL		netend ;
	int		rs = SR_OK ;
	int		f_got = false ;
	char		tsbuf[tslen + 1] ;

	ntp->proto = IPPROTO_UDP ;

#if	CF_USEUDP
	{
	    UDPARGS	ua{} ;
	    ua.ntp = ntp ;
	    ua.proto = ntp->proto ;
	    ua.af = af ;
	    ua.hostname = hostname ;
	    ua.svc = svc ;
	    ua.to = to ;
	    ua.nsp = &netstart ;
	    ua.nep = &netend ;
	    ua.pf = -1 ;
	    rs = nettime_udptrythem(&ua,tsbuf) ;
	    f_got = (rs > 0) ;
	}
#else
	rs = SR_PROTONOSUPPORT ;
#endif /* CF_USEUDP */

	if ((rs >= 0) && f_got) {
	    uint64_t	uti_start, uti_end ;
	    uint64_t	uti_inet, uti_local ;
	    uint64_t	uti_trip ;
	    int64_t	uti_off ;

	    uti_start = utime_timeval(&netstart) ;

	    uti_end = utime_timeval(&netend) ;

	    uti_local = utime_udpcalc(uti_end,uti_start) ;

	    {
	        uint64_t	t = gettime_inet(tsbuf) ;
	        uti_inet = t * onemillion ;
	        uti_off = uti_inet - uti_local ;
	        tv_loadusec(&ntp->off,uti_off) ;
	    }

	    uti_trip = uti_end - uti_start ;
	    tv_loadusec(&ntp->trip,uti_trip) ;

	} /* end if */

	return (rs >= 0) ? f_got : rs ;
}
/* end subroutine (nettime_udp) */

static int nettime_tcp(nettime *ntp,int af,cc *hostname,cc *svc,int to) noex {
	TIMEVAL		netstart, netend ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		s = -1 ;
	int		pf ;
	int		raf ;
	int		len ;
	int		f_got = false ;
	char		tsbuf[tslen + 1] ;

	ntp->proto = IPPROTO_TCP ;

/* retrieve network data */

	gettimeofday(&netstart,nullptr) ;

	if ((af == AF_UNSPEC) || (af == AF_INET4)) {
	    raf = AF_INET4 ;
	    pf = PF_UNSPEC ;
	    if ((rs1 = getprotofamily(raf)) >= 0) pf = rs1 ;
	    ntp->pf = pf ;
	    rs = dialtcp(hostname,svc,raf,to,0) ;
	    s = rs ;
	}

	if ((((s < 0) && (rs >= 0)) || connagain(rs)) &&
	    ((af == AF_UNSPEC) || (af == AF_INET6))) {
	    raf = AF_INET6 ;
	    pf = PF_UNSPEC ;
	    if ((rs1 = getprotofamily(raf)) >= 0) pf = rs1 ;
	    ntp->pf = pf ;
	    rs = dialtcp(hostname,svc,raf,to,0) ;
	    s = rs ;
	}

	if (rs >= 0) {

	    if ((rs = uc_reade(s,tsbuf,tslen,to,FM_EXACT)) >= 0) {
	        len = rs ;
	        if (len == NETTIMELEN) {
	            f_got = true ;
	            gettimeofday(&netend,nullptr) ;
	        } else {
	            rs = SR_BADMSG ;
		}
	    }

	    u_close(s) ;
	} /* end if */

/* process network data */

	if ((rs >= 0) && f_got) {
	    uint64_t	uti_start, uti_end ;
	    uint64_t	uti_inet, uti_local ;
	    uint64_t	uti_trip ;
	    int64_t	uti_off ;

	    uti_start = utime_timeval(&netstart) ;

	    uti_end = utime_timeval(&netend) ;

	    uti_local = utime_tcpcalc(uti_end,uti_start) ;

	    {
	        uint64_t	t = gettime_inet(tsbuf) ;
	        uti_inet = t * onemillion ;
	        uti_off = uti_inet - uti_local ;
	        tv_loadusec(&ntp->off,uti_off) ;
	    }

	    uti_trip = uti_end - uti_start ;
	    tv_loadusec(&ntp->trip,uti_trip) ;

	} /* end if */

	return (rs >= 0) ? f_got : rs ;
}
/* end subroutine (nettime_tcp) */

#if	CF_USEUDP

static int nettime_udptrythem(UDPARGS *uap,char *tsbuf) noex {
	ADDRINFO	hint{} ;
	vechand		alist ;
	HOSTADDR	ha ;
	cint		proto = IPPROTO_UDP ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		pf ;
	int		f_got = false ;

	uap->proto = proto ;
	hint.ai_protocol = proto ;

	if (uap->af >= 0) {
	    int pf = getprotofamily(uap->af) ;
	    if (pf >= 0) {
	        uap->pf = pf ;
	        hint.ai_family = pf ;
	    }
	}

	if ((rs = vechand_start(&alist,2,0)) >= 0) {
	    cchar	*hn = uap->hostname ;
	    if ((rs = hostaddr_start(&ha,hn,uap->svc,&hint)) >= 0) {

	        if (((! f_got) && (rs >= 0)) || connagain(rs)) {
	            pf = PF_INET4 ;
	            if ((uap->pf <= 0) || (uap->pf == pf)) {
	                rs1 = nettime_udptrysome(uap,tsbuf,&alist,&ha,pf) ;
	                f_got = (rs1 > 0) ;
	                if ((rs >= 0) || (rs1 != SR_NOTFOUND)) rs = rs1 ;
	            }
	        }

	        if (((! f_got) && (rs >= 0)) || connagain(rs)) {
	            pf = PF_INET6 ;
	            if ((uap->pf <= 0) || (uap->pf == pf)) {
	                rs1 = nettime_udptrysome(uap,tsbuf,&alist,&ha,pf) ;
	                f_got = (rs1 > 0) ;
	                if ((rs >= 0) || (rs1 != SR_NOTFOUND)) rs = rs1 ;
	            }
	        }

	        if (((! f_got) && (rs >= 0)) || connagain(rs)) {
	            pf = PF_UNSPEC ;
	            if (uap->pf <= 0) {
	                rs1 = nettime_udptrysome(uap,tsbuf,&alist,&ha,pf) ;
	                f_got = (rs1 > 0) ;
	                if ((rs >= 0) || (rs1 != SR_NOTFOUND)) rs = rs1 ;
	            }
	        }

	        hostaddr_finish(&ha) ;
	        if ((rs >= 0) && (uap->c == 0)) rs = SR_PROTONOSUPPORT ;
	    } /* end if (initialized host addresses) */
	    vechand_finish(&alist) ;
	} /* end if (vechand) */

	return (rs >= 0) ? f_got : rs ;
}
/* end subroutine (nettime_udptrythem) */

static int nettime_udptrysome(UDPARGS *uap,char *tsbuf,vechand *alp,
		HOSTADDR *hap,int pf) noex {
	hostaddr_cur	cur ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		f_got = false ;

	if ((rs = hostaddr_curbegin(hap,&cur)) >= 0) {
	    ADDRINFO	*aip ;
	    int		proto ;
	    bool		f ;
	    while (hostaddr_curenum(hap,&cur,&aip) >= 0) {

	        proto = aip->ai_protocol ;
	        f = ((proto == uap->proto) || (proto <= 0)) ;

	        f = f && ((pf == PF_UNSPEC) || (pf == aip->ai_family)) ;

	        if (f) {
	            rs1 = vechand_already(alp,aip) ;
	            f = (rs1 == SR_NOTFOUND) ;
	        }

	        if (f) {

	            uap->c += 1 ;
	            rs = vechand_add(alp,aip) ;
	            if (rs >= 0) {
	                rs = nettime_udptryone(uap,tsbuf,aip) ;
	                f_got = (rs > 0) ;
	            }

	            if (f_got) break ;
	        } /* end if (match) */

	    } /* end while */

	    hostaddr_curend(hap,&cur) ;
	} /* end if (cursor) */

	return (rs >= 0) ? f_got : rs ;
}
/* end subroutine (nettime_udptrysome) */

static int nettime_udptryone(UDPARGS *uap,char *tsbuf,ADDRINFO *aip) noex {
	int		rs = SR_OK ;
	int		pf ;
	int		st ;
	int		proto ;
	int		i ;
	int		f = false ;

	uap->ntp->pf = aip->ai_family ;
	pf = aip->ai_family ;
	st = aip->ai_socktype ;
	proto = uap->proto ;
	if ((rs = u_socket(pf,st,proto)) >= 0) {
	    cint	fd = rs ;

#if	CF_CONNECTUDP
	    rs = u_connect(fd,aip->ai_addr,aip->ai_addrlen) ;
#endif

	    for (i = 0 ; (rs >= 0) && (i < NTRIES) ; i += 1) {

	        rs = nettime_udptryoner(uap,tsbuf,aip,fd) ;
	        f = (rs > 0) ;

#if	CF_SOLARIS
	        if (f || ((rs < 0) && (! connagain(rs))))
	            break ;
#else
	        if (f || ((rs < 0) && (rs != SR_TIMEDOUT)))
	            break ;
#endif

	    } /* end for */

	    u_close(fd) ;
	} /* end if (opened socket) */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (nettime_udptryone) */

static int nettime_udptryoner(UDPARGS *uap,char *tsbuf,
		ADDRINFO *aip,int fd) noex {
	sockaddress	from ;
	int		rs = SR_OK ;
	int		to = uap->to ;
	int		fromlen ;
	int		netlen ;
	int		f = false ;
	char		netbuf[NETBUFLEN + 1] ;

	tsbuf[0] = '\0' ;
	gettimeofday(uap->nsp,nullptr) ;

#if	CF_UDPMUX
	rs = sncpy2(netbuf,NETBUFLEN,INETSVC_TIME,"\n") ;
	netlen = rs ;
#else
	netbuf[0] ;
	netlen = 0 ;
#endif /* CF_UDPMUX */

	if (rs >= 0) {
	    cint	addrlen = aip->ai_addrlen ;

#if	CF_CONNECTUDP
	    rs = u_send(fd,netbuf,netlen,0) ;
#else
	    rs = u_sendto(fd,netbuf,netlen,0,aip->ai_addr,addrlen) ;
#endif

	} /* end if (send message) */

	if (rs >= 0) {
	    int		c = 0 ;
	    while (rs >= 0) {
	        fromlen = sizeof(sockaddress) ;
	        rs = uc_recvfrome(fd,netbuf,NETBUFLEN,0,&from,&fromlen,to,0) ;
	        netlen = rs ;
	        if (rs < 0) break ;
	        f = isaddrsame(&from,aip->ai_addr) ;
	        if (f) {
	            if (netlen == NETTIMELEN) {
	                f = true ;
	            } else {
	                rs = SR_BADMSG ;
		    }
	            break ;
	        }
	        c += 1 ;
	    } /* end while */
	} /* end if (ok) */

	if ((rs >= 0) && f) {
	    gettimeofday(uap->nep,nullptr) ;
	    memcpy(tsbuf,netbuf,NETTIMELEN) ;
	}

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (nettime_udptryoner) */

#endif /* CF_USEUDP */

static uint64_t gettime_inet(cchar *buf) noex {
	uint64_t	rtime = 0 ;
	if (buf) {
	    uint64_t	net = 0 ;
	    uint64_t	netoff = 2208988800ULL ;
	    const uchar	*ubuf = (const uchar *) buf ;
	    {
	        net = (net << 8) | ubuf[0] ;
	        net = (net << 8) | ubuf[1] ;
	        net = (net << 8) | ubuf[2] ;
	        net = (net << 8) | ubuf[3] ;
	    }
    /* can we extend "nettime" for one or more 136 year chunks? */
	    while (net < NETTIMEROLL) {
	        net += uint64_t(100 * onemillion) ;
	    }
	    rtime = (net - netoff) ;
	} /* end if (non-null) */
	return rtime ;
}
/* end subroutine (gettime_inet) */

static uint64_t utime_timeval(TIMEVAL *tvp) noex {
	uint64_t	r = onemillion ;
	r *= tvp->tv_sec ;
	r += tvp->tv_usec ;
	return r ;
}
/* end subroutine (utime_timeval) */

static uint64_t utime_tcpcalc(uint64_t uti2,uint64_t uti1) noex {
	uint64_t	utid = uti2 - uti1 ;
	uint64_t	r{} ;
	{
	    double	d = utid ;
	    d = ((d * 3) / 4) ;		/* formula for TCP */
	    utid = d ;
	}
	r = uti1 + utid ;
	return r ;
}
/* end subroutine (utime_tcpcalc) */

static uint64_t utime_udpcalc(uint64_t uti2,uint64_t uti1) noex {
	uint64_t	utid = uti2 - uti1 ;
	uint64_t	r{} ;
	{
	    double	d = utid ;
	    d = (d / 2) ;		/* formula for UDP */
	    utid = d ;
	}
	r = uti1 + utid ;
	return r ;
}
/* end subroutine (utime_udpcalc) */

static int tv_loadusec(TIMEVAL *tvp,int64_t uti) noex {
	int		rs = SR_FAULT ;
	if (tvp) {
	    tvp->tv_sec = (uti / onemillion) ;
	    tvp->tv_usec = (uti % onemillion) ;
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (tv_loadusec) */

static int vechand_already(vechand *alp,void *aip) noex {
	int		rs ;
	void		*ep ;
	for (int i = 0 ; (rs = vechand_get(alp,i,&ep)) >= 0 ; i += 1) {
	    if (ep) {
	        if (ep == aip) break ;
	    }
	} /* end for */
	return rs ;
}
/* end subroutine (vechand_already) */

static int connagain(int rs) noex {
	int		f = false ;
	if (rs < 0) {
	    for (int i = 0 ; connagains[i] != 0 ; i += 1) {
	        f = (rs == connagains[i]) ;
	        if (f) break ;
	    } /* end for */
	}
	return f ;
}
/* end subroutine (connagain) */

static int isaddrsame(cvoid *addr1,cvoid *addr2) noex {
	sockaddress	*sa1p = (sockaddress *) addr1 ;
	sockaddress	*sa2p = (sockaddress *) addr2 ;
	int		af1, af2 ;
	int		rs = SR_OK ;
	int		f = false ;

	if (rs >= 0) {
	    rs = sockaddress_getaf(sa1p) ;
	    af1 = rs ;
	}
	if (rs >= 0) {
	    rs = sockaddress_getaf(sa2p) ;
	    af2 = rs ;
	}
	if (rs >= 0) {
	    cint	addrlen = sockaddress_getaddrlen(sa1p) ;
	    f = (af1 == af2) ;
	    if (f) {
	        int	p1 = sockaddress_getport(sa1p) ;
	        int	p2 = sockaddress_getport(sa2p) ;
	        f = (p1 == p2) ;
	    }
	    if (f) {
		char	addrbuf1[ADDRBUFLEN + 1] ;
		char	addrbuf2[ADDRBUFLEN + 1] ;
	        sockaddress_getaddr(sa1p,addrbuf1,ADDRBUFLEN) ;
	        sockaddress_getaddr(sa2p,addrbuf2,ADDRBUFLEN) ;
	        f = (memcmp(addrbuf1,addrbuf2,addrlen) == 0) ;
	    }
	} /* end if (ok) */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (isaddrsame) */

#ifdef	COMMENT

static int mkprintaddr(char *printaddr,int printalen,SOCKADDR *ssap) noex {
	sockaddress	*sap = (sockaddress *) ssap ;
	uint		af ;
	uint		port ;
	uint		flow ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		addrlen = 0 ;
	int		f_flow = false ;
	char		addrbuf[ADDRBUFLEN + 1] ;


	if (rs >= 0) {
	    rs = sockaddress_getlen(sap) ;
	    addrlen = rs ;
	}

	if (rs >= 0) {
	    rs = sockaddress_getaf(sap) ;
	    af = rs ;
	}

	if (rs >= 0) {
	    rs = sockaddress_getport(sap) ;
	    port = rs ;
	}

	if (rs >= 0) {
	    rs1 = sockaddress_getflow(sap,&flow) ;
	    f_flow = (rs1 >= 0) ;
	}

	if (rs >= 0) {
	    rs = sockaddress_getaddr(sap,addrbuf,ADDRBUFLEN) ;
	}

	if (rs >= 0) {
	    sbuf	b ;
	    if ((rs = sbuf_start(&b,printaddr,printalen)) >= 0) {
	        cint	n = (af == AF_INET6) ? INET6ADDRLEN : INET4ADDRLEN ;

	        sbuf_deci(&b,af) ;
	        sbuf_chr(&b,'-') ;

	        sbuf_deci(&b,port) ;
	        sbuf_chr(&b,'-') ;

	        if (f_flow) {
	            sbuf_hexui(&b,flow) ;
	            sbuf_chr(&b,'-') ;
	        }

	        for (int i = 0 ; (rs >= SR_OK) && (i < n) ; i += 1) {
	            if ((i > 0) && ((i & 1) == 0)) {
	                sbuf_chr(&b,':') ;
		    }
		    {
			cint	hlen = 10 ;
		        char	hbuf[10+1] ;
			cuint	v = (addrbuf[i] & UCHAR_MAX) ;
	                if ((rs = cthexui(hbuf,hlen,v)) >= 0) {
	                    rs = sbuf_strw(&b,(hbuf + 6),2) ;
			}
		    }
	        } /* end for */

	        rs1 = sbuf_finish(&b) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (sbuf) */
	} /* end if (ok) */

	return (rs >= 0) ? addrlen : rs ;
}
/* end subroutine (mkprintaddr) */

static int mkprintscope(char *printaddr,int printalen,SOCKADDR *ssap) noex {
	sockaddress	*sap = (sockaddress *) ssap ;
	uint		af ;
	uint		scope ;
	uint		extra ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		addrlen = 0 ;
	bool		f_scope = false ;

	printaddr[0] = '\0' ;

	if (rs >= 0) {
	    rs = sockaddress_getlen(sap) ;
	    addrlen = rs ;
	}

	if (rs >= 0) {
	    rs = sockaddress_getaf(sap) ;
	    af = rs ;
	}

	if (rs >= 0) {
	    rs1 = sockaddress_getscope(sap,&scope) ;
	    f_scope = (rs1 >= 0) ;
	}

	if ((rs >= 0) && f_scope) {
	    rs = sockaddress_getextra(sap,&extra) ;
	}

	if ((rs >= 0) && f_scope) {
	    sbuf	b ;
	    if ((rs = sbuf_start(&b,printaddr,printalen)) >= 0) {
		{
	            sbuf_hexui(&b,scope) ;
	            sbuf_chr(&b,'-') ;
	            sbuf_hexui(&b,extra) ;
		}
	        rs1 = sbuf_finish(&b) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (sbuf) */
	} /* end if (ok) */

	return (rs >= 0) ? addrlen : rs ;
}
/* end subroutine (mkprintscope) */

#endif /* COMMENT */


