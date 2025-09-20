/* listenspec SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* hold (or manage) a "listen" specification */
/* version %I% last-modified %G% */

#define	CF_OPENPORT	1		/* use 'openport(3dam)' */
#define	CF_NONBLOCK	0		/* set non-blocking on sockets */

/* revision history:

	= 2001-03-23, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	listenspec

	Description:
	This object holds a "listen" specification.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<getnodename.h>
#include	<getproto.h>
#include	<getaddr.h>
#include	<getportnum.h>
#include	<getaf.h>
#include	<acceptpass.h>
#include	<openport.h>
#include	<sockaddress.h>
#include	<vechand.h>
#include	<storebuf.h>
#include	<hostinfo.h>
#include	<mkpr.h>
#include	<strn.h>
#include	<strwcpy.h>
#include	<matxstr.h>
#include	<cfdec.h>
#include	<cfnum.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	<listenconn.h>
#include	<listenpass.h>
#include	<listentcp.h>
#include	<listenudp.h>
#include	<listenusd.h>
#include	<listenuss.h>

#include	"listenspec.h"


/* local defines */

#define	LISTENSPEC_STARTLEN	50		/* starting listenspec length */
#define	LISTENSPEC_DEFPORT	"5108"

#define	LS			listenspec

#ifndef	PROTONAME_TCP
#define	PROTONAME_TCP		"tcp"
#endif

#ifndef	PROTONAME_UDP
#define	PROTONAME_UDP		"udpp"
#endif

#ifndef	PROTONAME_SCTP
#define	PROTONAME_SCTP		"sctp"
#endif

#ifndef	ANYHOST
#define	ANYHOST			"anyhost"
#endif

#ifndef	INETXADDRLEN
#define	INETXADDRLEN		MAX(INET4ADDRLEN,INET6ADDRLEN)
#endif

#define	PRNAME			"LOCAL"

#define	TO_RECVFD		10 /* seconds */

#define	TCPADDR			struct tcpaddr

#define	ARGINFO			struct arginfo

#ifndef	CF_NONBLOCK
#define	CF_NONBLOCK		0
#endif

#if	defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0) 
#define	F_STREAMS	1
#else
#define	F_STREAMS	0
#endif


/* imported namespaces */


/* local typedefs */

typedef mainv		mv ;
typedef SOCKADDR *	sockaddrp ;


/* external subroutines */


/* external variables */


/* local structures */

struct arginfo {
	vechand		pargs ;
	int		slen ;
	int		argvalue ;
	int		mode ;
	int		f_adash ;
} ;

struct ss {
	cchar		*sp ;
	int		sl ;
} ;

struct tcpaddr {
	struct ss	af, host, port ;
} ;


/* forward references */

