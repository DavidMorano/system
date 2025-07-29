/* nettime SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* program to get time from a network time server host */
/* version %I% last-modified %G% */

#define	CF_USEUDP	1		/* use UDP */
#define	CF_UDPMUX	1		/* pretend using UDPMUX */
#define	CF_SOLARIS	1		/* broken Solaris® */
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
	represent the time in seconds since 1 Jan 1900.  We will
	subtract the value "86400 * ((365 * 70) + 17)" (see
	'NETTIMEOFF" below) to get the time in seconds since 1 Jan
	1970 (the UNIX® epoch).

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
#include	<cstring>		/* |memcpy(3c)| */
#include	<netdb.h>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<getprotofamily.h>
#include	<mallocxx.h>
#include	<dialtcp.h>
#include	<hostaddr.h>
#include	<sockaddress.h>
#include	<vechand.h>
#include	<sbuf.h>
#include	<cthex.h>
#include	<sncpyx.h>
#include	<localmisc.h>		/* |TIMEBUFLEN| + |NYEARS_CENTURY| */

#include	"nettime.h"

import libutil ;

/* local defines */

#define	ABEXTRA		20
#define	NETBUFLEN	1024
#define	NETTIMEROLL	3576712516UL	/* roll-over */
#define	NETTIMEOFF	2208988800ULL	/* offset to UNIX® epoch */
#define	EPOCHDIFF	(86400 * ((365 * 70) + 17))
#define	NETTIMELEN	4

#ifndef	INETSVC_TIME
#define	INETSVC_TIME	"time"
#endif

#define	NTRIES		2

#define	UA		struct udpargs
#define	AI		ADDRINFO

#ifndef	CF_USEUDP
#define	CF_USEUDP	1		/* use UDP */
#endif
#ifndef	CF_UDPMUX
#define	CF_UDPMUX	1		/* pretend using UDPMUX */
#endif
#ifndef	CF_SOLARIS
#define	CF_SOLARIS	1		/* broken Solaris */
#endif
#ifndef	CF_CONNECTUDP
#define	CF_CONNECTUDP	0		/* make UDP connection */
#endif


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

namespace {
    struct vars {
	int		maxpathlen ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */

static int	nettime_tcp(nettime *,int,cchar *,cchar *,int) noex ;
static int	nettime_udp(nettime *,int,cchar *,cchar *,int) noex ;
static int	nettime_udptrythem(UA *,char *) noex ;
static int	nettime_udptrysome(UA *,char *,vechand *,hostaddr *,int) noex ;
static int	nettime_udptryone(UA *,char *,AI *) noex ;
static int	nettime_udptryoner(UA *,char *,AI *,int) noex ;

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
} ; /* end array (connagains) */

static vars		var ;

constexpr int		tslen		= TIMEBUFLEN ;
constexpr int		onemillion	= 1000000 ;

constexpr char		inetsvc[]	= INETSVC_TIME ;

constexpr bool		f_useudp	= CF_USEUDP ;
constexpr bool		f_udpmux	= CF_UDPMUX ;
constexpr bool		f_solaris	= CF_SOLARIS ;
constexpr bool		f_connectudp	= CF_CONNECTUDP ;


/* exported variables */


/* exported subroutines */

int getnettime(nettime *ntp,int proto,int af,cc *hostname,cc *svc,int to) noex {
	int		rs = SR_FAULT ;
	int		f_got = false ;
	if (ntp && hostname) {
	    rs = SR_INVALID ;
	    memclear(ntp) ;
	    if ((proto >= 0) && (af >= 0) && hostname[0]) {
	        static cint	rsv = var ;
	        if ((rs = rsv) >= 0) {
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
		} /* end if (var.mkvars) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? f_got : rs ;
}
/* end subroutine (getnettime) */


/* local subroutines */

static int nettime_udp(nettime *ntp,int af,cc *hostname,cc *svc,int to) noex {
	TIMEVAL		netstart ;
	TIMEVAL		netend ;
	int		rs = SR_OK ;
	int		f_got = false ;
	char		tsbuf[tslen + 1] ;
	ntp->proto = IPPROTO_UDP ;
	if_constexpr (f_useudp) {
	    UA	ua{} ;
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
	} else {
	    rs = SR_PROTONOSUPPORT ;
	} /* end if_constexpr (f_useudp) */
	if ((rs >= 0) && f_got) {
	    uint64_t	uti_start = utime_timeval(&netstart) ;
	    uint64_t	uti_end = utime_timeval(&netend) ;
	    uint64_t	uti_inet ;
	    int64_t	uti_off ;
	    {
	        uint64_t	uti_local = utime_udpcalc(uti_end,uti_start) ;
	        uint64_t	t = gettime_inet(tsbuf) ;
	        uti_inet = t * onemillion ;
	        uti_off = (uti_inet - uti_local) ;
	        tv_loadusec(&ntp->off,uti_off) ;
	    }
	    {
	        const uint64_t	uti_trip = (uti_end - uti_start) ;
	        tv_loadusec(&ntp->trip,uti_trip) ;
	    }
	} /* end if */
	return (rs >= 0) ? f_got : rs ;
}
/* end subroutine (nettime_udp) */

static int nettime_tcp(nettime *ntp,int af,cc *hostname,cc *svc,int to) noex {
	TIMEVAL		netstart ;
	TIMEVAL		netend ;
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
	    rs1 = u_close(s) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if */
	/* process network data */
	if ((rs >= 0) && f_got) {
	    uint64_t	uti_start = utime_timeval(&netstart) ;
	    uint64_t	uti_end = utime_timeval(&netend) ;
	    uint64_t	uti_inet ;
	    uint64_t	uti_local ;
	    uint64_t	uti_trip ;
	    int64_t	uti_off ;
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

static int nettime_udptrythem(UA *uap,char *tsbuf) noex {
	AI		hint{} ;
	hostaddr	ha ;
	cint		proto = IPPROTO_UDP ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		f_got = false ;
	uap->proto = proto ;
	hint.ai_protocol = proto ;
	if (uap->af >= 0) {
	    if (cint pf = getprotofamily(uap->af) ; pf >= 0) {
	        uap->pf = pf ;
	        hint.ai_family = pf ;
	    }
	}
	if (vechand alist ; (rs = vechand_start(&alist,2,0)) >= 0) {
	    cchar	*hn = uap->hostname ;
	    if ((rs = hostaddr_start(&ha,hn,uap->svc,&hint)) >= 0) {
		int pf ;
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
	    rs1 = vechand_finish(&alist) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vechand) */
	return (rs >= 0) ? f_got : rs ;
}
/* end subroutine (nettime_udptrythem) */

static int nettime_udptrysome(UA *uap,char *tsbuf,vechand *alp,
		hostaddr *hap,int pf) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		f_got = false ;
	if (hostaddr_cur cur ; (rs = hostaddr_curbegin(hap,&cur)) >= 0) {
	    bool	f{} ;
	    for (AI *aip ; hostaddr_curenum(hap,&cur,&aip) >= 0 ; ) {
	        cint	proto = aip->ai_protocol ;
	        f = ((proto == uap->proto) || (proto <= 0)) ;
	        f = f && ((pf == PF_UNSPEC) || (pf == aip->ai_family)) ;
	        if (f) {
	            rs1 = vechand_already(alp,aip) ;
	            f = (rs1 == SR_NOTFOUND) ;
	        }
	        if (f) {
	            uap->c += 1 ;
	            if ((rs = vechand_add(alp,aip)) >= 0) {
	                rs = nettime_udptryone(uap,tsbuf,aip) ;
	                f_got = (rs > 0) ;
	            }
	            if (f_got) break ;
	        } /* end if (match) */
	    } /* end for */
	    rs1 = hostaddr_curend(hap,&cur) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (cursor) */
	return (rs >= 0) ? f_got : rs ;
}
/* end subroutine (nettime_udptrysome) */

static int nettime_udptryone(UA *uap,char *tsbuf,AI *aip) noex {
	int		rs ;
	int		rs1 ;
	int		pf = aip->ai_family ;
	int		st = aip->ai_socktype ;
	int		proto = uap->proto ;
	int		f = false ; /* return-value */
	uap->ntp->pf = aip->ai_family ;
	if ((rs = u_socket(pf,st,proto)) >= 0) {
	    cint	fd = rs ;
	    if_constexpr (f_connectudp) {
	        rs = u_connect(fd,aip->ai_addr,aip->ai_addrlen) ;
	    }
	    for (int i = 0 ; (rs >= 0) && (i < NTRIES) ; i += 1) {
	        rs = nettime_udptryoner(uap,tsbuf,aip,fd) ;
	        f = (rs > 0) ;
		if_constexpr (f_solaris) {
	            if (f || ((rs < 0) && (! connagain(rs)))) break ;
		} else {
	            if (f || ((rs < 0) && (rs != SR_TIMEDOUT))) break ;
		} /* end if_constexpr (f_solaris) */
	    } /* end for */
	    rs1 = u_close(fd) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (opened socket) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (nettime_udptryone) */

static int nettime_udptryoner(UA *uap,char *tsbuf,AI *aip,int fd) noex {
	int		rs ;
	int		fret = false ;
	if (char *nbuf{} ; (rs = malloc_ml(&nbuf)) >= 0) {
	    cint	nlen = rs ;
	    int		to = uap->to ;
	    sockaddress	from ;
	    int		netlen ;
	    tsbuf[0] = '\0' ;
	    gettimeofday(uap->nsp,nullptr) ;
	    if_constexpr (f_udpmux) {
	        rs = sncpy2(nbuf,nlen,INETSVC_TIME,"\n") ;
	        netlen = rs ;
	    } else {
	        nbuf[0] = '\0' ;
	        netlen = 0 ;
	    } /* end if_constexpr (f_udpmux) */
	    if (rs >= 0) {
	        cint	addrlen = aip->ai_addrlen ;
		if_constexpr (f_connectudp) {
	            rs = u_send(fd,nbuf,netlen,0) ;
		} else {
	            rs = u_sendto(fd,nbuf,netlen,0,aip->ai_addr,addrlen) ;
		} /* end if_constexpr (f_connectudp) */
	    } /* end if (send message) */
	    if (rs >= 0) {
	        int	c = 0 ;
	        while (rs >= 0) {
	            int	fromlen = szof(sockaddress) ;
	            rs = uc_recvfrome(fd,nbuf,nlen,0,&from,&fromlen,to,0) ;
	            netlen = rs ;
	            if (rs < 0) break ;
	            fret = isaddrsame(&from,aip->ai_addr) ;
	            if (fret) {
	                if (netlen == NETTIMELEN) {
	                    fret = true ;
	                } else {
	                    rs = SR_BADMSG ;
		        }
	                break ;
	            }
	            c += 1 ;
	        } /* end while */
	    } /* end if (ok) */
	    if ((rs >= 0) && fret) {
	        gettimeofday(uap->nep,nullptr) ;
	        memcpy(tsbuf,nbuf,nlen) ;
	    }
	    rs = rsfree(rs,nbuf) ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? fret : rs ;
}
/* end subroutine (nettime_udptryoner) */

#endif /* CF_USEUDP */

static uint64_t gettime_inet(cchar *buf) noex {
	uint64_t	rtime = 0 ;
	if (buf) {
	    uint64_t	net = 0 ;
	    uint64_t	netoff = NETTIMEOFF ;
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
	uint64_t	utid = (uti2 - uti1) ;
	uint64_t	r{} ;
	{
	    longdouble	d = cast_static<longdouble>(utid) ;
	    d = ((d * 3) / 4) ;		/* formula for TCP */
	    utid = cast_static<uint64_t>(d) ;
	}
	r = (uti1 + utid) ;
	return r ;
}
/* end subroutine (utime_tcpcalc) */

static uint64_t utime_udpcalc(uint64_t uti2,uint64_t uti1) noex {
	uint64_t	utid = uti2 - uti1 ;
	uint64_t	r{} ;
	{
	    longdouble	d = cast_static<longdouble>(utid) ;
	    d = (d / 2) ;		/* formula for UDP */
	    utid = cast_static<uint64_t>(d) ;
	}
	r = uti1 + utid ;
	return r ;
}
/* end subroutine (utime_udpcalc) */

static int tv_loadusec(TIMEVAL *tvp,int64_t uti) noex {
	int		rs = SR_FAULT ;
	if (tvp) {
	    tvp->tv_sec = (uti / onemillion) ;
	    tvp->tv_usec = cast_static<suseconds_t>(uti % onemillion) ;
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
	int		af1{} ;
	int		af2{} ;
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
		int	ai = 0 ;
		int	sz ;
		cint	abuflen = (var.maxpathlen + ABEXTRA) ;
		sz = ((abuflen + 1) * 2) ;
		if (char *a{} ; (rs = uc_malloc(sz,&a)) >= 0) {
		    {
		        char	*addrbuf1 = (a + ((abuflen + 1) * ai++)) ;
		        char	*addrbuf2 = (a + ((abuflen + 1) * ai++)) ;
	                sockaddress_getaddr(sa1p,addrbuf1,abuflen) ;
	                sockaddress_getaddr(sa2p,addrbuf2,abuflen) ;
	                f = (memcmp(addrbuf1,addrbuf2,addrlen) == 0) ;
		    }
		    rs = rsfree(rs,a) ;
		} /* end if (m-a-f) */
	    }
	} /* end if (ok) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (isaddrsame) */

#ifdef	COMMENT

static int mkprintaddr(char *printaddr,int printalen,SOCKADDR *ssap) noex {
	cint		abuflen = (var.maxpathlen + ABEXTRA) ;
	int		rs ;
	int		rs1 ;
	int		addrlen = 0 ;
	if (char *addrbuf{} ; (rs = uc_malloc((abuflen+1),&addrbuf)) >= 0) {
	    sockaddress	*sap = (sockaddress *) ssap ;
	    uint	af ;
	    uint	port ;
	    uint	flow ;
	    bool	f_flow = false ;
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
	        rs = sockaddress_getaddr(sap,addrbuf,abuflen) ;
	    }
	    if (rs >= 0) {
	        cint	n = (af == AF_INET6) ? INET6ADDRLEN : INET4ADDRLEN ;
	        if (sbuf b ; (rs = b.start(printaddr,printalen)) >= 0) {
	            b.deci(af) ;
	            b.chr('-') ;
	            b.deci(port) ;
	            b.chr('-') ;
	            if (f_flow) {
	                b.hexui(flow) ;
	                b.chr('-') ;
	            }
	            for (int i = 0 ; (rs >= SR_OK) && (i < n) ; i += 1) {
	                if ((i > 0) && ((i & 1) == 0)) {
	                    b.chr(':') ;
		        }
		        {
			    cint	hlen = 10 ;
		            char	hbuf[10+1] ;
			    cuint	v = (addrbuf[i] & UCHAR_MAX) ;
	                    if ((rs = cthexui(hbuf,hlen,v)) >= 0) {
	                        rs = b.strw((hbuf + 6),2) ;
			    }
		        }
	            } /* end for */
	            rs1 = b.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (sbuf) */
	    } /* end if (ok) */
	    rs = rsfree(rs,addrbuf) ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? addrlen : rs ;
}
/* end subroutine (mkprintaddr) */

static int mkprintscope(char *printaddr,int printalen,SOCKADDR *ssap) noex {
	sockaddress	*sap = (sockaddress *) ssap ;
	uint		af = -1 ;
	uint		scope = 0 ;
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
	if ((rs >= 0) && (af == AF_INET6)) {
	    rs = sockaddress_getscope(sap,&scope) ;
	    f_scope = true ;
	}
	if ((rs >= 0) && f_scope) {
	    if (int extra ; (rs = sockaddress_getextra(sap,&extra)) >= 0) {
	        if (sbuf b ; (rs = b.start(printaddr,printalen)) >= 0) {
		    {
	                b.hexui(scope) ;
	                b.chr('-') ;
	                b.hexui(extra) ;
		    }
	            rs1 = b.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (sbuf) */
	    } /* end if (getextra) */
	} /* end if (ok) */
	return (rs >= 0) ? addrlen : rs ;
}
/* end subroutine (mkprintscope) */

#endif /* COMMENT */

vars::operator int () noex {
	int		rs ;
	if ((rs = getbufsize(getbufsize_mp)) >= 0) {
	    maxpathlen = rs ;
	}
	return rs ;
} /* end method (vars::operator) */


