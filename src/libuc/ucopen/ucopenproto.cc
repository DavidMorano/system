/* ucopenproto SUPPORT */
/* charset=ISO8859-1 */
/* lang=C20 */

/* open a protocol */
/* version %I% last-modified %G% */

#define	CF_FINGERCLEAN	1		/* FINGER clean */
#define	CF_FINGERBACK	0		/* FINGER clean in background */
#define	CF_UNDERWORLD	0		/* print the "underworld" message */
#define	CF_EXPANDTAB	1		/* expand tabs (why do this?) */
#define	CF_TICOTSORD	0		/* compile in TICOTSORD */

/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	All protocol filenames have the format:

	/proto/<protoname>/<protoargs>

	where:

	<protoname>	protocol:
				ticotsord
				ticotsordnls
				ticotsordmux
				uss
				ussnls
				ussmux
				tcp
				tcpnls
				tcpmux
				udp
				usd
				finger
				http

	<protoargs>	arguments specific to a protocol

	- protocol		protocol-arguments

	ticotsord		/<addr>
	ticotsordnls[:<svc>]	/<addr>[/<svc>]
	ticotsordmux[:<svc>]	/<addr>[/<svc>]
	uss			<pxath>
	ussnls[:<svc>]		<pxath>
	ussmux[:<svc>]		<pxath>[­<arg(s)>]
	tcp			/<af>/<host>/<svc>
	tcpnls[:<port>]		/<af>/<host>[:<port>]/<svc>
	tcpmux[:<port>]		/<af>/<host>[:<port>]/<svc>­<arg(s)>
	udp[:<port>]		/<af>/<host>/<svc>
	usd			<pxath>
	finger[:<port>]		/<af>/<host>[:<port>]/<query>[+<ss>]­<arg(s)>
	http[:<port>]		/<af>/<host>/<pxath>

	where:

	<af>		address-family:
					unspec
					inet
					inet4
					inet6
	<host>		hostname of remote host to contact
	<svc>		service to contact
	<port>		INET-TCP or INET-UDP port
	<addr>		TLI address

	Examples:
	/proto/finger:5501/inet/rca/daytime
	/proto/tcpmux:5108/inet/rca/daytime
	/proto/tcp/inet/rca/daytime
	/proto/uss:daytime/tmp/local/tcpmuxd/srv
	/proto/http/inet/rca/robots.txt

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<netdb.h>
#include	<usystem.h>
#include	<getaf.h>
#include	<libmallocxx.h>
#include	<openshm.h>
#include	<ascii.h>
#include	<vecstr.h>
#include	<hostent.h>
#include	<sockaddress.h>
#include	<sbuf.h>
#include	<storeitem.h>
#include	<storebuf.h>
#include	<inetaddr.h>
#include	<filer.h>
#include	<linefold.h>
#include	<sfx.h>
#include	<snx.h>
#include	<snwcpy.h>
#include	<sncpyx.h>
#include	<strn.h>
#include	<strwcpy.h>
#include	<strdcpy.h>
#include	<mkpathx.h>
#include	<matstr.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"ucopen.h"
#include	"upt.h"

import libutil ;

/* local defines */

#define	SUBINFO		struct subinfo

#define	TICOTSORDARGS	struct ticotsordargs

#define	FINGERARGS	struct fingerargs

#ifndef	AF_INET4
#define	AF_INET4	AT_INET
#endif

#define	BUFLEN		(3 * 1024)

#ifndef	SVCLEN
#define	SVCLEN		MAXNAMELEN
#endif

#ifndef	REQBUFLEN
#define	REQBUFLEN	(4 * MAXPATHLEN)
#endif

#ifndef	POLL_INTMULT
#define	POLL_INTMULT	1000
#endif

#ifndef	PORTSPEC_FINGER
#define	PORTSPEC_FINGER	"finger"
#endif

#ifndef	CHAR_ISEND
#define	CHAR_ISEND(c)	(((c) == '\r') || ((c) == '\n'))
#endif

#undef	COLUMNS
#define	COLUMNS		80		/* display columns for FINGER */

#undef	COLBUFLEN
#define	COLBUFLEN	((2*COLUMNS) + 1) /* must be *twice* COLUMN length */

#undef	INDENT
#define	INDENT		2

#define	TO_DIAL		30
#define	TO_READ		(1*60)

#define	SI		SUBINFO


/* external subroutines */

#if	CF_TICOTSORD
extern int dialticotsord(cchar *,int,int,int) noex ;
extern int dialticotsordnls(cchar *,int,cchar *,int,int) noex ;
extern int dialticotsordmux(cchar *,int,cchar *,mainv,int,int) noex ;
#endif /* CF_TICOTSORD */

extern int dialuss(cchar *,int,int) noex ;
extern int dialussnls(cchar *,cchar *,int,int) noex ;
extern int dialussmux(cchar *,cchar *,mainv,int,int) noex ;
extern int dialtcp(cchar *,cchar *,int,int,int) noex ;
extern int dialtcpnls(cchar *,cchar *,int,cchar *,int,int) noex ;
extern int dialtcpmux(cchar *,cchar *,int,cchar *,mainv,int,int) noex ;
extern int dialudp(cchar *,cchar *,int,int,int) noex ;
extern int dialusd(cchar *,int,int) noex ;
extern int dialhttp(cchar *,cchar *,int,cchar *,mainv,int,int) noex ;


/* local structures */