template<typename ... Args>
static int listenspec_ctor(listenspec *op,Args ... args) noex {
    	LISTENSPEC	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (listenspec_ctor) */

static int listenspec_dtor(listenspec *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (listenspec_dtor) */

template<typename ... Args>
static inline int listenspec_magic(listenspec *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == LISTENSPEC_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (listenspec_magic) */

static int	listenspec_parse(LS *,int,mainv) noex ;

static int	listenspec_tcpbegin(LS *,int,mainv) noex ;
static int	listenspec_tcpaddrload(LS *,TCPADDR *,int) noex ;
static int	listenspec_tcpend(LS *) noex ;
static int	listenspec_tcpsame(LS *,LISTENSPEC *) noex ;
static int	listenspec_tcpactive(LS *,int,int) noex ;
static int	listenspec_tcpaccept(LS *,void *,int *,int) noex ;
static int	listenspec_tcpinfo(LS *,listenspec_info *) noex ;

static int	listenspec_ussbegin(LS *,int,mainv) noex ;
static int	listenspec_ussend(LS *) noex ;
static int	listenspec_usssame(LS *,LISTENSPEC *) noex ;
static int	listenspec_ussactive(LS *,int,int) noex ;
static int	listenspec_ussaccept(LS *,void *,int *,int) noex ;
static int	listenspec_ussinfo(LS *,listenspec_info *) noex ;

static int	listenspec_passbegin(LS *,int,mainv) noex ;
static int	listenspec_passend(LS *) noex ;
static int	listenspec_passsame(LS *,LISTENSPEC *) noex ;
static int	listenspec_passactive(LS *,int,int) noex ;
static int	listenspec_passaccept(LS *,void *,int *,int) noex ;
static int	listenspec_passinfo(LS *,listenspec_info *) noex ;

static int	listenspec_connbegin(LS *,int,mainv) noex ;
static int	listenspec_connend(LS *) noex ;
static int	listenspec_connsame(LS *,LISTENSPEC *) noex ;
static int	listenspec_connactive(LS *,int,int) noex ;
static int	listenspec_connaccept(LS *,void *,int *,int) noex ;
static int	listenspec_conninfo(LS *,listenspec_info *) noex ;

static int	listenspec_procargs(LS *,ARGINFO *,int,mainv) noex ;

#if	CF_OPENPORT
static int	listenspec_openport(LS *,int,cchar *,cchar *,int) noex ;
static int	listenspec_openporter(LS *,cchar *,int,
			cchar *,int,int) noex ;
static int	listenspec_openportaddr(LS *,cchar *,
			ADDRINFO *,char *,int,cchar *) noex ;
static int	listenspec_openportao(LS *,char *,int,cc *) noex ;
#endif /* CF_OPENPORT */

static int	listenspec_prlocal(LS *) noex ;

#ifdef	COMMENT
static int	tcp_nfield(cchar *,int) noex ;
#endif

static int	tcpaddr_load(TCPADDR *,cchar *,int) noex ;

static int	arginfo_start(ARGINFO *) noex ;
static int	arginfo_add(ARGINFO *,cchar *) noex ;
static int	arginfo_get(ARGINFO *,int,cchar **) noex ;
static int	arginfo_finish(ARGINFO *) noex ;


/* local variables */

enum ltypes {
	ltype_none,
	ltype_tcp,
	ltype_uss,
	ltype_pass,
	ltype_conn,
	ltype_overlast
} ;

constexpr cpcchar	ltypes[] = {
	"none",
	"tcp",
	"uss",
	"pass",
	"conn",
	nullptr
} ;

enum lopts {
	lopt_here,
	lopt_reuse,
	lopt_ra,
	lopt_overlast
} ;

constexpr cpcchar	lopts[] = {
	"here",
	"reuse",
	"ra",
	nullptr
} ;

constexpr bool		f_nonblock = CF_NONBLOCK ;
constexpr bool		f_streams = F_STREAMS ;


/* exported variables */


/* exported subroutines */

int listenspec_start(LS *op,int ac,mv av) noex {
	int		rs ;
	int		n = 0 ;
	if ((rs = listenspec_ctor(op,av)) >= 0) {
	    rs = SR_INVALID ;
	    if (ac > 0) {
	        memclear(op) ;
	        if ((rs = listenspec_parse(op,ac,av)) >= 0) {
	            n = rs ;
	            op->magic = LISTENSPEC_MAGIC ;
	        }
	    } /* end if (valid) */
	    if (rs < 0) {
		listenspec_dtor(op) ;
	    }
	} /* end if (listenspec_start) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (listenspec_start) */

int listenspec_finish(LS *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = listenspec_magic(op)) >= 0) {
	    if (op->fl.active) {
	        rs1 = listenspec_active(op,0,false) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->info) {
	        rs1 = 0 ;
	        switch (op->ltype) {
	        case ltype_tcp:
	            rs1 = listenspec_tcpend(op) ;
	            break ;
	        case ltype_uss:
	            rs1 = listenspec_ussend(op) ;
	            break ;
	        case ltype_pass:
	            rs1 = listenspec_passend(op) ;
	            break ;
	        case ltype_conn:
	            rs1 = listenspec_connend(op) ;
	            break ;
	        } /* end switch */
	        if (rs >= 0) rs = rs1 ;
	        rs1 = uc_free(op->info) ;
	        if (rs >= 0) rs = rs1 ;
	        op->info = nullptr ;
	    } /* end if */
	    if (op->prlocal) {
	        rs1 = uc_free(op->prlocal) ;
	        if (rs >= 0) rs = rs1 ;
	        op->prlocal = nullptr ;
	    }
	    {
	        rs1 = listenspec_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->ltype = 0 ;
	    op->magic = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (listenspec_finish) */

int listenspec_issame(LS *op,LISTENSPEC *otherp) noex {
	int		rs ;
	int		f = false ; /* return-value */
	if ((rs = listenspec_magic(op,otherp)) >= 0) {
	    f = (op->ltype == otherp->ltype) ;
	    if (f) {
	        switch (op->ltype) {
	        case ltype_tcp:
	            rs = listenspec_tcpsame(op,otherp) ;
	            f = (rs > 0) ;
	            break ;
	        case ltype_uss:
	            rs = listenspec_usssame(op,otherp) ;
	            f = (rs > 0) ;
	            break ;
	        case ltype_pass:
	            rs = listenspec_passsame(op,otherp) ;
	            f = (rs > 0) ;
	            break ;
	        case ltype_conn:
	            rs = listenspec_connsame(op,otherp) ;
	            f = (rs > 0) ;
	            break ;
	        default:
	            rs = SR_DOM ;
		    break ;
	        } /* end switch */
	    } /* end if */
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (listenspec_issame) */

int listenspec_active(LS *op,int opts,int f) noex {
	int		rs ;
	int		f_previous = false ;
	if ((rs = listenspec_magic(op)) >= 0) {
	    f_previous = op->fl.active ;
	    switch (op->ltype) {
	    case ltype_tcp:
	        rs = listenspec_tcpactive(op,opts,f) ;
	        break ;
	    case ltype_uss:
	        rs = listenspec_ussactive(op,opts,f) ;
	        break ;
	    case ltype_pass:
	        rs = listenspec_passactive(op,opts,f) ;
	        break ;
	    case ltype_conn:
	        rs = listenspec_connactive(op,opts,f) ;
	        break ;
	    default:
	        rs = SR_DOM ;
	        break ;
	    } /* end switch */
	    /* if we just activated (rs > 0), then set Close-On-Exec */
	    if ((rs > 0) && (op->fd >= 0) && op->fl.active) {
	        rs = uc_closeonexec(op->fd,true) ;
	        if_constexpr (f_nonblock) {
	            if (rs >= 0) {
	                rs = uc_nonblock(op->fd,true) ;
	            }
	        } /* end if_constexpr (f_nonblock) */
	        if ((rs < 0) && f) {
	            listenspec_active(op,opts,false) ;
	        }
	    } /* end if (just activated) */
	} /* end if (magic) */
	return (rs >= 0) ? f_previous : rs ;
}
/* end subroutine (listenspec_active) */

int listenspec_isactive(LS *op) noex {
	int		rs ;
	int		f = false ;
	if ((rs = listenspec_magic(op)) >= 0) {
	    f = op->fl.active ;
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (listenspec_isactive) */

int listenspec_delset(LS *op,int f) noex {
	int		rs ;
	if ((rs = listenspec_magic(op)) >= 0) {
	    op->fl.delmark = f ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (listenspec_delset) */

int listenspec_delmarked(LS *op) noex {
	int		rs ;
	int		f = false ;
	if ((rs = listenspec_magic(op)) >= 0) {
	    f = op->fl.delmark ;
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (listenspec_delmarked) */

int listenspec_getfd(LS *op) noex {
	int		rs ;
	if ((rs = listenspec_magic(op)) >= 0) {
	    rs = (op->fl.active) ? op->fd : SR_BADFD ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (listenspec_getfd) */

int listenspec_gettype(LS *op) noex {
	int		rs ;
	if ((rs = listenspec_magic(op)) >= 0) {
	    rs = op->ltype ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (listenspec_gettype) */

int listenspec_accept(LS *op,void *fromp,int *fromlenp,int to) noex {
	int		rs ;
	if ((rs = listenspec_magic(op)) >= 0) {
	    if (op->fl.active) {
	        switch (op->ltype) {
	        case ltype_tcp:
	            rs = listenspec_tcpaccept(op,fromp,fromlenp,to) ;
	            break ;
	        case ltype_uss:
	            rs = listenspec_ussaccept(op,fromp,fromlenp,to) ;
	            break ;
	        case ltype_pass:
	            rs = listenspec_passaccept(op,fromp,fromlenp,to) ;
	            break ;
	        case ltype_conn:
	            rs = listenspec_connaccept(op,fromp,fromlenp,to) ;
	            break ;
	        default:
	            rs = SR_DOM ;
	            break ;
	        } /* end switch */
	    } else {
	        rs = SR_BADFD ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (listenspec_accept) */

int listenspec_getinfo(LS *op,listenspec_info *lip) noex {
	int		rs ;
	if ((rs = listenspec_magic(op,lip)) >= 0) {
	    lip->type[0] = '\0' ;
	    lip->addr[0] = '\0' ;
	    switch (op->ltype) {
	    case ltype_tcp:
	        rs = listenspec_tcpinfo(op,lip) ;
	        break ;
	    case ltype_uss:
	        rs = listenspec_ussinfo(op,lip) ;
	        break ;
	    case ltype_pass:
	        rs = listenspec_passinfo(op,lip) ;
	        break ;
	    case ltype_conn:
	        rs = listenspec_conninfo(op,lip) ;
	        break ;
	    default:
	        rs = SR_INVALID ;
	        break ;
	    } /* end switch */
	    if (rs >= 0) {
	        lip->state = 0 ;
	        if (op->fl.active) lip->state |= LISTENSPEC_MACTIVE ;
	        if (op->fl.delmark) lip->state |= LISTENSPEC_MDELPEND ;
	        if (op->fl.broken) lip->state |= LISTENSPEC_MBROKEN ;
	        strwcpy(lip->type,ltypes[op->ltype],LISTENSPEC_TYPELEN) ;
	    } /* end if */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (listenspec_info) */

int listenspec_geterr(LS *op,int *rp) noex {
	int		rs ;
	if ((rs = listenspec_magic(op)) >= 0) {
	    if (rp) {
	        *rp = op->rs_error ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (listenspec_geterr) */

int listenspec_clear(LS *op) noex {
	int		rs ;
	if ((rs = listenspec_magic(op)) >= 0) {
	    op->fl.broken = false ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (listenspec_clear) */


/* private subroutines */

static int listenspec_parse(LS *op,int ac,mv av) noex {
	cint		ti = matstr(ltypes,av[0],-1) ;
	int		rs = SR_OK ;
	int		sz = 0 ;
	int		n = 0 ;
	op->ltype = ti ;
	ac -= 1 ;
	/* allocate our information structure */
	switch (ti) {
	case ltype_tcp:
	    sz = szof(LISTENSPEC_TCP) ;
	    break ;
	case ltype_uss:
	    sz = szof(LISTENSPEC_USS) ;
	    break ;
	case ltype_pass:
	    sz = seof(LISTENSPEC_PASS) ;
	    break ;
	case ltype_conn:
	    sz = szof(LISTENSPEC_CONN) ;
	    break ;
	} /* end switch */
	if (sz > 0) {
	    void	*p ;
	    if ((rs = uc_malloc(sz,&p)) >= 0) {
	        memset(p,0,sz) ;
	        op->info = p ;
	        switch (ti) {
	        case ltype_tcp:
	            rs = listenspec_tcpbegin(op,ac,(av+1)) ;
	            n = rs ;
	            break ;
	        case ltype_uss:
	            rs = listenspec_ussbegin(op,ac,(av+1)) ;
	            n = rs ;
	            break ;
	        case ltype_pass:
	            rs = listenspec_passbegin(op,ac,(av+1)) ;
	            n = rs ;
	            break ;
	        case ltype_conn:
	            rs = listenspec_connbegin(op,ac,(av+1)) ;
	            n = rs ;
	            break ;
	        default:
	            rs = SR_BUGCHECK ;
	            break ;
	        } /* end switch */
	        if (rs < 0) {
	            uc_free(op->info) ;
	            op->info = nullptr ;
	        }
	    } /* end if (memory-allocation) */
	} else {
	    rs = SR_PROTONOSUPPORT ;
	}
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (listenspec_parse) */

static int listenspec_tcpbegin(LS *op,int ac,mv av) noex {
	TCPADDR		a ;
	ARGINFO		ai, *aip = &ai ;
	int		rs ;
	int		rs1 ;
	int		n = 0 ;
	tcpaddr_load(&a,nullptr,0) ;
	if ((rs = arginfo_start(aip)) >= 0) {
	    if ((rs = listenspec_procargs(op,aip,ac,av)) >= 0) {
		int	pan = 0 ;
		int	al, cl ;
		int	sz = 0 ;
		cchar	*ap ;
		cchar	*cp ;
	        while ((al = arginfo_get(aip,pan,&ap)) >= 0) {
	            switch (pan) {
	            case 0:
	                if (al > 0) {
	                    sz = al ;
	                    n = tcpaddr_load(&a,ap,al) ;
	                    if ((n > 0) && (a.port.sp == nullptr))
	                        n = 0 ;
	                }
	                if (n == 0) {
	                    cp = LISTENSPEC_DEFPORT ;
	                    cl = lenstr(cp) ;
	                    sz = (cl + 1) ;
	                    a.port.sp = cp ;
	                    a.port.sl = cl ;
	                    n = 1 ;
	                }
	                break ;
	            } /* end switch */
	            pan += 1 ;
	        } /* end while (positional arguments) */
	        if (n > 0) {
	            rs = listenspec_tcpaddrload(op,&a,sz) ;
	        }
	    } /* end if (procargs) */
	    rs1 = arginfo_finish(aip) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (arginfo) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (listenspec_tcpbegin) */

static int listenspec_tcpaddrload(LS *op,TCPADDR *ap,int sz) noex {
	LISTENSPEC_TCP	*ip = (LISTENSPEC_TCP *) op->info ;
	int		rs ;
	char		*abuf = nullptr ;
	char		*abp ;

	if ((rs = uc_malloc(sz,&abuf)) >= 0) {

	    ip->a = abuf ;
	    abp = abuf ;
	    if (ap->af.sp != nullptr) {
	        ip->af = abp ;
	        abp = strwcpy(abp,ap->af.sp,ap->af.sl) + 1 ;
	    }

	    if (ap->host.sp != nullptr) {
	        ip->host = abp ;
	        abp = strwcpy(abp,ap->host.sp,ap->host.sl) + 1 ;
	    }

	    if (ap->port.sp != nullptr) {
	        ip->port = abp ;
	        abp = strwcpy(abp,ap->port.sp,ap->port.sl) + 1 ;
	    }

	} /* end if (memory-allocation) */

	return rs ;
}
/* end subroutine (listenspec_tcpaddrload) */

static int listenspec_tcpend(LS *op) noex {
	LISTENSPEC_TCP	*ip = (LISTENSPEC_TCP *) op->info ;
	int		rs = SR_OK ;
	int		rs1 ;

	if (ip->a != nullptr) {
	    rs1 = uc_free(ip->a) ;
	    if (rs >= 0) rs = rs1 ;
	    ip->a = nullptr ;
	}

	return rs ;
}
/* end subroutine (listenspec_tcpend) */

static int listenspec_tcpsame(LS *op,LISTENSPEC *otherp) noex {
	LISTENSPEC_TCP	*ip = (LISTENSPEC_TCP *) op->info ;
	LISTENSPEC_TCP	*oip ;
	int		rs = SR_OK ;
	int		f = true ;

	oip = (LISTENSPEC_TCP *) otherp->info ;
	f = f && LEQUIV((ip->af != nullptr),(oip->af != nullptr)) ;
	if (f && (ip->af != nullptr))
	    f = (strcmp(ip->af,oip->af) == 0) ;

	f = f && LEQUIV((ip->host != nullptr),(oip->host != nullptr)) ;
	if (f && (ip->host != nullptr))
	    f = (strcmp(ip->host,oip->host) == 0) ;

	f = f && LEQUIV((ip->port != nullptr),(oip->port != nullptr)) ;
	if (f && (ip->port != nullptr))
	    f = (strcmp(ip->port,oip->port) == 0) ;

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (listenspec_tcpsame) */

/* f=1 make-active, f=0 make inactive */
static int listenspec_tcpactive(LS *op,int opts,int f) noex {
	int		rs = SR_OK ;
	int		f_a = false ;

	if (f && (! op->fl.active)) {
	    LISTENSPEC_TCP *ip = (LISTENSPEC_TCP *) op->info ;
	    int		af ;
	    cchar	*cp ;
	    cchar	*hostspec ;
	    cchar	*portspec ;

/* address family */

	    cp = ip->af ;
	    af = AF_INET4 ; /* default when none was specified */
	    if (cp != nullptr) {
	        rs = getaf(cp,-1) ;
	        af = rs ;
	    }

/* hostname */

	    hostspec = ip->host ;
	    if (hostspec == nullptr) hostspec = "" ;

/* port */

	    portspec = ip->port ;
	    if (portspec == nullptr) portspec = LISTENSPEC_DEFPORT ;

/* activate */

	    if (rs >= 0) {
	        int	lopts = (opts & LISTENSPEC_MREUSE) ? 1 : 0 ;
	        rs = listentcp(af,hostspec,portspec,lopts) ;
	        op->fd = rs ;
#if	CF_OPENPORT
	        if (rs == SR_ACCESS) {
	            rs = listenspec_openport(op,af,hostspec,portspec,opts) ;
	            op->fd = rs ;
	        }
#endif /* CF_OPENPORT */
	    } /* end if */

	    if (rs >= 0) {
	        op->fl.active = true ;
	        op->fl.broken = false ;
		f_a = true ;
	    } else {
		op->rs_error = rs ;
	        op->fl.broken = true ;
	    }
	} else if ((! f) && op->fl.active) {
	    if (op->fd >= 0) {
	        u_close(op->fd) ;
	        op->fd = -1 ;
	    }
	    op->fl.active = false ;
	} /* end if */

	return (rs >= 0) ? f_a : rs ;
}
/* end subroutine (listenspec_tcpactive) */

static int listenspec_tcpaccept(LS *op,void *fp,int *flp,int to) noex {
	SOCKADDR	*sap = sockaddrp(fp) ;
	return uc_accepte(op->fd,sap,flp,to) ;
}
/* end subroutine (listenspec_tcpaccept) */

static int listenspec_tcpinfo(LS *op,listenspec_info *lip) noex {
	LISTENSPEC_TCP	*ip = (LISTENSPEC_TCP *) op->info ;
	int		rs = SR_OK ;
	int		bl = LISTENSPEC_ADDRLEN ;
	int		i = 0 ;
	char		*bp = lip->addr ;
	if ((rs >= 0) && (ip->af != nullptr)) {
	    rs = storebuf_strw(bp,bl,i,ip->af,-1) ;
	    i += rs ;
	    if (rs >= 0) {
	        rs = storebuf_chr(bp,bl,i,':') ;
	        i += rs ;
	    }
	}
	if ((rs >= 0) && (ip->host != nullptr)) {
	    rs = storebuf_strw(bp,bl,i,ip->host,-1) ;
	    i += rs ;
	    if (rs >= 0) {
	        rs = storebuf_chr(bp,bl,i,':') ;
	        i += rs ;
	    }
	}
	if ((rs >= 0) && (ip->port != nullptr)) {
	    rs = storebuf_strw(bp,bl,i,ip->port,-1) ;
	    i += rs ;
	}
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (listenspec_tcpinfo) */

static int listenspec_ussbegin(LS *op,int ac,mv av) noex {
	LISTENSPEC_USS	*ip = (LISTENSPEC_USS *) op->info ;
	ARGINFO		ai, *aip = &ai ;
	int		rs ;
	int		rs1 ;
	int		n = 0 ;

/* initialize */

	ip->mode = -1 ;

	if ((rs = arginfo_start(aip)) >= 0) {
	    if ((rs = listenspec_procargs(op,aip,ac,av)) >= 0) {
		int	pan = 0 ;
		int	al ;
		cchar	*ap ;
		cchar	*cp ;

	        if (aip->mode >= 0)
	            ip->mode = aip->mode ;

	        while ((al = arginfo_get(aip,pan,&ap)) >= 0) {

	            switch (pan) {
	            case 0:
	                if (al > 0) {
	                    n = 1 ;
	                    rs = uc_mallocstrw(ap,al,&cp) ;
	                    if (rs >= 0) ip->fname = cp ;
	                }
	                break ;
	            } /* end switch */
	            pan += 1 ;

	            if (rs < 0) break ;
	        } /* end while (positional arguments) */

/* default arguments */

	        if (ip->mode < 0) {
	            ip->mode = 0666 ;
		}

	    } /* end if (procargs) */
	    rs1 = arginfo_finish(aip) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (arginfo) */

	return (rs >= 0) ? n : rs ;
}
/* end subroutine (listenspec_ussbegin) */

static int listenspec_ussend(LS *op) noex {
	LISTENSPEC_USS	*ip = (LISTENSPEC_USS *) op->info ;
	int		rs = SR_OK ;
	int		rs1 ;
	if (ip->fname != nullptr) {
	    rs1 = uc_free(ip->fname) ;
	    if (rs >= 0) rs = rs1 ;
	    ip->fname = nullptr ;
	}
	return rs ;
}
/* end subroutine (listenspec_ussend) */

static int listenspec_usssame(LS *op,LISTENSPEC *otherp) noex {
	LISTENSPEC_USS	*ip = (LISTENSPEC_USS *) op->info ;
	LISTENSPEC_USS	*oip ;
	int		rs = SR_OK ;
	int		f = true ;

	oip = (LISTENSPEC_USS *) otherp->info ;
	f = f && LEQUIV((ip->fname != nullptr),(oip->fname != nullptr)) ;
	if (f && (ip->fname != nullptr))
	    f = (strcmp(ip->fname,oip->fname) == 0) ;

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (listenspec_usssame) */

static int listenspec_ussactive(LS *op,int opts,int f) noex {
	LISTENSPEC_USS	*ip = (LISTENSPEC_USS *) op->info ;
	int		rs = SR_OK ;
	int		f_a = false ;

	if (f && (! op->fl.active)) {
	    if ((rs = listenuss(ip->fname,ip->mode,opts)) >= 0) {
	        op->fd = rs ;
	        op->fl.active = true ;
	        op->fl.broken = false ;
		f_a = true ;
	    } else {
		op->rs_error = rs ;
	        op->fl.broken = true ;
	    }
	} else if ((! f) && op->fl.active) {
	    if (op->fd >= 0) {
	        u_close(op->fd) ;
	        op->fd = -1 ;
	    }
	    op->fl.active = false ;
	} /* end if */

	return (rs >= 0) ? f_a : rs ;
}
/* end subroutine (listenspec_ussactive) */

static int listenspec_ussaccept(LS *op,void *fp,int *flp,int to) noex {
	SOCKADDR	*sap = sockaddrp(fp) ;
	return uc_accepte(op->fd,sap,flp,to) ;
}
/* end subroutine (listenspec_ussaccept) */

static int listenspec_ussinfo(LS *op,listenspec_info *lip) noex {
	LISTENSPEC_USS	*ip = (LISTENSPEC_USS *) op->info ;
	int		rs = SR_OK ;
	int		bl = LISTENSPEC_ADDRLEN ;
	int		len = 0 ;
	char		*bp = lip->addr ;

	if (ip->fname == nullptr) rs = SR_NOENT ;

	if (rs >= 0) {
	    len = strwcpy(bp,ip->fname,bl) - bp ;
	}

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (listenspec_ussinfo) */

static int listenspec_passbegin(LS *op,int ac,mv av) noex {
	LISTENSPEC_PASS	*ip = (LISTENSPEC_PASS *) op->info ;
	ARGINFO		ai, *aip = &ai ;
	int		rs ;
	int		rs1 ;
	int		n = 0 ;

	ip->mode = -1 ;

	if ((rs = arginfo_start(aip)) >= 0) {
	    if ((rs = listenspec_procargs(op,aip,ac,av)) >= 0) {
		int	pan = 0 ;
		int	al ;
		cchar	*ap ;
		cchar	*cp ;

/* option arguments */

	        if (aip->mode >= 0)
	            ip->mode = aip->mode ;

/* positional arguments */

	        pan = 0 ;
	        while ((al = arginfo_get(aip,pan,&ap)) >= 0) {

	            switch (pan) {
	            case 0:
	                if (al > 0) {
	                    n = 1 ;
	                    rs = uc_mallocstrw(ap,al,&cp) ;
	                    if (rs >= 0) ip->fname = cp ;
	                }
	                break ;
	            } /* end switch */
	            pan += 1 ;

	            if (rs < 0) break ;
	        } /* end while (positional arguments) */

/* default arguments */

	        if (ip->mode < 0) {
	            ip->mode = 0666 ;
		}

	    } /* end if (procargs) */
	    rs1 = arginfo_finish(aip) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (arginfo) */

	return (rs >= 0) ? n : rs ;
}
/* end subroutine (listenspec_passbegin) */

static int listenspec_passend(LS *op) noex {
	LISTENSPEC_PASS	*ip = (LISTENSPEC_PASS *) op->info ;
	int		rs = SR_OK ;
	int		rs1 ;

	if (ip->fname != nullptr) {
	    rs1 = uc_free(ip->fname) ;
	    if (rs >= 0) rs = rs1 ;
	    ip->fname = nullptr ;
	}

	return rs ;
}
/* end subroutine (listenspec_passend) */

static int listenspec_passsame(LS *op,LISTENSPEC *otherp) noex {
	LISTENSPEC_PASS	*ip = (LISTENSPEC_PASS *) op->info ;
	LISTENSPEC_PASS	*oip = (LISTENSPEC_PASS *) otherp->info ;
	int		rs = SR_OK ;
	int		f = true ;

	f = f && LEQUIV((ip->fname != nullptr),(oip->fname != nullptr)) ;
	if (f && (ip->fname != nullptr)) {
	    f = (strcmp(ip->fname,oip->fname) == 0) ;
	}

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (listenspec_passsame) */

static int listenspec_passactive(LS *op,int opts,int f) noex {
	LISTENSPEC_PASS	*ip = (LISTENSPEC_PASS *) op->info ;
	int		rs = SR_OK ;
	int		f_a = false ;
	if (f && (! op->fl.active)) {
	    if ((rs = listenpass(ip->fname,ip->mode,opts)) >= 0) {
	        op->fd = rs ;
	        op->fl.active = true ;
	        op->fl.broken = false ;
		f_a = true ;
		if_constexpr (f_streams) {
	            rs = u_ioctl(op->fd,I_SRDOPT,RMSGD) ;
		}
	    } else {
		op->rs_error = rs ;
	        op->fl.broken = true ;
	    }
	} else if ((! f) && op->fl.active) {
	    if (op->fd >= 0) {
	        u_close(op->fd) ;
	        op->fd = -1 ;
	    }
	    op->fl.active = false ;
	} /* end if */
	return (rs >= 0) ? f_a : rs ;
}
/* end subroutine (listenspec_passactive) */

static int listenspec_passaccept(LS *op,void *fp,int *flp,int to) noex {
	int		rs ;
	int		fd = -1 ;
	if (fp) {
	    *flp = szof(long) ;
	    memset(fp,0,*flp) ;
	}
	{
	    rs = acceptpass(op->fd,nullptr,to) ;
	    fd = rs ;
	}
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (listenspec_passaccept) */

static int listenspec_passinfo(LS *op,listenspec_info *lip) noex {
	LISTENSPEC_PASS	*ip = (LISTENSPEC_PASS *) op->info ;
	int		rs = SR_OK ;
	int		bl = LISTENSPEC_ADDRLEN ;
	int		len = 0 ;
	char		*bp = lip->addr ;

	if (ip->fname == nullptr) rs = SR_NOENT ;

	if (rs >= 0) {
	    len = strwcpy(bp,ip->fname,bl) - bp ;
	}

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (listenspec_passinfo) */

static int listenspec_connbegin(LS *op,int ac,mv av) noex {
	LISTENSPEC_CONN	*ip = (LISTENSPEC_CONN *) op->info ;
	ARGINFO		ai, *aip = &ai ;
	int		rs ;
	int		rs1 ;
	int		n = 0 ;

	ip->mode = -1 ;

	if ((rs = arginfo_start(aip)) >= 0) {
	    if ((rs = listenspec_procargs(op,aip,ac,av)) >= 0) {
		int	pan = 0 ;
		int	al ;
		cchar	*ap ;
		cchar	*cp ;

/* option arguments */

	        if (aip->mode >= 0)
	            ip->mode = aip->mode ;

/* positional arguments */

	        while ((al = arginfo_get(aip,pan,&ap)) >= 0) {

	            switch (pan) {
	            case 0:
	                if (al > 0) {
	                    n = 1 ;
	                    rs = uc_mallocstrw(ap,al,&cp) ;
	                    if (rs >= 0) ip->fname = cp ;
	                }
	                break ;
	            } /* end switch */
	            pan += 1 ;

	            if (rs < 0) break ;
	        } /* end while (positional arguments) */

/* default arguments */
	        if (ip->mode < 0) {
	            ip->mode = 0666 ;
		}
	    } /* end if (procargs) */
	    rs1 = arginfo_finish(aip) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (arginfo) */

	return (rs >= 0) ? n : rs ;
}
/* end subroutine (listenspec_connbegin) */

static int listenspec_connend(LS *op) noex {
	LISTENSPEC_CONN	*ip = (LISTENSPEC_CONN *) op->info ;
	int		rs = SR_OK ;
	int		rs1 ;

	if (ip->fname != nullptr) {
	    rs1 = uc_free(ip->fname) ;
	    if (rs >= 0) rs = rs1 ;
	    ip->fname = nullptr ;
	}

	return rs ;
}
/* end subroutine (listenspec_connend) */

static int listenspec_connsame(LS *op,LISTENSPEC *otherp) noex {
	LISTENSPEC_CONN	*ip = (LISTENSPEC_CONN *) op->info ;
	LISTENSPEC_CONN	*oip = (LISTENSPEC_CONN *) otherp->info ;
	int		rs = SR_OK ;
	int		f = true ;

	f = f && LEQUIV((ip->fname != nullptr),(oip->fname != nullptr)) ;
	if (f && (ip->fname != nullptr)) {
	    f = (strcmp(ip->fname,oip->fname) == 0) ;
	}

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (listenspec_connsame) */

static int listenspec_connactive(LS *op,int opts,int f) noex {
	LISTENSPEC_CONN	*ip = (LISTENSPEC_CONN *) op->info ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		f_a = false ;

	if (f && (! op->fl.active)) {
	    if ((rs = listenconn(ip->fname,ip->mode,opts)) >= 0) {
	        op->fd = rs ;
	        op->fl.active = true ;
	        op->fl.broken = false ;
		f_a = true ;
#if	SYSHAS_STREAMS
	        u_ioctl(op->fd,I_SRDOPT,RMSGD) ;
#endif
	    } else {
		op->rs_error = rs ;
	        op->fl.broken = true ;
	    }
	} else if ((! f) && op->fl.active) {
	    if (ip->fname[0] != '\0') {
	        rs1 = uc_fdetach(ip->fname) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    if (op->fd >= 0) {
	        u_close(op->fd) ;
	        op->fd = -1 ;
	    }
	    op->fl.active = false ;
	} /* end if */

	return (rs >= 0) ? f_a : rs ;
}
/* end subroutine (listenspec_connactive) */

static int listenspec_connaccept(LS *op,void *fp,int *flp,int to) noex {
	int		rs ;
	int		fd = -1 ;
	if (fp) {
	    *flp = szof(long) ;
	    memset(fp,0,*flp) ;
	}
	{
	    rs = acceptpass(op->fd,nullptr,to) ;
	    fd = rs ;
	}
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (listenspec_connaccept) */

static int listenspec_conninfo(LS *op,listenspec_info *lip) noex {
	LISTENSPEC_CONN	*ip = (LISTENSPEC_CONN *) op->info ;
	int		rs = SR_NOTFOUND ;
	int		len = 0 ;
	if (ip->fname) {
	    int		bl = LISTENSPEC_ADDRLEN ;
	    char	*bp = lip->addr ;
	    rs = SR_OK ;
	    len = strwcpy(bp,ip->fname,bl) - bp ;
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (listenspec_conninfo) */

static int listenspec_procargs(LS *op,ARGINFO *aip,int ac,mv av) noex {
	uint		uv ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		fl ;
	int		v ;
	int		ai, ar ;
	int		kwi ;
	int		akl, avl ;
	int		pan = 0 ;
	int		f_optminus, f_optplus, f_optequal ;
	cchar		*akp, *avp ;
	cchar		*fp ;
	aip->mode = -1 ;
	/* allocate buffer for further parsing */
	ar = ac ;
	for (ai = 0 ; (ai < ac) && (av[ai] != nullptr) ; ai += 1) {
	    ar -= 1 ;
	    fp = av[ai] ;
	    fl = lenstr(fp) ;
	    f_optminus = (fp[0] == '-') ;
	    f_optplus = (fp[0] == '+') ;
	    if ((fl > 1) && (f_optminus || f_optplus) && (! aip->f_adash)) {
		cint	ach = mkchar(fp[1]) ;
	        if (isdigitlatin(ach)) {
	            rs1 = cfdecti((fp + 1),(fl - 1),&v) ;
	            if (rs1 >= 0) aip->argvalue = v ;
	        } else if (ach == '-') {
	            aip->f_adash = true ;
	            break ;
	        } else {
	            akp = (fp + 1) ;
	            f_optequal = false ;
	            if ((avp = strchr(akp,'=')) != nullptr) {
	                f_optequal = true ;
	                akl = avp - akp ;
	                avp += 1 ;
	                avl = akp + (fl - 1) - avp ;
	            } else {
	                avp = nullptr ;
	                avl = 0 ;
	                akl = (fl - 1) ;
	            }
	            if ((kwi = matostr(lopts,2,akp,akl)) >= 0) {
	                switch (kwi) {
	                case lopt_here:
	                    break ;
	                case lopt_reuse:
	                case lopt_ra:
	                    op->fl.reuse = true ;
	                    break ;
	                } /* end switch */
	            } else {
	                while (akl--) {
	                    cint	kc = mkchar(*akp) ;
	                    switch (kc) {
	                    case 'm':
	                        if (f_optequal) {
	                            f_optequal = false ;
	                            if (avl) {
	                                rs1 = cfnumui(avp,avl,&uv) ;
	                                if (rs1 >= 0) {
	                                    aip->mode = (uv & 0777) ;
					}
	                            }
	                        } else {
	                            if (ar > 0) {
	                                ar -= 1 ;
	                                fp = av[++ai] ;
	                                fl = lenstr(fp) ;
	                                if (fl > 0) {
	                                    rs1 = cfnumui(fp,fl,&uv) ;
	                                    if (rs1 >= 0) {
	                                        aip->mode = (uv & 0777) ;
					    }
	                                }
	                            }
	                        } /* end if */
	                        break ;
	                    } /* end switch */
	                    akp += 1 ;
			    if (rs < 0) break ;
	                } /* end while */
	            } /* end if */
	        } /* end if */
	    } else {
	        rs = arginfo_add(aip,fp) ;
	        pan += 1 ;
	    } /* end if */
	    if (rs < 0) break ;
	} /* end while */
	return rs ;
}
/* end subroutine (listenspec_procargs) */

#if	CF_OPENPORT

/* note that the 'opts' argument is ignored; this always REUSEs the port */
static int listenspec_openport(LS *op,int af,cc *hostspec,cc *portspec,
		int opts) noex {
	int		rs = SR_INVALID ;
	int		fd = -1 ;
	if (af >= 0) {
	    cchar	*protoname = PROTONAME_TCP ;
	    cchar	*hp = hostspec ;
	    rs = SR_OK ;
	    if (portspec[0] != '\0') {
	        if ((rs = getportnum(protoname,portspec)) >= 0) {
		    cint	port = rs ;
		    if ((rs = listenspec_prlocal(op)) >= 0) {
		         cchar	*pr = op->prlocal ;
		         bool	f = false ;
		         f = f || (hp == nullptr) ;
		         f = f || (hp[0] == '\0') ;
		         f = f || (strcmp(hp,"*") == 0) ;
		         if (f) hp = ANYHOST ;
		         rs = listenspec_openporter(op,pr,af,hp,port,opts) ;
		         fd = rs ;
		    } /* end if (listenspec_prlocal) */
	        } /* end if (getportnum) */
	    } else {
	        rs = SR_INVALID ;
	    }
	} /* end if (valid) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (listenspec_openport) */

/* ARGSUSED */
static int listenspec_openporter(LS *op,cchar *pr,int af,cchar *hp,
		int port,int opts) noex {
	ADDRINFO	ai ;
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	char		addr[INETXADDRLEN + 1] ;
	(void) opts ;
	if ((rs = listenspec_openportaddr(op,pr,&ai,addr,af,hp)) >= 0) {
	    sockaddress	sa ;
	    cint	type = SOCK_STREAM ;
	    cint	proto = IPPROTO_TCP ;
	    cint	flow = 0 ;
	    if ((rs = sockaddress_start(&sa,af,addr,port,flow)) >= 0) {
	    	cint	pf = ai.ai_family ;
		if ((rs = openport(pf,type,proto,&sa)) >= 0) {
	            fd = rs ;
	            rs = u_listen(fd,10) ;
		    if ((rs < 0) && (fd >= 0)) {
			u_close(fd) ;
			fd = -1 ;
		    }
	        } /* end if (openport) */
	        rs1 = sockaddress_finish(&sa) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (sockaddress) */
	} /* end if (open-port-addr) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (listenspec_openporter) */

/* ARGSUSED */
static int listenspec_openportaddr(LS *op,cchar *pr,ADDRINFO *aip,
		char *addr,int af,cchar	*hn) noex {
	int		rs = SR_INVALID ;
	int		addrlen = -1 ;
	(void) pr ;
	if (af >= 0) {
	    int		raf = 0 ;
	    addr[0] = '\0' ;
	    rs = memclear(aip) ;
	    if ((addrlen < 0) && ((af == AF_UNSPEC) || (af == AF_INET4))) {
	        raf = AF_INET4 ;
	        rs = listenspec_openportao(op,addr,raf,hn) ;
	        addrlen = rs ;
	    }
	    if ((addrlen < 0) || (rs == SR_NOTFOUND)) {
	        if ((af == AF_UNSPEC) || (af == AF_INET6)) {
	            raf = AF_INET6 ;
	            rs = listenspec_openportao(op,addr,raf,hn) ;
	            addrlen = rs ;
	        }
	    }
	    if (rs >= 0) {
	        if (raf > 0) {
	            if ((rs = getprotofamily(raf)) >= 0) {
	                aip->ai_family = rs ;
	            }
	        } else {
		    rs = SR_AFNOSUPPORT ;
	        }
	    } /* end if (ok) */
	} /* end if (valid) */
	return (rs >= 0) ? addrlen : rs ;
}
/* end subroutine (listenspec_openportaddr) */

static int listenspec_openportao(LS *op,char *addr,int af,cc *hn) noex {
	int		rs = SR_BUGCHECK ;
	int		rs1 ;
	int		addrlen = 0 ;
	if (op && addr && hn) {
	    rs = SR_INVALID ;
	    addr[0] = '\0' ;
	    if (af >= 0) {
		if ((rs = getaddrlen(af)) >= 0) {
	            hostinfo	hi ;
		    addrlen = rs ;
	            if ((rs = hostinfo_start(&hi,af,hn)) >= 0) {
	                hostinfo_cur	hicur ;
	                if ((rs = hostinfo_curbegin(&hi,&hicur)) >= 0) {
	                    auto hi_ea = hostinfo_curenumaddr ;
	                    const uchar	*ap ;
	                    while ((rs = hi_ea(&hi,&hicur,&ap)) > 0) {
	                        if (rs == addrlen) {
	                            memcpy(addr,ap,addrlen) ;
	                            break ;
	                        }
	                    } /* end while */
	                    rs1 = hostinfo_curend(&hi,&hicur) ;
	                    if (rs >= 0) rs = rs1 ;
	                } /* end if (cursor) */
	                rs1 = hostinfo_finish(&hi) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (hostinfo) */
	        } /* end if (getaddrlen) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? addrlen : rs ;
}
/* end subroutine (listenspec_openportao) */

#endif /* CF_OPENPORT */

static int listenspec_prlocal(LS *op) noex {
	int		rs ;
	int		rs1 ;
	if (op->prlocal == nullptr) {
	    char	*dbuf{} ;
	    if ((rs = malloc_hn(&dbuf)) >= 0) {
		cint	dlen = rs ;
	        if ((rs = getinetdomain(dbuf,dlen)) >= 0) {
	            char	*pbuf{} ;
		    if ((rs = malloc_mp(&pbuf)) >= 0) {
			cint	plen = rs ;
	                if ((rs = mkpr(pbuf,plen,PRNAME,dbuf)) >= 0) {
		            cchar	*cp ;
		            if ((rs = uc_mallocstrw(pbuf,rs,&cp)) >= 0) {
		                op->prlocal = cp ;
		            } /* end if (memory-allocation) */
	                } /* end if (mkpr) */
			rs1 = uc_free(pbuf) ;
			if (rs >= 0) rs = rs1 ;
		    } /* end if (m-a-f) */
	        } /* end if (getinetdname) */
		rs1 = uc_free(dbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} else {
	    rs = lenstr(op->prlocal) ;
	}
	return rs ;
}
/* end subroutine (listenspec_prlocal) */

#ifdef	COMMENT
static int tcp_nfield(cchar *fp,int fl) noex {
	int		n = 0 ;
	if (fl > 0) {
	    cchar	*cp ;
	    n += 1 ;
	    if (int cl ; (cp = strnchr(fp,fl,':')) != nullptr) {
	        n += 1 ;
	        cp += 1 ;
	        cl = ((fp + fl) - cp) ;
	        if (strnchr(cp,cl,':') != nullptr) {
	            n += 1 ;
		}
	    } /* end if */
	} /* end if (non-zero) */
	return n ;
}
/* end subroutine (tcp_nfield) */
#endif /* COMMENT */

static int tcpaddr_load(TCPADDR *ap,cchar *sp,int sl) noex {
	int		n = 0 ;
	if (sl < 0) sl = lenstr(sp) ;
	memclear(ap) ;
	if (sl > 0) {
	    int		cl ;
	    cchar	*cp ;
	    cchar	*tp ;
	    n += 1 ;
	    if ((tp = strnchr(sp,sl,':')) != nullptr) {
	        n += 1 ;
	        ap->af.sp = sp ;
	        ap->af.sl = (tp - sp) ;
	        ap->host.sp = sp ;
	        ap->host.sl = (tp - sp) ;
	        cp = (tp + 1) ;
	        cl = ((sp + sl) - cp) ;
	        if ((tp = strnchr(cp,cl,':')) != nullptr) {
	            n += 1 ;
	            ap->host.sp = cp ;
	            ap->host.sl = (tp - cp) ;
	            ap->port.sp = (tp + 1) ;
	            ap->port.sl = ((sp + sl) - (tp + 1)) ;
	        } else {
	            ap->af.sp = nullptr ;
	            ap->af.sl = 0 ;
	            ap->port.sp = cp ;
	            ap->port.sl = cl ;
	        } /* end if */
	    } else {
	        ap->port.sp = sp ;
	        ap->port.sl = sl ;
	    } /* end if */
	} /* end if (non-zero) */
	return n ;
}
/* end subroutine (tcpaddr_load) */

static int arginfo_start(ARGINFO *aip) noex {
	int		rs ;
	{
	    memclear(aip) ;
	    aip->argvalue = -1 ;
	    aip->slen = -1 ;
	    aip->mode = -1 ;
	    rs = vechand_start(&aip->pargs,0,0) ;
	}
	return rs ;
}
/* end subroutine (arginfo_start) */

static int arginfo_add(ARGINFO *aip,cchar *sp) noex {
	int		rs ;
	{
	    rs = vechand_add(&aip->pargs,sp) ;
	}
	return rs ;
}
/* end subroutine (arginfo_add) */

static int arginfo_get(ARGINFO *aip,int i,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	if (aip && rpp) {
	    vechand	*pap = &aip->pargs ;
	    void	*vp{} ;
	    if ((rs = vechand_get(pap,i,&vp)) >= 0) {
		if (vp) {
		    *rpp = charp(vp) ;
	            if (*rpp) {
	                rs = lenstr(*rpp) ;
	            }
		} else {
		    if (rpp) *rpp = nullptr ;
		}
	    } /* end if (vechand_get) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (arginfo_get) */

static int arginfo_finish(ARGINFO *aip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = vechand_finish(&aip->pargs) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (arginfo_finish) */


