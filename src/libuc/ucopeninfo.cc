/* ucopeninfo SUPPORT */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* higher-level "open" /w timeout */

#define	CF_ISMORE	0		/* compile in |isMorePossible()| */

/* revision history:

	= 1998-04-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Filename formats:

	UNIX® domain sockets have the format:
		filepath

	where:
		filepath

	is just a regular UNIX® file path to the socket file.

	File-systems that are supported internally (no external
	shared-memory object needed) are:

	proto
	prog
	pass
	shm
	u
	sys

	Protocols (when using the 'proto' filesystem above) have
	the format:

		/proto/<protoname>/<af>/<host>/<service>

	where:
		proto		constant name 'proto'
		<protoname>	protocol name
					tcp
					tcpmux[:port]
					tcpnls[:port]
					udp
					uss
					ussmux[:svc]
					usd
		<af>		address family
					inet
					inet4
					inet6
		<host>		hostname of remote host to contact
		<service>	service to contact


	Examples:

	/something/unix/domain/socket

	/proto/tcp/inet/rca/daytime
	/proto/udp/inet/rca/daytime
	/proto/udp/inet6/rca/daytime
	/proto/uss/unix/path
	/proto/usd/unix/path

	/proto/inet/tcp/rca/daytime
	/proto/inet/udp/rca/daytime
	/proto/inet6/udp/rca/daytime


	Notes: Whew! Is this a "smelly" code module?   I hope not.
	But am I in a good position to judge that?

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/socket.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<poll.h>
#include	<cstdlib>
#include	<cstring>
#include	<pwd.h>
#include	<netdb.h>
#include	<usystem.h>
#include	<vecstr.h>
#include	<getax.h>
#include	<typenonpath.h>
#include	<ucpwcache.h>		/* |ucpwcache_name(3uc)| */
#include	<mkchar.h>
#include	<localmisc.h>

#include	"ucopeninfo.h"


/* local defines */

#define	FUNCSTAT	u_stat
#define	FUNCFSTAT	u_fstat
#define	FUNCLSTAT	u_lstat
#define	FUNCOPEN	u_open

#ifndef	MAXSYMLINKS
#define	MAXSYMLINKS	20		/* defined by the OS */
#endif

#undef	FLBUFLEN
#define	FLBUFLEN	100
#define	PRNBUFLEN	MAXNAMELEN
#define	SVCBUFLEN	MAXNAMELEN

#define	NPOLLS		2
#define	POLLMULT	1000


/* external subroutines */

extern int	snopenflags(char *,int,int) ;
extern int	snwcpy(char *,int,cchar *,int) ;
extern int	sncpy1w(char *,int,cchar *,int) ;
extern int	mkpath1(char *,cchar *) ;
extern int	mkpath2(char *,cchar *,cchar *) ;
extern int	mkpath1w(char *,cchar *,int) ;
extern int	sfdirname(cchar *,int,cchar **) ;
extern int	matstr(cchar **,cchar *,int) ;
extern int	haslc(cchar *,int) ;
extern int	hascdpath(cchar *,int) ;
extern int	hasvarpathprefix(cchar *,int) ;
extern int	sichr(cchar *,int,int) ;
extern int	getnodedomain(char *,char *) ;
extern int	getdomainname(char *,int) ;
extern int	getuserhome(char *,int,cchar *) ;
extern int	mkpr(char *,int,cchar *,cchar *) ;
extern int	mkuserpath(char *,cchar *,cchar *,int) ;
extern int	mkvarpath(char *,cchar *,int) ;
extern int	mkcdpath(char *,cchar *,int) ;
extern int	isOneOf(cint *,int) ;
extern int	isNotPresent(int) ;

extern char	*strwcpy(char *,cchar *,int) ;
extern char	*strwcpylc(char *,cchar *,int) ;
extern char	*strwcpyuc(char *,cchar *,int) ;
extern char	*strnpbrk(cchar *,int,cchar *) ;
extern char	*strdcpy1(char *,int,cchar *) ;


/* external variables */

extern cchar	**environ ;


/* local structures */


/* forward references */

static int	u_openex(UCOPENINFO *) ;