struct subinfo {
	cchar		*fpath ;	/* caller supplied */
	cchar		*pnp ;		/* caller supplied (protocol-name) */
	cchar		*pap ;		/* caller supplied (protocol-args) */
	int		pnl ;
	int		pal ;
} ;

struct inetargs {
	cchar		*protop ;	/* only specially used */
	cchar		*afp ;
	cchar		*hostp ;
	cchar		*portp ;
	cchar		*svcp ;
	char		*a ;		/* memory allocation */
	vecstr		args ;
	int		protol ;
	int		afl ;
	int		hostl ;
	int		portl ;
	int		svcl ;
	int		f_args ;
} ;

struct ticotsordargs {
	storeitem	ss ;
	cchar		*addr ;
	cchar		*svc ;
	int		c ;
} ;

struct socktype {
	int		proto ;
	int		type ;
} ;

struct spacename {
	cchar		*name ;
	int		af ;
} ;

struct fingerargs {
	int		nfd ;
	int		cfd ;
} ;


/* forward references */

static int	openproto_ussmux(SI *,cchar *,cchar *,int,int) noex ;
static int	openproto_finger(SI *,cchar *,int,int) noex ;
static int	openproto_http(SI *,cchar *,int,int) noex ;
static int	openproto_ticotsord(SI *,int,int,int,int) noex ;
static int	openproto_inet(SI *,int,cchar *,int,int) noex ;

static int	inetargs_start(inetargs *,cchar *) noex ;
static int	inetargs_starter(inetargs *,cchar *) noex ;
static int	inetargs_alloc(inetargs *) noex ;
static int	inetargs_finish(inetargs *) noex ;

#if	CF_TICOTSORD
static int ticotsordargs_start(TICOTSORDARGS *,char *,int,cchar *,int) noex ;
static int ticotsordargs_load(TICOTSORDARGS *,cchar *,int) noex ;
static int ticotsordargs_finish(TICOTSORDARGS *) noex ;
#endif /* CF_TICOTSORD */

static int	loadargs(vecstr *,cchar *) noex ;
static int	sockshut(int,int) noex ;

static int	dialfinger(inetargs *,cchar *,int,int,int) noex ;

#if	CF_FINGERCLEAN
static int	fingerclean(int) noex ;
#if	CF_FINERBACK
static int	fingerworker(FINGERARGS *) noex ;
static int	fingerworker_loop(FINGERARGS *,FILER *,FILER *,
			int,int,int) noex ;
static int	fingerworker_liner(FINGERARGS *,FILER *,
			int,int,int,cchar *,int) noex ;
#endif /* CF_FINERBACK */
#endif /* CF_FINGERCLEAN */

#if	CF_FINGERCLEAN
#if	CF_FINGERBACK
static int	getline(int,cchar *,int) noex ;
static int	mkexpandtab(char *,int,int,cchar *,int) noex ;
#endif /* CF_FINGERBACK */
static bool	hasdirty(cchar *,int) noex ;
static bool	hasmseol(cchar *,int) noex ;
static bool	isdirty(int) noex ;
#endif /* CF_FINGERCLEAN */

static bool	hasBadOflags(int) noex ;


/* local variables */

enum protonames {
	protoname_ticotsord,
	protoname_ticotsordnls,
	protoname_ticotsordmux,
	protoname_uss,
	protoname_ussnls,
	protoname_ussmux,
	protoname_tcp,
	protoname_tcpnls,
	protoname_tcpmux,
	protoname_usd,
	protoname_udp,
	protoname_finger,
	protoname_http,
	protoname_overlast
} ;

static constexpr cpcchar	protonames[] = {
	"ticotsord",
	"ticotsordnls",
	"ticotsordmux",
	"uss",
	"ussnls",
	"ussmux",
	"tcp",
	"tcpnls",
	"tcpmux",
	"usd",
	"udp",
	"finger",
	"http",
	nullptr
} ;

constexpr cauto		mall = uc_libmalloc ;
constexpr cauto		mfre = uc_libfree ;


/* exported variables */


/* exported subroutines */