static int	open_eval(UCOPENINFO *) ;
static int	open_otherlink(UCOPENINFO *,int *,char *) ;
static int	open_othertry(UCOPENINFO *,int *,char *) ;
static int	open_floatpath(UCOPENINFO *,int) ;
static int	open_pseudopath(UCOPENINFO *,cchar *,int) ;
static int	open_nonpath(UCOPENINFO *,int) ;
static int	open_nonpather(UCOPENINFO *,int,cchar *,cchar *) ;

static int	openproger(cchar *,int,mainv) noex ;
static int	accmode(int) ;
static int	waitready(int,int,int) ;
static int	pollok(int) ;
static int	getprefixfs(cchar *,cchar **) ;
static int	getnormalfs(cchar *,cchar **) ;
static int	noexist(cchar *,int) ;
static int	loadargs(vecstr *,cchar *) ;

#if	CF_ISMORE
extern int	isMorePossible(int) ;
#endif

constexpr bool	issl(int ch) noex {
	return (ch == '/') ;
}


/* local variables */

static constexpr cpcchar	normalfs[] = {
	"devices",
	"proc",
	"var",
	"kernel",
	"platform",
	"lib",
	"xfn",
	"bin",
	nullptr
} ;

enum prefixfses {
	prefixfs_proto,
	prefixfs_prog,
	prefixfs_pass,
	prefixfs_shm,
	prefixfs_user,
	prefixfs_sys,
	prefixfs_dev,
	prefixfs_overlast
} ;

static constexpr cpcchar	prefixfs[] = {
	"proto",
	"prog",
	"pass",
	"shm",
	"u",
	"sys",
	"dev",
	nullptr
} ;

enum accmodes {
	accmode_rdonly,
	accmode_wronly,
	accmode_rdwr,
	accmode_overlast
} ;

enum nonpaths {
	nonpath_not,
	nonpath_dialer,
	nonpath_fsvc,
	nonpath_usvc,
	nonpath_overlast
} ;

static cchar	*nonpaths = "/Â¥Â§~" ;

#if	CF_ISMORE
static cint	rsmore[] = {
	SR_OPNOTSUPP,
	SR_NOENT,
	0
} ;
#endif /* CF_ISMORE */


/* exported variables */


/* exported subroutines */

int uc_openex(cchar *fn,int of,mode_t om,int to,int oo) noex {
	ucopeninfo	oi{} ;
	oi.fname = fn ;
	oi.oflags = of ;
	oi.operms = om ;
	oi.to = to ;
	oi.opts = oo ;
	return uc_openinfo(&oi) ;
}
/* end subroutine (uc_openex) */

int uc_openinfo(ucopeninfo *oip) noex {
	int		rs ;
	int		fd = -1 ;

	if (oip == nullptr) return SR_FAULT ;
	if (oip->fname == nullptr) return SR_FAULT ;

	if (oip->fname[0] == '\0') return SR_INVALID ;

	if ((rs = accmode(oip->oflags)) >= 0) {

	    if (hascdpath(oip->fname,-1)) {
	        cint	tlen = MAXPATHLEN ;
	        char		*tbuf ;
	        if ((rs = uc_libmalloc((tlen+1),&tbuf)) >= 0) {
	            if ((rs = mkcdpath(tbuf,oip->fname,-1)) > 0) {
			oip->fname = tbuf ;
	                rs = open_eval(oip) ;
	                fd = rs ;
	            } else if (rs == 0) {
	                rs = SR_NOENT ;
	            }
	            uc_libfree(tbuf) ;
	        } /* end if (memory-allocation) */
	    } else {
	        rs = open_eval(oip) ;
	        fd = rs ;
	    }

	} /* end if (accmode) */

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (uc_openinfo) */


/* local subroutines */

static int u_openex(ucopeninfo *oip) noex {
	int		rs ;
	int		oflags = oip->oflags ;
	int		fd = -1 ;

	if (oip->opts & FM_LARGEFILE) oflags |= O_LARGEFILE ;

	if ((oflags & O_NDELAY) || (oip->to < 0)) {

	    if ((rs = FUNCOPEN(oip->fname,oflags,oip->operms)) >= 0) {
	        fd = rs ;
	    }

	} else {
	    oflags |= O_NDELAY ;
	    if ((rs = FUNCOPEN(oip->fname,oflags,oip->operms)) >= 0) {
	        USTAT	sb ;
	        fd = rs ;

	        if ((rs = FUNCFSTAT(fd,&sb)) >= 0) {
	            cmode	fm = sb.st_mode ;

	            if (S_ISFIFO(fm) &&
	                ((oflags & O_RDONLY) || (oflags & O_WRONLY))) {

	                rs = waitready(fd,oflags,oip->to) ;

	            } /* end if */

	            if (rs >= 0) {
	                oflags &= (~ O_NDELAY) ;
	                rs = u_fcntl(fd,F_SETFL,oflags) ;
	            } else {
	                u_close(fd) ;
	                fd = -1 ;
	            }

	        } /* end if (stat) */

	        if ((rs < 0) && (fd >= 0)) {
	            u_close(fd) ;
	            fd = -1 ;
	        }

	    } /* end if (ok) */

	} /* end if (alternatives) */

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (u_openex) */

static int open_eval(ucopeninfo *oip) noex {
	int		rs = SR_OK ;
	int		npi ;			/* non-path index */
	int		fd = -1 ;
	char		*efname = nullptr ;
	char		ofname[MAXPATHLEN+1] = { 0 } ;

	while ((rs >= 0) && (fd < 0)) {

	    if ((npi = typenonpath(oip->fname,-1)) > 0) {
	        rs = open_floatpath(oip,npi) ;
	        fd = rs ;
	    } else {
	        if (hasvarpathprefix(oip->fname,-1) && (efname == nullptr)) {
	            cint	sz = (MAXPATHLEN + 1) ;
	            void	*p ;
	            if ((rs = uc_libmalloc(sz,&p)) >= 0) {
	                efname = p ;
	                efname[0] = '\0' ;
	                if ((rs = mkvarpath(efname,oip->fname,-1)) > 0) {
	                    oip->fname = efname ;
	                } else if (rs <= 0) {
	                    if (rs == 0) rs = SR_BADFMT ;
	                    uc_libfree(efname) ;
	                    efname = nullptr ;
	                }
	            }
	        } /* end if (var-path) */

	        if (rs >= 0) {
		    cint	fch = mkchar(oip->fname[0]) ;
	            int		pi ;
		    cchar	*fn = oip->fname ;
	            cchar	*rp = nullptr ;
	            if (issl(fsch) && ((pi = getprefixfs(fn,&rp)) >= 0)) {
	                rs = open_pseudopath(oip,rp,pi) ;
	                fd = rs ;
	            } else if (issl(fch) && ((pi = getnormalfs(fn,&rp)) >= 0)) {
	                rs = u_openex(oip) ;
	                fd = rs ;
	            } else {
	                rs = open_othertry(oip,&fd,ofname) ; /* <- FD as arg */
	            } /* end if (a protocol or not) */
	        } /* end if (ok) */

	    } /* end if (alternatives) */

	} /* end while */

	if (efname) {
	    uc_libfree(efname) ;
	}

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (open_eval) */

static int open_othertry(ucopeninfo *oip,int *fdp,char *ofname) noex {
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	if ((rs = u_openex(oip)) >= 0) {
	    fd = rs ;
	} else {
	    USTAT	sb ;
	    switch (rs) {
	    case SR_OPNOTSUPP:
	        {
	            cint	of = oip->oflags ;
	            cint	to = oip->to ;
	            int		rs1 ;
	            cchar	*fn = oip->fname ;

	            if ((rs1 = FUNCSTAT(oip->fname,&sb)) >= 0) {
	                if (S_ISSOCK(sb.st_mode)) {
	                    rs = uc_opensocket(fn,of,to) ;
	                    fd = rs ;
	                }
	            } else if (! isNotPresent(rs1)) {
	                rs = rs1 ;
	            }

	        }
	        break ;
	    case SR_NOENT:
	        {

	            if ((rs1 = FUNCLSTAT(oip->fname,&sb)) >= 0) {

	                if (S_ISLNK(sb.st_mode)) {
	                    rs = open_otherlink(oip,fdp,ofname) ;
	                } /* end if (S_ISLNK) */

	            } else if (! isNotPresent(rs1)) {
			rs = rs1 ;
	            } /* end if (was a symbolic link) */

	        }
	        break ;
	    } /* end switch */
	} /* end if */
	if ((rs >= 0) && (fd >= 0)) *fdp = fd ;
	return rs ;
}
/* end subroutine (open_othertry) */

static int open_otherlink(ucopeninfo *oip,int *fdp,char *ofname) noex {
	cint	rlen = MAXPATHLEN ;
	int		rs ;
	int		fd = -1 ;
	char		rbuf[MAXPATHLEN + 1] ;
	cchar		*fn = oip->fname ;

	if ((rs = u_readlink(fn,rbuf,rlen)) >= 0) {
	    int		npi ;
	    rbuf[rs] = '\0' ;

	    if (rbuf[0] == '/') {
	        oip->fname = (cchar *) ofname ;
	        rs = mkpath1(ofname,rbuf) ;
	    } else if ((npi = typenonpath(rbuf,-1)) > 0) {
	        oip->fname = (cchar *) ofname ;
	        if ((rs = mkpath1(ofname,rbuf)) >= 0) {
	            rs = open_floatpath(oip,npi) ;
	            fd = rs ;
	        }
	    } else {
	        int		cl ;
	        cchar		*cp ;
	        cchar		*fn = oip->fname ;
	        char		dname[MAXPATHLEN + 1] ;

	        if ((cl = sfdirname(fn,-1,&cp)) > 0) {
	            cint	plen = MAXPATHLEN ;

	            oip->fname = (cchar *) ofname ;
	            if ((rs = snwcpy(dname,plen,cp,cl)) >= 0) {
	                rs = mkpath2(ofname,dname,rbuf) ;
	            }

	        } else {

	            oip->fname = (cchar *) ofname ;
	            rs = mkpath1(ofname,rbuf) ;

	        } /* end if */

	    } /* end if */

	    oip->clinks += 1 ;
	    if (rs >= 0) {
	        if (oip->clinks >= MAXSYMLINKS) {
	            rs = SR_MLINK ;
	        }
	    }

	    if ((rs >= 0) && (fd >= 0)) *fdp = fd ;
	} /* end if (reading symbolic link) */

	return rs ;
}
/* end subroutine (open_otherlink) */

static int open_floatpath(ucopeninfo *oip,int npi) noex {
	int		rs = SR_OK ;
	switch (npi) {
	case nonpath_dialer:
	case nonpath_fsvc:
	case nonpath_usvc:
	    rs = open_nonpath(oip,npi) ;
	    break ;
	default:
	    rs = SR_NOENT ;
	    break ;
	} /* end if */
	return rs ;
}
/* end subroutine (open_floatpath) */

static int open_pseudopath(ucopeninfo *oip,cchar *rp,int pi) noex {
	int		rs = SR_OK ;
	switch (pi) {
	case prefixfs_proto:
	    rs = uc_openproto(rp,oip->oflags,oip->to,oip->opts) ;
	    break ;
	case prefixfs_prog:
	    rs = openproger(rp,oip->oflags,oip->envv) ;
	    break ;
	case prefixfs_pass:
	    rs = uc_openpass(rp,oip->oflags,oip->to,0) ;
	    break ;
	case prefixfs_shm:
	    rs = uc_openshm(rp,oip->oflags,oip->operms) ;
	    break ;
	case prefixfs_user:
	    oip->fname = rp ;
	    rs = uc_openuserinfo(oip) ;
	    break ;
	case prefixfs_sys:
	case prefixfs_dev:
	    {
	        int		of = oip->oflags ;
	        mode_t		om = oip->operms ;
	        mainv		envv = oip->envv ;
	        int		to = oip->to ;
	        int		opts = oip->opts ;
	        switch (pi) {
	        case prefixfs_sys:
	            rs = uc_opensys(rp,of,om,envv,to,opts) ;
	            break ;
	        case prefixfs_dev:
	            rs = uc_opendev(rp,of,om,envv,to,opts) ;
	            break ;
	        } /* end switch */
	    } /* end block */
	    break ;
	default:
	    rs = SR_NOSYS ;
	    break ;
	} /* end switch */
	return rs ;
}
/* end subroutine (open_pseudopath) */

static int open_nonpath(ucopeninfo *oip,int npi) noex {
	cint	nlen = PRNBUFLEN ;
	cint	nch = mkchar(nonpaths[npi]) ;
	int		rs = SR_OK ;
	int		fd = -1 ;
	cchar		*fname = oip->fname ;
	cchar		*tp ;
	char		brkbuf[4] ;

	brkbuf[0] = nch ;
	brkbuf[1] = (char) 0xAD ;
	brkbuf[2] = '/' ;
	brkbuf[3] = 0 ;
	if ((tp = strpbrk(fname,brkbuf)) != nullptr) {

	    if (mkchar(tp[0]) == nch) {
	        char	prn[PRNBUFLEN+1] ;
	        if ((rs = sncpy1w(prn,nlen,fname,(tp - fname))) >= 0) {
	            cchar	*sp = (tp+1) ;
	            if (sp[0] != '\0') {
	                rs = open_nonpather(oip,npi,prn,sp) ;
	                fd = rs ;
	            } else {
	                rs = SR_PROTO ;		/* no SVC -> protocol error */
		    }
	        } /* end if (sncpy1w) */
	    } else {
	        rs = SR_NOANODE ;		/* bug-check exception */
	    }

	} /* end if (non-null) */

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (open_nonpath) */

static int open_nonpather(ucopeninfo *oip,int npi,cchar *prn,cchar *sp) noex {
	vecstr		args ;
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;

	if ((rs = vecstr_start(&args,4,0)) >= 0) {
	    cint	prlen = MAXPATHLEN ;
	    int		sl = -1 ;
	    mainv	av = nullptr ;
	    mainv	ev = oip->envv ;
	    cchar	*tp ;
	    char	svc[SVCBUFLEN+1] = { 0 } ;
	    char	brkbuf[4] ;
	    char	prbuf[MAXPATHLEN+1] ;

	    if (ev == nullptr) ev = (cchar **) environ ;

	    brkbuf[0] = (char) 0xAD ;
	    brkbuf[1] = ':' ;
	    brkbuf[2] = 0 ;
	    if ((tp = strpbrk(sp,brkbuf)) != nullptr) {
	        cint	ch = mkchar(*tp) ;
	        int		cl = -1 ;
	        cchar		*cp = sp ;

	        if (ch == ':') {
	            sl = (tp-sp) ;
	            cp = (tp+1) ;
	            tp = strchr(cp,0xAD) ;
	        }

	        if (tp != nullptr) {

	            cl = (tp-cp) ;
	            if (sl < 0) {
	                sl = cl ; /* or » if (ch != ':') sl = cl « */
	            }
	            if ((rs = vecstr_add(&args,cp,cl)) >= 0) {
	                cp = (tp+1) ;
	                rs = loadargs(&args,cp) ;
	            }

	        } else {
	            rs = vecstr_add(&args,cp,cl) ;
	        }

	    } else {
	        rs = vecstr_add(&args,sp,sl) ;
	    }

	    if (rs >= 0) {
	        if ((rs = sncpy1w(svc,SVCBUFLEN,sp,sl)) >= 0) {
	            rs = vecstr_getvec(&args,&av) ;
	        }
	    }

	    if (rs >= 0) {
	        switch (npi) {
	        case nonpath_dialer:
	            break ;
	        case nonpath_fsvc:
	            {
			cint	dl = MAXHOSTNAMELEN ;
	                char		dn[MAXHOSTNAMELEN+1] ;
	                if ((rs = getdomainname(dn,dl)) >= 0) {
	                    rs = mkpr(prbuf,prlen,prn,dn) ;
	                }
	            }
	            break ;
	        case nonpath_usvc:
	            {
	                rs = getuserhome(prbuf,prlen,prn) ;
	            }
	            break ;
	        } /* end switch */
	    } /* end if (ok) */

	    if (rs >= 0) {
	        cmode	om = oip->operms ;
	        cint	of = oip->oflags ;
	        cint	to = oip->to ;
	        switch(npi) {
	        case nonpath_dialer:
	            rs = uc_opendialer(prn,svc,of,om,av,ev,to) ;
	            fd = rs ;
	            break ;
	        case nonpath_usvc:
	            rs = uc_openusvc(prbuf,prn,svc,of,om,av,ev,to) ;
	            fd = rs ;
	            break ;
	        case nonpath_fsvc:
	            rs = uc_openfsvc(prbuf,prn,svc,of,om,av,ev,to) ;
	            fd = rs ;
	            break ;
	        default:
	            rs = SR_NOENT ;
	            break ;
	        } /* end switch */
	    } /* end if (ok) */

	    rs1 = vecstr_finish(&args) ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && (fd >= 0)) u_close(fd) ;
	} /* end if (vecstr-args) */

	return (rs >= 0) ? fd : rs ;
}
/* end if (open_nonpather) */

static int openproger(cchar *fname,int oflags,mainv ev) noex {
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	char		expfname[MAXPATHLEN+1] ;

	if (ev == nullptr) ev = (cchar **) environ ;

	if ((rs = mkuserpath(expfname,nullptr,fname,-1)) >= 0) {
	    vecstr	args ;
	    cchar	*fnp = fname ;
	    cchar	*pfp ;
	    cchar	*svcp ;
	    char	progfname[MAXPATHLEN + 1] ;

	    if (rs > 0) fnp = expfname ;

	    if ((rs = vecstr_start(&args,4,0)) >= 0) {

	        pfp = fnp ;
	        if ((svcp = strchr(fnp,0xAD)) != nullptr) {
	            pfp = progfname ;
	            if ((rs = mkpath1w(progfname,fnp,(svcp - fnp))) >= 0) {
	                if ((rs = vecstr_add(&args,pfp,rs)) >= 0) {
	                    rs = loadargs(&args,(svcp+1)) ;
	                }
	            }
	        } else {
	            rs = vecstr_add(&args,pfp,-1) ;
	        }

	        if (rs >= 0) {
	    	    cchar	**av = nullptr ;
	            if ((rs = vecstr_getvec(&args,&av)) >= 0) {

	                if ((pfp[0] == '/') && (pfp[1] == '%')) pfp += 1 ;
	                rs = uc_openprog(pfp,oflags,av,ev) ;
	                fd = rs ;
	            }
	        } /* end if (ok) */

	        rs1 = vecstr_finish(&args) ;
	        if (rs >= 0) rs = rs1 ;
		if ((rs < 0) && (fd >= 0)) u_close(fd) ;
	    } /* end if (args) */

	} /* end if (mkuserpath) */

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (openproger) */

static int accmode(int oflags) noex {
	int		rs = SR_INVALID ;
	int		am = (oflags & O_ACCMODE) ;
	switch (am) {
	case O_RDONLY:
	    rs = accmode_rdonly ;
	    break ;
	case O_WRONLY:
	    rs = accmode_wronly ;
	    break ;
	case O_RDWR:
	    rs = accmode_rdwr ;
	    break ;
	default:
	    rs = SR_INVALID ;
	    break ;
	} /* end switch */
	return rs ;
}
/* end subroutine (accmode) */

static int waitready(int fd,int oflags,int timeout) noex {
	cint	f_rdonly = (oflags & O_RDONLY) ;
	int		rs = SR_OK ;
	int		f_wait ;
	int		f = FALSE ;

	f_wait = f_rdonly || (oflags & O_WRONLY) ;
	if ((timeout >= 0) && f_wait) {
	    POLLFD	polls[NPOLLS] = {} ;
	    time_t	ti_timeout ;
	    time_t	daytime = time(nullptr) ;
	    int		sz = NPOLLS * sizeof(POLLFD) ;
	    int		pollto = ((timeout > 0) ? POLLMULT : 0) ;

	    polls[0].fd = fd ;
	    polls[0].events = (f_rdonly) ? POLLIN : POLLOUT ;
	    polls[1].fd = -1 ;

	    ti_timeout = daytime + timeout ;
	    while (rs >= 0) {
	        if ((rs = u_poll(polls,1,pollto)) > 0) {
	            cint	re = polls[0].revents ;
	            if ((rs = pollok(re)) >= 0) {
	                if (f_rdonly) {
	                    f = !!(re & POLLIN) ;
	                } else {
	                    f = !!(re & POLLOUT) ;
	                }
	            } /* end if (pollok) */
		} else {
	            if (rs == SR_INTR) rs = SR_OK ;
		    if (rs >= 0) {
			daytime = time(nullptr) ;
	                if (daytime >= ti_timeout) {
	                    rs = SR_TIMEDOUT ;
			}
		    }
	        } /* end if (poll) */
	        if (f) break ;
	    } /* end while */

	} /* end if (waiting) */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (waitready) */

static int pollok(int re) noex {
	int		rs = SR_OK ;
	if (re & POLLHUP) {
	    rs = SR_HANGUP ;
	} else if (re & POLLERR) {
	    rs = SR_POLLERR ;
	} else if (re & POLLNVAL) {
	    rs = SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (pollok) */

static int getnormalfs(cchar *fname,cchar **rpp) noex {
	int		rs = SR_OK ;
	int		pi = -1 ;

	*rpp = nullptr ;
	if (fname[0] == '/') {
	    cchar	*tp ;
	    cchar	*pp = (fname + 1) ;

	    while (*pp && (pp[0] == '/')) {
	        pp += 1 ;
	    }

	    if ((tp = strchr(pp,'/')) != nullptr) {
	        pi = matstr(normalfs,pp,(tp - pp)) ;
	        *rpp = (pi >= 0) ? tp : nullptr ;
	    } else {
	        rs = SR_NOENT ;
	    }

	} else {
	    rs = SR_NOENT ;
	}

	return (rs >= 0) ? pi : rs ;
}
/* end subroutine (getnormalfs) */

/* get the prefix-FS index (if there is a prefix-FS) */
static int getprefixfs(cchar *fname,cchar **rpp) noex {
	int		rs = SR_OK ;
	int		pi = -1 ;
	cchar		*tp = nullptr ;

	if (fname[0] == '/') {
	    cchar	*pp = (fname + 1) ;
	    int		pl = -1 ;

	    while (*pp && (pp[0] == '/')) pp += 1 ;

	    if ((tp = strchr(pp,'/')) != nullptr) {
	        pl = (tp-pp) ;
	    } else {
	        pl = strlen(pp) ;
	        tp = (pp+pl) ;
	    }

	    if ((pi = matstr(prefixfs,pp,pl)) >= 0) {
	        switch (pi) {
	        case prefixfs_proto:
	        case prefixfs_prog:
	        case prefixfs_pass:
	        case prefixfs_shm:
	        case prefixfs_user:
	        case prefixfs_sys:
	            rs = noexist(pp,pl) ;
	            break ;
	        } /* end switch */
	    } else {
	        rs = SR_NOENT ;
	    }

	} else {
	    rs = SR_NOENT ;
	}

	if (rpp != nullptr) {
	    *rpp = (rs >= 0) ? tp : nullptr ;
	}

	return (rs >= 0) ? pi : rs ;
}
/* end subroutine (getprefixfs) */

static int noexist(cchar *pp,int pl) noex {
	cint		nlen = MAXNAMELEN ;
	int		rs ;
	char		nbuf[MAXNAMELEN + 1] ;

	nbuf[0] = '/' ;
	if ((rs = snwcpy((nbuf+1),(nlen-1),pp,pl)) >= 0) {
	    USTAT	sb ;
	    if ((rs = FUNCSTAT(nbuf,&sb)) >= 0) {
	        if (! S_ISDIR(sb.st_mode)) {
	            rs = SR_EXIST ;
	        }
	    } else if (isNotPresent(rs)) {
	        rs = SR_OK ;
	    }
	}

	return rs ;
}
/* end subroutine (noexist) */

static int loadargs(vecstr *alp,cchar *sp) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	if (sp[0] != '\0') {
	    cchar		*tp ;

	    while ((tp = strchr(sp,0xAD)) != nullptr) {
	        c += 1 ;
	        rs = vecstr_add(alp,sp,(tp - sp)) ;
	        sp = (tp+1) ;
	        if (rs < 0) break ;
	    } /* end while */

	    if (rs >= 0) { /* do this unconditionally */
	        c += 1 ;
	        rs = vecstr_add(alp,sp,-1) ;
	    } /* end if (last component) */

	} /* end if (non-empty string) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (loadargs) */

#if	CF_ISMORE
static int isMorePossible(int rs) noex {
	return isOneOf(rsmore,rs) ;
}
/* end subroutine (isMorePossible) */
#endif /* CF_ISMORE */