int uc_openproto(cchar *fname,int of,int to,int opts) noex {
	SUBINFO		si ;
	int		rs = SR_OK ;
	int		pni ;
	int		pnl, pal ;
	int		fd = -1 ;
	cchar	*fpath ;
	cchar	*pnp, *pap ;
	cchar	*tp ;
	cchar	*sp ;

	if (fname == nullptr) return SR_FAULT ;

	if (fname[0] == '\0') return SR_INVALID ;

	if (to < 0)
	    to = TO_DIAL ;

/* get the protocol name out of the filename */

	pnp = fname ;
	while (*pnp && (pnp[0] == '/')) {
	    pnp += 1 ;
	}

	if ((tp = strchr(pnp,'/')) != nullptr) {

	    fpath = tp ;
	    sp = (tp + 1) ;

/* is there a protocol argument? */

	    pal = 0 ;
	    cint	tl = intconv(tp - pnp) ;
	    if ((pap = strnchr(pnp,tl,':')) != nullptr) {
	        pnl = intconv(pap - pnp) ;
	        pap += 1 ;
	        pal = intconv(tp - pap) ;
	    } else {
	        pnl = intconv(tp - pnp) ;
	    }

	    memclear(&si) ;
	    si.fpath = fpath ;
	    si.pnp = pnp ;
	    si.pnl = pnl ;
	    si.pap = pap ;
	    si.pal = pal ;

/* lookup the protocol name */

	    pni = matcasestr(protonames,pnp,pnl) ;

	    switch (pni) {
	    case protoname_ticotsord:
	    case protoname_ticotsordnls:
	    case protoname_ticotsordmux:
	        rs = openproto_ticotsord(&si,pni,of,to,opts) ;
	        fd = rs ;
	        break ;
	    case protoname_tcp:
	    case protoname_tcpnls:
	    case protoname_tcpmux:
	    case protoname_udp:
	        if ((rs = openproto_inet(&si,pni,sp,to,opts)) >= 0) {
	            fd = rs ;
	            rs = sockshut(fd,of) ;
	        }
	        break ;
	    case protoname_usd:
	        if ((rs = dialusd(fpath,to,opts)) >= 0) {
	            fd = rs ;
	            rs = sockshut(fd,of) ;
	        }
	        break ;
	    case protoname_uss:
	        if ((rs = dialuss(fpath,to,opts)) >= 0) {
	            fd = rs ;
	            rs = sockshut(fd,of) ;
	        }
	        break ;
	    case protoname_ussnls:
	    case protoname_ussmux:
	        {
	            char	svcspec[SVCLEN + 1] ;
	            if (pap != nullptr) {
	                strdcpy1w(svcspec,SVCLEN,pap,pal) ;
	                if (pni == protoname_ussmux) {
	                    rs = openproto_ussmux(&si,fpath,svcspec,to,opts) ;
	                } else {
	                    rs = dialussnls(fpath,svcspec,to,opts) ;
	                }
	                if (rs >= 0) {
	                    fd = rs ;
	                    rs = sockshut(fd,of) ;
	                }
	            } else
	                rs = SR_DESTADDRREQ ;
	        } /* end block */
	        break ;
	    case protoname_finger:
	        rs = openproto_finger(&si,sp,of,to) ;
	        fd = rs ;
	        break ;
	    case protoname_http:
	        rs = openproto_http(&si,sp,of,to) ;
	        fd = rs ;
	        break ;
	    default:
	        rs = SR_PROTONOSUPPORT ;
	        break ;
	    } /* end switch (protocol name) */
	    if ((rs < 0) && (fd >= 0)) u_close(fd) ;

	} else {
	    rs = SR_INVALID ;
	}

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (uc_openproto) */


/* local subroutines */

static int openproto_inet(SI *sip,int pni,cchar *ap,int to,int no) noex {
	inetargs	a ;
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	if ((rs = inetargs_start(&a,ap)) >= 0) {
	    if ((rs = getaf(a.afp)) >= 0) {
	        cint	af = rs ;
	        cchar	*hp = a.hostp ;
	        cchar	*sp = a.svcp ;

	        switch (pni) {
	        case protoname_udp:
	            rs = dialudp(hp,sp,af,to,no) ;
	            break ;
	        case protoname_tcp:
	            rs = dialtcp(hp,sp,af,to,no) ;
	            break ;
	        case protoname_tcpmux:
	        case protoname_tcpnls:
	            {
	                cchar	*ps ;
	                char	pbuf[SVCLEN + 1] ;
	                ps = a.portp ;
	                if ((ps == nullptr) && (sip->pap != nullptr)) {
	                    ps = pbuf ;
	                    strdcpy1w(pbuf,SVCLEN,sip->pap,sip->pal) ;
	                }
	                switch (pni) {
	                case protoname_tcpmux:
	                    {
	                        mainv	av = nullptr ;
	                        if (a.f_args) {
	                            rs = vecstr_getvec(&a.args,&av) ;
	                        }
	                        if (rs >= 0) {
	                            rs = dialtcpmux(hp,ps,af,sp,av,to,no) ;
	                        }
	                    }
	                    break ;
	                case protoname_tcpnls:
	                    rs = dialtcpnls(hp,ps,af,sp,to,no) ;
	                    break ;
	                } /* end switch */
	            } /* end block */
	            break ;
	        } /* end switch */
	        fd = rs ;

	    } /* end if (getaf) */
	    rs1 = inetargs_finish(&a) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (inetargs) */
	if ((rs < 0) && (fd >= 0)) u_close(fd) ;

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (openproto_inet) */

#if	CF_TICOTSORD
static int openproto_ticotsord(SI *sip,int pni,int of,int to,int opts) noex {
	vecstr		args ;
	int		rs = SR_OK ;
	int		pl = -1 ;
	int		f_args = false ;
	cchar		*tp ;
	cchar		*pp = sip->fpath ;
	mainv		av = nullptr ;
	if ((tp = strchr(sip->fpath,0xAD)) != nullptr) {
	    f_args = true ;
	    pp = sip->fpath ;
	    pl = (tp-sip->fpath) ;
	    if ((rs = vecstr_start(&args,4,0)) >= 0) {
	        if ((rs = loadargs(&args,(tp+1))) >= 0) {
	            rs = vecstr_getvec(&args,&av) ;
	        }
	        if (rs < 0) {
	            f_args = false ;
	            vecstr_finish(&args) ;
	        }
	    } /* end if (vecstr) */
	} /* end if (arguments) */

	if (rs >= 0) {
	    TICOTSORDARGS	targs, *tap = &targs ;
	    cint		alen = MAXPATHLEN ;
	    char		abuf[MAXPATHLEN + 1] ;
	    if ((rs = ticotsordargs_start(&targs,abuf,alen,pp,pl)) >= 0) {
	        cchar	*svc = tap->svc ;
	        cint	ai = rs ;
	        if (tap->svc == nullptr) {
	            cchar	*pap = sip->pap ;
	            int		pal = sip->pal ;
	            svc = (abuf+ai) ;
#ifdef	COMMENT
	            rs = storebuf_strw(abuf,alen,ai,pap,pal) ;
#else
	            rs = snwcpy((abuf+ai),(alen-ai),pap,pal) ;
#endif /* COMMENT */
	        }
	        if (rs >= 0) {
	            switch (pni) {
	            case protoname_ticotsord:
	                rs = dialticotsord(tap->addr,-1,to,opts) ;
	                break ;
	            case protoname_ticotsordnls:
	                rs = dialticotsordnls(tap->addr,-1,svc,to,opts) ;
	                break ;
	            case protoname_ticotsordmux:
	                rs = dialticotsordmux(tap->addr,-1,svc,av,to,opts) ;
	                break ;
	            } /* end if (switch) */
	        } /* end if */
	        ticotsordargs_finish(&targs) ;
	    } /* end if (ticotsordargs) */
	} /* end if */

	if (f_args) {
	    vecstr_finish(&args) ;
	}

	return rs ;
}
/* end subroutine (openproto_ticotsord) */
#else /* CF_TICOTSORD */
/* ARGSUSED */
static int openproto_ticotsord(SI *sip,int pni,int of,int to,int opts) noex {
	(void) sip ;
	(void) pni ;
	(void) of ;
	(void) to ;
	(void) opts ;
	return SR_NOSYS ;
}
#endif /* CF_TICOTSORD */

static int openproto_ussmux(SI *sip,cchar *fpath,cchar *svc,
		int to,int no) noex {
	int		rs = SR_OK ;
	int		f_args = false ;
	cchar		*tp ;
	cchar		*pp = fpath ;
	char		pbuf[MAXPATHLEN + 1] ;

	if (sip == nullptr) return SR_FAULT ;

	if ((tp = strchr(fpath,0xAD)) != nullptr) {
	    cint	tl = intconv(tp - fpath) ;
	    pp = pbuf ;
	    rs = mkpath1w(pbuf,fpath,tl) ;
	    f_args = true ;
	}

	if (rs >= 0) {
	    vecstr	args ;
	    mainv	av = nullptr ;
	    if (f_args) {
	        if ((rs = vecstr_start(&args,4,0)) >= 0) {
	            if ((rs = loadargs(&args,(tp+1))) >= 0) {
	                rs = vecstr_getvec(&args,&av) ;
	            }
	        }
	    }
	    if (rs >= 0) {
	        rs = dialussmux(pp,svc,av,to,no) ;
	    }
	    if (f_args) {
	        vecstr_finish(&args) ;
	    }
	} /* end if (ok) */

	return rs ;
}
/* end subroutine (openproto_ussmux) */

static int openproto_finger(SI *sip,cchar *sp,int of,int to) noex {
	inetargs	a ;
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;

	if (hasBadOflags(of)) return SR_ROFS ;

	if ((rs = inetargs_start(&a,sp)) >= 0) {
	    if ((rs = getaf(a.afp)) >= 0) {
	        cint	af = rs ;
	        cchar		*psp = nullptr ;
	        char		pbuf[SVCLEN + 1] ;
	        psp = a.portp ;
	        if ((psp == nullptr) || (psp[0] == '\0')) {
	            if (sip->pap != nullptr) {
	                psp = pbuf ;
	                strdcpy1w(pbuf,SVCLEN,sip->pap,sip->pal) ;
	            }
	        } /* end if (port-spec) */
	        if (rs >= 0) {
	            mainv	av = nullptr ;
	            if (a.f_args) {
	                rs = vecstr_getvec(&a.args,&av) ;
	            }
	            if (rs >= 0) {
	                rs = dialfinger(&a,psp,af,to,of) ;
	                fd = rs ;
	            }
	        } /* end if (ok) */
	    } /* end if (getaf) */
	    rs1 = inetargs_finish(&a) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (inetargs) */

#if	CF_FINGERCLEAN
	if (rs >= 0) {
	    if ((rs = fingerclean(fd)) >= 0) {
	        u_close(fd) ;
	        fd = rs ;
	    }
	}
#endif /* CF_FINGERCLEAN */

	if ((rs < 0) && (fd >= 0)) u_close(fd) ;

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (openproto_finger) */

static int openproto_http(SI *sip,cchar *sp,int of,int to) noex {
	inetargs	a ;
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;

	if (sip == nullptr) return SR_FAULT ;

	if (hasBadOflags(of)) return SR_ROFS ;

	if ((rs = inetargs_start(&a,sp)) >= 0) {
	    if ((rs = getaf(a.afp)) >= 0) {
	        cint	af = rs ;
	        mainv	av = nullptr ;
	        if (a.f_args) {
	            rs = vecstr_getvec(&a.args,&av) ;
	        }
	        if (rs >= 0) {
	            rs = dialhttp(a.hostp,a.portp,af,a.svcp,av,to,0) ;
	            fd = rs ;
	        }
	    } /* end if (getaf) */
	    rs1 = inetargs_finish(&a) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (inetargs) */
	if ((rs < 0) && (fd >= 0)) u_close(fd) ;

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (openproto_http) */

static int inetargs_start(inetargs *iap,cchar *args) noex {
	int		rs = SR_OK ;
	cchar		*tp, *sp ;
	memclear(iap) ;
	sp = args ;
	if ((tp = strchr(sp,'/')) != nullptr) {
	    iap->afp = sp ;
	    iap->afl = intconv(tp - sp) ;
	    if (iap->afl > 0) {
	        sp = (tp + 1) ;
	        if ((tp = strchr(sp,'/')) != nullptr) {
	            iap->hostp = sp ;
	            iap->hostl = intconv(tp - sp) ;
	            if (iap->hostl > 0) {
	                cchar	*pp ;
	                cchar	*hp = iap->hostp ;
	                cint	hl = iap->hostl ;
	                iap->portp = nullptr ;
	                iap->portl = 0 ;
	                sp = (tp + 1) ;
	                if ((pp = strnchr(hp,hl,':')) != nullptr) {
	                    iap->portp = (pp + 1) ;
	                    iap->portl = intconv((hp + hl) - (pp + 1)) ;
	                    iap->hostl = intconv(pp - hp) ;
	                }
	                rs = inetargs_starter(iap,sp) ;
	            } else {
	                rs = SR_INVALID ;
		    }
	        } else {
	            rs = SR_INVALID ;
		}
	    } else {
	        rs = SR_INVALID ;
	    }
	} else {
	    rs = SR_INVALID ;
	}

	return rs ;
}
/* end subroutine (inetargs_start) */

static int inetargs_starter(inetargs *iap,cchar *sp) noex {
	cint		vo = VECSTR_OCOMPACT ;
	int		rs ;
	if ((rs = vecstr_start(&iap->args,4,vo)) >= 0) {
	    cchar	*tp ;
	    iap->f_args = true ;
	    /* service-name */
	    iap->svcp = sp ;
	    iap->svcl = -1 ;
	    if ((tp = strchr(sp,0xAD)) != nullptr) {
	        iap->svcl = intconv(tp - sp) ;
	        rs = loadargs(&iap->args,(tp+1)) ;
	    } /* end if (had dialer arguments) */
	    if (rs >= 0) {
	        if (iap->svcp[0] != '\0') {
	            rs = inetargs_alloc(iap) ;
	        } else {
	            rs = SR_INVALID ;
	        }
	    }
	    if (rs < 0) {
	        if (iap->f_args) {
	            iap->f_args = false ;
	            vecstr_finish(&iap->args) ;
	        }
	    }
	} /* end if (vecstr_start) */
	return rs ;
}
/* end subroutine (inetargs_starter) */

static int inetargs_finish(inetargs *iap) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (iap->a) {
	    rs1 = mfre(iap->a) ;
	    iap->a = nullptr ;
	    if (rs < 0) rs = rs1 ;
	}

	if (iap->f_args) {
	    iap->f_args = false ;
	    rs1 = vecstr_finish(&iap->args) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (inetargs_finish) */

static int inetargs_alloc(inetargs *iap) noex {
	int		rs = SR_OK ;
	int		sz = 0 ;
#ifdef	COMMENT
	if (iap->protop != nullptr) {
	    sz += (lenstr(iap->protop,iap->protol) + 1) ;
	}
#endif /* COMMENT */
	if (iap->afp != nullptr) {
	    sz += (lenstr(iap->afp,iap->afl) + 1) ;
	}
	if (iap->hostp != nullptr) {
	    sz += (lenstr(iap->hostp,iap->hostl) + 1) ;
	}
	if (iap->portp != nullptr) {
	    sz += (lenstr(iap->portp,iap->portl) + 1) ;
	}
	if (iap->svcp != nullptr) {
	    sz += (lenstr(iap->svcp,iap->svcl) + 1) ;
	}
	if (char *bp ; (rs = mall(sz,&bp)) >= 0) {
	    cchar	*cp ;
	    iap->a = bp ;
#ifdef	COMMENT
	    if (iap->protop != nullptr) {
	        cp = bp ;
	        bp = (strwcpy(bp,iap->protop,iap->protol) + 1) ;
	        iap->protop = cp ;
	    }
#endif /* COMMENT */
	    if (iap->afp != nullptr) {
	        cp = bp ;
	        bp = (strwcpy(bp,iap->afp,iap->afl) + 1) ;
	        iap->afp = cp ;
	    }
	    if (iap->hostp != nullptr) {
	        cp = bp ;
	        bp = (strwcpy(bp,iap->hostp,iap->hostl) + 1) ;
	        iap->hostp = cp ;
	    }
	    if (iap->portp != nullptr) {
	        cp = bp ;
	        bp = (strwcpy(bp,iap->portp,iap->portl) + 1) ;
	        iap->portp = cp ;
	    }
	    if (iap->svcp != nullptr) {
	        cp = bp ;
	        bp = (strwcpy(bp,iap->svcp,iap->svcl) + 1) ;
	        iap->svcp = cp ;
	    }
	} /* end if (memory-allocation) */
	return rs ;
}
/* end subroutine (inetargs_alloc) */

#if	CF_TICOTSORD

static int ticotsordargs_start(TICOTSORDARGS *tap,char *abuf,int alen,
		cchar *pp,int pl) noex {
	int		rs ;
	memclear(tap) ;
	if (pl < 0) pl = lenstr(pp) ;
	if ((rs = storeitem_start(&tap->ss,abuf,alen)) >= 0) {
	    cchar	*tp ;
	    while (pl && (pp[0] == '/')) {
	        pp += 1 ;
	        pl -= 1 ;
	    }
	    while ((tp = strnchr(pp,pl,'/')) != nullptr) {
	        rs = ticotsordargs_load(tap,pp,(tp-pp)) ;
	        pl -= ((tp+1)-pp) ;
	        pp = (tp+1) ;
	        if (rs < 0) break ;
	    } /* end while */
	    if ((rs >= 0) && pl) {
	        rs = ticotsordargs_load(tap,pp,pl) ;
	    }
	    if (rs >= 0) {
	        rs = storeitem_getlen(&tap->ss) ;
	    }
	} /* end if (storeitem) */
	return rs ;
}
/* end subroutine (ticotsordargs_start) */

static int ticotsordargs_finish(TICOTSORDARGS *tap) noex {
	return storeitem_finish(&tap->ss) ;
}
/* end subroutine (ticotsordargs_finish) */

static int ticotsordargs_load(TICOTSORDARGS *tap,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	if (sl > 0) {
	    switch (tap->c++) {
	    case 0:
	        rs = storeitem_strw(&tap->ss,sp,sl,&tap->addr) ;
	        break ;
	    case 1:
	        rs = storeitem_strw(&tap->ss,sp,sl,&tap->svc) ;
	        break ;
	    } /* end switch */
	} /* end if (non-zero-length string) */
	return rs ;
}
/* end subroutine (ticotsordargs_load) */

#endif /* CF_TICOTSORD */

static int dialfinger(inetargs *iap,cchar *psp,int af,int to,int of) noex {
	cint		reqlen = REQBUFLEN ;
	int		rs = SR_OK ;
	int		fd = -1 ;
	cchar		*portspec = nullptr ;
	char		reqbuf[REQBUFLEN + 1], *rp = reqbuf ;

	if (portspec == nullptr) {
	    if ((iap->portp != nullptr) && (iap->portp[0] != '\0')) {
#ifdef	COMMENT
	        portspec = argbuf ;
	        rs = snwcpy(argbuf,arglen,iap->portp,iap->portl) ;
#else
	        portspec = iap->portp ;
#endif /* COMMENT */
	    }
	}

	if (portspec == nullptr) {
	    if (psp != nullptr) {
	        portspec = psp ;
	    }
	}

	if (portspec == nullptr) {
	    portspec = PORTSPEC_FINGER ;
	}

	if (rs >= 0) {
	    if ((rs = dialtcp(iap->hostp,portspec,af,to,0)) >= 0) {
	        sbuf	b ;
	        int	rl ;
	        fd = rs ;
	        if ((rs = sbuf_start(&b,reqbuf,reqlen)) >= 0) {
	            sbuf_strw(&b,iap->svcp,iap->svcl) ;
	            if (of & O_NOCTTY) {
	                sbuf_strw(&b," /W",3) ;
	            }
	            if (iap->f_args) {
	                vecstr	*alp = &iap->args ;
	                cchar	*ap ;
	                for (int i = 0 ; vecstr_get(alp,i,&ap) >= 0 ; i += 1) {
	                    if (ap) {
	                        rs = sbuf_chr(&b,' ') ;
	                        if (rs >= 0) rs = sbuf_addquoted(&b,ap,-1) ;
	                    }
	                    if (rs < 0) break ;
	                } /* end for */
	            } /* end if (args) */
	            if (rs >= 0) {
	                sbuf_strw(&b,"\n\r",2) ;
	            }
	            rl = sbuf_finish(&b) ;
	            if (rs >= 0) rs = rl ;
	        } /* end if (sbuf) */
	        if (rs >= 0) {
	            rs = uc_writen(fd,rp,rl) ;
	        }
	        if (rs >= 0) {
	            rs = u_shutdown(fd,SHUT_WR) ;
	        }
	        if (rs < 0)
	            u_close(fd) ;
	    } /* end if (dialtcp) */
	} /* end if (ok) */

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (dialfinger) */

#if	CF_FINGERCLEAN
#if	CF_FINGERBACK
static int fingerclean(int nfd) noex {
	int		rs = SR_OK ;
	int		pfds[2] ;
	int		fd = -1 ;
	if ((rs = u_pipe(pfds)) >= 0) {
	    FINGERARGS	fa{} ;
	    pta		ta ;
	    pthread_t	tid ;
	    fd = pfds[0] ; /* return read end */
	    fa.nfd = nfd ;
	    fa.cfd = pfds[1] ; /* pass down write end */
	    if ((rs = pta_create(&ta)) >= 0) {
	        int	v = PTHREAD_CREATE_DETACHED ;
	        if ((rs = pta_setdetachstate(&ta,v)) >= 0) {
	            int (*worker)(void *) = (int (*)(void *)) fingerworker ;
	            rs = uptcreate(&tid,&ta,worker,&fa) ;
	        }
	        pta_destroy(&ta) ;
	    } /* end if (pthread-attribute) */
	    if (rs < 0) {
	        int	i ;
	        for (i = 0 ; i < 2 ; i += 1) u_close(pfds[i]) ;
	    }
	} /* end if (pipe) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (fingerclean) */

static int fingerworker(FINGERARGS *fap) noex {
	filer		out, *ofp = &out ;
	cint		nfd = fap->nfd ;
	cint		cfd = fap->cfd ;
	cint		cols = COLUMNS ;
	cint		ind = INDENT ;
	cint		to = TO_READ ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if ((rs = filer_start(ofp,cfd,0z,0,0)) >= 0) {
	    cint	fbo = FILER_ONET ;
	    if (filer fb ; (rs = filer_start(&fb,nfd,0z,0,fbo)) >= 0) {
	        {
	            rs = fingerworker_loop(fap,ofp,&fb,cols,ind,to) ;
	            wlen = rs ;
	        }
	        rs1 = filer_finish(&fb) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (filer) */
	    rs1 = filer_finish(ofp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (filer) */
	u_close(nfd) ;
	u_close(cfd) ;
	wlen &= INT_MAX ;
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (fingerworker) */

static int fingerworker_loop(FINGERARGS *fap,filer *ofp,filer *ifp,
		int cols,int ind,int to) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if (char *lbuf ; (rs = libmalloc_ml(&lbuf)) >= 0) {
	    cint	llen = rs ;
	    int		clen = 0 ;
	    int		sl ;
	    cchar	*sp ;
	    cchar	*cp ;
	    while ((rs = filer_readln(ifp,lbuf,llen,to)) > 0) {
		cchar	*lp ;
		if (int ll ; (ll = sfcontent(lbuf,rs,&lp)) > 0) {
	            if (linefold lf ; (rs = lf.start(cols,ind,lp,ll)) >= 0) {
	                if (int i = 0 ; (sl = lf.get(i,&sp)) >= 0 ; i += 1) {
#ifdef	COMMENT
	                    if ((sl == 0) || (sp[sl-1] != '\n')) {
	                        char *cp = colbuf ;
	                        if (sl > 0) {
	                            cp = strdcpy1w(colbuf,collen,sp,sl) ;
	                        }
	                        *cp++ = '\n' ;
	                        *cp = '\0' ;
	                        sp = colbuf ;
	                        sl = (cp-colbuf) ;
	                    }
#endif /* COMMENT */
	                    rs = fingerworker_liner(fap,ofp,cols,ind,i,sp,sl) ;
	                    wlen += rs ;
	                    i += 1 ;
	                    if (rs < 0) break ;
	                } /* end while */
	                rs1 = lf.finish ;
			if (rs >= 0) rs = rs1 ;
	            } /* end if */
	        } else {
	            rs = filer_println(ofp,lbuf,0) ;
	            wlen += rs ;
	        } /* end if (clen) */
	        if (rs < 0) break ;
	    } /* end while (reading lines) */
	    rs1 = uc_libfree(lbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (fingerworker_loop) */

static int fingerworker_liner(FINGERARGS *fap,filer *ofp,int cols,
		int ind,int ln,cc *sp,int sl) noex {
	int		rs ;
	int		rs1 ;
	int		clen ;
	int		sz ;
	int		gcols ;
	int		icols ;
	int		wlen = 0 ;

	if (fap == nullptr) return SR_FAULT ;

	if (cols <= 0) cols = 1 ;

	gcols = (ln == 0) ? cols : (cols-ind) ;
	icols = (ln == 0) ? 0 : ind ;

	clen = (2*cols) ;
	sz = (clen+1) ;
	if (char *cbuf ; (rs = mall(sz,&cbuf)) >= 0) {
	    int		i = 0 ;
	    int		ll, cl ;
	    int		ncols = gcols ;
	    int		nind = icols ;
	    cchar	*lp, *cp ;
	    cchar	*tp ;

	    while ((ll = getline(ncols,sp,sl)) >= 0) {
	        if ((ll == 0) && (i > 0)) break ;
	        lp = sp ;

	        cp = lp ;
	        cl = ll ;
	        while (cl && CHAR_ISEND(cp[sl-1])) cl -= 1 ;

#if	CF_EXPANDTAB
	        if ((tp = strnchr(lp,ll,'\t')) != nullptr) {
	            char	*bp = strwcpy(cbuf,lp,(tp-lp)) ;
	            int	bl ;
	            bl = (bp-cbuf) ;
	            rs = mkexpandtab(bp,(clen-bl),bl,tp,((lp+ll)-tp)) ;
	            cl = (rs + bl) ;
	            cp = cbuf ;
	        }
#endif /* CF_EXPANDTAB */

	        if ((rs >= 0) && (nind > 0)) {
	            rs = filer_writeblanks(ofp,nind) ;
	            wlen += rs ;
	        }

	        if (rs >= 0) {
	            rs = filer_println(ofp,cp,cl) ;
	            wlen += rs ;
	        }

	        sl -= ((lp + ll) - sp) ;
	        sp = (lp + ll) ;

	        i += 1 ;
	        nind = (icols+ind) ;
	        ncols = (gcols-ind) ;
	        if (rs < 0) break ;
	    } /* end while */
	    rs1 = mfre(cbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (memory-allocation) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (fingerworker_liner) */
#else /* CF_FINGERBACK */
static int fingerclean(cint fd) noex {
	cmode		om = 0664 ;
	int		rs ;
	int		rs1 ;
	int		nfd = -1 ;
	if ((rs = openshmtmp(nullptr,0,om)) >= 0) {
	    cint	fo = FILER_ONET ;
	    nfd = rs ;
	    if (filer b ; (rs = filer_start(&b,fd,0z,0,fo)) >= 0) {
	        cint		to = (1*60) ;
	        cint		llen = LINEBUFLEN ;
	        cint		clen = LINEBUFLEN ;
	        int		sz = 0 ;
	        sz += (llen+1) ;
	        sz += (clen+1) ;
	        if (char *bp ; (rs = mall(sz,&bp)) >= 0) {
	            char	*lbuf = bp ;
	            int		cbuf = (bp+(llen+1)) ;
	            while ((rs = filer_readln(&b,lbuf,llen,to)) > 0) {
	                int	len = rs ;
	                if (hasmseol(lbuf,len)) {
	                    len -= 1 ;
	                    lbuf[len-1] = CH_NL ;
	                }
	                if (hasdirty(lbuf,len)) {
	                    cint	cl = clen ;
	                    char	*cp = cbuf ;
	                    if ((rs = snwcpyclean(cp,cl,'¿',lbuf,len)) >= 0) {
	                        len = rs ;
	                        lbuf = cbuf ;
	                    }
	                }
	                if ((rs >= 0) && (len > 0)) {
	                    rs = u_write(nfd,lbuf,len) ;
	                }
	                if (rs < 0) break ;
	            } /* end while */
	            rs1 = mfre(bp) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a) */
	        rs1 = filer_finish(&b) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (filer) */
	    if (rs >= 0) rs = u_rewind(nfd) ;
	    if (rs < 0) u_close(nfd) ;
	} /* end if (tmp-file) */
	return (rs >= 0) ? nfd : rs ;
}
/* end subroutine (fingerclean) */
#endif /* CF_FINGERBACK */
#endif /* CF_FINGERCLEAN */

static int loadargs(vecstr *alp,cchar *sp) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	cchar		*tp ;
	while ((tp = strchr(sp,0xAD)) != nullptr) {
	    cint	tl = intconv(tp - sp) ;
	    c += 1 ;
	    rs = vecstr_add(alp,sp,tl) ;
	    sp = (tp + 1) ;
	    if (rs < 0) break ;
	} /* end while */
	if ((rs >= 0) && (sp[0] != '\0')) {
	    c += 1 ;
	    rs = vecstr_add(alp,sp,-1) ;
	}
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (loadargs) */

static int sockshut(int fd,int of) noex {
	cint		am = (of & O_ACCMODE) ;
	int		rs = SR_OK ;
	switch (am) {
	case O_WRONLY:
	    rs = u_shutdown(fd,SHUT_RD) ;
	    break ;
	case O_RDONLY:
	    rs = u_shutdown(fd,SHUT_WR) ;
	    break ;
	} /* end switch */
	return rs ;
}
/* end subroutine (sockshut) */

#if	CF_FINGERCLEAN

#if	CF_FINGERBACK

static int getline(int linelen,cchar *sp,int sl) noex {
	int		len = 0 ;
	if (sl > 0) {
	    cchar	*tp ;
	    len = MIN(sl,linelen) ;
	    if ((tp = strnbrk(sp,len,"\r\n")) != nullptr) {
	        len = ((tp + 1) - sp) ;
	    }
	    if ((len > 0) && (len < sl) && CHAR_ISEND(sp[len])) {
	        len += 1 ;
	        if ((len < sl) && (sp[len-1] == '\r') && (sp[len] == '\n')) {
	            len += 1 ;
	        }
	    }
	} /* end if (non-zero) */
	return len ;
}
/* end subroutine (getline) */

static int mkexpandtab(char *dp,int dl,int ci,cchar *sp,int sl) noex {
	sbuf		d ;
	int		rs ;
	int		len = 0 ;
	if ((rs = sbuf_start(&d,dp,dl)) >= 0) {
	    int		n ;
	    for (int i = 0 ; (rs >= 0) && (i < sl) && *sp ; i += 1) {
	        if (sp[i] == '\t') {
	            n = (8 - (ci % 8)) ;
	            rs = sbuf_blanks(&d,n) ;
	            ci += (n-1) ;
	        } else {
	            rs = sbuf_chr(&d,sp[i]) ;
	        }
	        ci += 1 ;
	    } /* end for */
	    len = sbuf_finish(&d) ;
	    if (rs >= 0) rs = len ;
	} /* end if (sbuf) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mkexpandtab) */

#endif /* CF_FINGERBACK */

static bool hasmseol(cchar *lp,int ll) noex {
	bool		f = false ;
	if (ll >= 2) {
	    f = ((lp[ll-2] == CH_CR) && (lp[ll-1] == CH_NL)) ;
	}
	return f ;
}
/* end subroutine (hasmseol) */

static bool hasdirty(cchar *lp,int ll) noex {
	int		ch ;
	bool		f = false ;
	for (int i = 0 ; (i < ll) && (lp[i] != '\0')  ; i += 1) {
	    ch = mkchar(lp[i]) ;
	    f = isdirty(ch) ;
	    if (f) break ;
	} /* end for */
	return f ;
}
/* end subroutine (hasdirty) */

static bool isdirty(int ch) noex {
	bool		f = false ;
	f = f || isprintlatin(ch) ;
	f = f || (ch == CH_BEL) ;
	f = f || (ch == CH_BS) ;
	f = f || (ch == CH_TAB) ;
	f = f || (ch == CH_NL) ;
	return (! f) ;
}
/* end subroutine (isdirty) */

#endif /* CF_FINGERCLEAN */


/* DEBUG subroutines */

#if	COMMENT
static int makeint(const void *addr) noex {
	int	hi = 0 ;
	uchar	*us = (uchar *) addr ;
	hi |= (mkchar(us[0]) << 24) ;
	hi |= (mkchar(us[1]) << 16) ;
	hi |= (mkchar(us[2]) << 8) ;
	hi |= (mkchar(us[3]) << 0) ;
	return hi ;
}
#endif /* CF_DEBUGS */

static bool hasBadOflags(int of) noex {
	bool		f = false ;
	f = f || (of & O_WRONLY) ;
	f = f || (of & O_RDWR) ;
	f = f || (of & O_CREAT) ;
	f = f || (of & O_TRUNC) ;
	return f ;
}
/* end subroutine (hasBadOflags) */


