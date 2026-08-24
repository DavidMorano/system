/* ucopeninfo SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* higher-level "open" /w timeout */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */
#define	CF_ISMORE	0		/* compile in |isMorePossible()| */

/* revision history:

	= 1998-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	ucopeninfo

	Description:
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

	/something/unix/<domain>/<socket>

	/proto/tcp/inet/rca/daytime
	/proto/udp/inet/rca/daytime
	/proto/udp/inet6/rca/daytime
	/proto/uss/unix/path
	/proto/usd/unix/path

	/proto/inet/tcp/rca/daytime
	/proto/inet/udp/rca/daytime
	/proto/inet6/udp/rca/daytime


	Notes: Whew!  Is this a "smelly" code module?  I hope not.
	But am I in a good position to judge that?

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>		/* POSIX */
#include	<sys/socket.h>		/* POSIX */
#include	<unistd.h>		/* POSIX */
#include	<fcntl.h>		/* POSIX */
#include	<poll.h>		/* POSIX */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ucsysconf.h>		/* LIBU */
#include	<ptm.h>			/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<getnodedomain.h>	/* LIBUC |getinetdomain(3uc)| */
#include	<getuserhome.h>		/* LIBYC |getuserhome(3uc)| */
#include	<getax.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<nonpath.h>		/* LIBUC |nonpathchs(3uc)| */
#include	<mkpathx.h>		/* LIBUC */
#include	<snwcpy.h>		/* LIBUC */
#include	<snx.h>			/* LIBUC */
#include	<mkx.h>			/* LIBUC */
#include	<sfx.h>			/* LIBUC |sfdirname(3uc)| */
#include	<strx.h>		/* LIBUC */
#include	<matstr.h>		/* LIBUC */
#include	<mkchar.h>		/* LIBU */
#include	<hasx.h>		/* LIBUC */
#include	<isoneof.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU |POLL_INTMULT| */
#include	<dprint.hh>		/* LIBU |DPRINTF(3u)| */

#include	"ucopen.h"
#include	"ucopeninfo.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	OI		ucopeninfo

#define	FUNSTAT		u_stat
#define	FUNFSTAT	u_fstat
#define	FUNSTATL	u_lstat
#define	FUNOPEN		u_open

#ifndef	PRNBUFLEN
#define	PRNBUFLEN	MAXNAMELEN
#endif
#ifndef	SVCBUFLEN
#define	SVCBUFLEN	MAXNAMELEN
#endif

#define	NPOLLS		2

#ifndef	POLL_INTMULT
#define	POLL_INTMULT	1000
#endif

#define	CHX_STAR	0xAD		/* the '¤' character */

#ifndef	CF_DEBUG
#define	CF_DEBUG	1		/* debugging */
#endif


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	ptm	mx ;
	mainv	envv ;
	char	*a ;
	char	*ofbuf ;
	char	*tfbuf ;
	int	oflen ;
	int	tflen ;
	int	nev ;
	int	maxsymloops ;
	int	maxpathlen ;
	int	maxnamelen ;
	int	svcnamelen ;
	int	maxdouble ;
	vars() noex : a(nullptr), ofbuf(nullptr), oflen(0) { 
	    tfbuf = nullptr ;
	    tflen = 0 ;
	} ; /* end ctor */
	operator int () noex ;
	int dacquire(char **) noex ;
	int drelease(char *) noex ;
	void dtor() noex ;
	destruct vars() {
	    if (ofbuf) dtor() ;
	} ; /* end destruct */
    private:
	int start() noex ;
	int finish() noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */

local int	ucopenex	(OI *) noex ;
local int	ucopeninfos	(OI *oip) noex ;

local int	open_eval	(OI *) noex ;
local int	open_otherlink	(OI *,int *,char *) noex ;
local int	open_othertry	(OI *,int *,char *) noex ;
local int	open_pathfloat	(OI *,int) noex ;
local int	open_pseudopath	(OI *,cchar *,int) noex ;
local int	open_pathnon	(OI *,int) noex ;
local int	open_pathnoner	(OI *,int,cchar *,cchar *) noex ;

local int	openproger	(cchar *,int,mainv) noex ;
local int	accmode_get	(int) noex ;
local int	waitready	(int,int,int) noex ;
local int	pollok		(int) noex ;
local int	getprefixfs	(cchar *,cchar **) noex ;
local int	getnormalfs	(cchar *,cchar **) noex ;
local int	noexist		(cchar *,int) noex ;
local int	loadargs	(vecstr *,cchar *) noex ;

#if	CF_ISMORE
extern int	isMorePossible(int) noex ;
#endif

constexpr bool	issl(int ch)		noex attrconst {
	return (ch == '/') ;
} /* end subroutine (issl) */

constexpr bool	isRdOrWrOnly(int of)	noex attrconst {
    	cint am = (of & O_ACCMODE) ;
    	return ((am == O_RDONLY) || (am == O_WRONLY)) ;
} /* end subroutine (isRdOrWrOnly) */


/* local variables */

constexpr cpcchar	normalfs[] = {
    	"**no-match**",
    	"usr",
	"var",
	"etc",
	"lib",
	"devices",
	"proc",
	"kernel",
	"platform",
	"bin",
	"sbin",
	"xfn",
	nullptr
} ; /* end array (normalfs) */

enum prefixfses {
	prefixfs_nomatch,
	prefixfs_proto,
	prefixfs_prog,
	prefixfs_pass,
	prefixfs_shm,
	prefixfs_user,
	prefixfs_sys,
	prefixfs_dev,
	prefixfs_overlast
} ; /* end enum */

namespace {
    struct prefixfs {
	cchar	*n[prefixfs_overlast + 1] ;
	consteval prefixfs() noex {
    	    n[prefixfs_nomatch]		= "**no-match**" ;
	    n[prefixfs_proto]		= "proto" ;
	    n[prefixfs_prog]		= "prog" ;
	    n[prefixfs_pass]		= "pass" ;
	    n[prefixfs_shm]		= "shm" ;
	    n[prefixfs_user]		= "u" ;
	    n[prefixfs_sys]		= "sys" ;
	    n[prefixfs_dev]		= "dev" ;
	    n[prefixfs_overlast]	= nullptr ;
	} ; /* end ctor */
    } ; /* end struct (prefixfs) */
} /* end namespace */

enum accmodes {
	accmode_rdonly,
	accmode_wronly,
	accmode_rdwr,
	accmode_overlast
} ; /* end enum */

#if	CF_ISMORE
constexpr int	rsmore[] = {
	SR_OPNOTSUPP,
	SR_NOENT,
	0
} ; /* end array */
#endif /* CF_ISMORE */

constexpr prefixfs	prefix_data ;
static vars		var ;
cint			ucloops		= 4 ;
cbool			f_debug		= CF_DEBUG ;


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
} /* end subroutine (uc_openex) */

int uc_openinfo(OI *oip) noex {
	int		rs = SR_FAULT ;
	int		fd = -1 ;
	if (oip) ylikely {
	    if (oip->fname) ylikely {
	        rs = SR_INVALID ;
	        if (oip->fname[0]) ylikely {
		    if ((rs = accmode_get(oip->oflags)) >= 0) ylikely {
			if (static cint rsv = var ; (rs = rsv) >= 0) {
			    rs = ucopeninfos(oip) ;
			    fd = rs ;
			} /* end if (var) */
	            } /* end if (accmode) */
	        } /* end if (valid) */
	    } /* end if (non-null) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (uc_openinfo) */


/* local subroutines */

local int ucopeninfos(OI *oip) noex {
    	int	rs ;
	int	rs1 ;
	int	fd = -1 ;
	DPRINTF("ent fname=%s\n",oip->fname) ;
	if ((rs = var.mx.lockbegin) >= 0) ylikely {
	    DPRINTF("lock-acquired rs=%d\n",rs) ;
            if (haspathprefix(oip->fname,-1)) {
		char *tfbuf = var.tfbuf ;
		cint tflen = var.tflen ;
	    	DPRINTF("pathprefix-y mknpathexp\n") ;
                if ((rs = mknpathexp(tfbuf,tflen,oip->fname,-1)) > 0) {
                    oip->fname = tfbuf ;
	    	    DPRINTF("open_eval rs=%d\n",rs) ;
                    rs = open_eval(oip) ;
                    fd = rs ;
	    	    DPRINTF("open_eval() rs=%d\n",rs) ;
                } else if (rs == 0) {
	    	    DPRINTF("mknpathexp-zero\n") ;
                    rs = SR_NOENT ;
                } /* end if */
            } else {
	    	DPRINTF("pathprefix-n open_eval\n") ;
                rs = open_eval(oip) ;
                fd = rs ;
            } /* end if */
	    DPRINTF("lock-releasing rs=%d\n",rs) ;
	    rs1 = var.mx.lockend ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ptm) */
	DPRINTF("ret rs=%d fd=%d\n",rs,fd) ;
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (ucopeninfos) */

local int open_eval(OI *oip) noex {
	int		rs = SR_OK ;
	int		fd = -1 ;
	char		*ofname = var.ofbuf ;
	char		*efname = nullptr ;
	ofname[0] = '\0' ;
	DPRINTF("ent\n") ;
	for (int i = 0 ; (rs >= 0) && (fd < 0) ; i += 1) {
	    DPRINTF("for-top\n") ;
	    if ((rs = nonpath(oip->fname,-1)) > 0) {
	        DPRINTF("nonpath() rs=%d\n",rs) ;
		cint npi = rs ;
	        rs = open_pathfloat(oip,npi) ;
	        fd = rs ;
	    } else if (rs == 0) {
	        DPRINTF("nonpath-zero rs=%d efname=%p\n",rs,efname) ;
	        if (haspathprefix(oip->fname,-1) && (efname == nullptr)) {
	            DPRINTF("haspathprefix\n") ;
	            if (char *p ; (rs = var.dacquire(&p)) >= 0) {
	                DPRINTF("acquire() rs=%d\n",rs) ;
	                efname = p ;
	                efname[0] = '\0' ;
	                if ((rs = mkpathvar(efname,oip->fname,-1)) > 0) {
	                    oip->fname = efname ;
	                } else if (rs <= 0) {
	                    if (rs == 0) rs = SR_BADFMT ;
	                    var.drelease(efname) ;
	                    efname = nullptr ;
	                } /* end if (error) */
	            } /* end if (memory-acquire) */
	            DPRINTF("haspathprefix rs=%d\n",rs) ;
	        } /* end if (var-path) */
	        DPRINTF("mid rs=%d\n",rs) ;
	        if (rs >= 0) {
		    cint	fch = mkchar(oip->fname[0]) ;
	            int		pi ;
		    cchar	*fn = oip->fname ;
	            cchar	*rp = nullptr ;
	            DPRINTF("trailer-ent rs=%d\n",rs) ;
		    cauto normy = [fn,fch] (cchar **rpp) noex -> int {
	                int rsl = issl(fch) ;
			if (rsl) rsl = getnormalfs(fn,rpp) ;
	            	return rsl ;
		    } ; /* end lambda */
	            if (issl(fch) && ((rs = getprefixfs(fn,&rp)) > 0)) {
			pi = rs ;
	                DPRINTF("trailer-pseudopath\n") ;
	                rs = open_pseudopath(oip,rp,pi) ;
	                fd = rs ;
	            } else if ((rs >= 0) && (rs = normy(&rp)) > 0) {
			pi = rs ;
	                DPRINTF("trailer-ucopenex\n") ;
	                rs = ucopenex(oip) ;
	                fd = rs ;
	            } else if (rs >= 0) {
	                DPRINTF("trailer-open_othertry\n") ;
	                rs = open_othertry(oip,&fd,ofname) ; /* <- FD as arg */
	            } /* end if (a protocol or not) */
	            DPRINTF("trailer-out rs=%d\n",rs) ;
	        } /* end if (ok) */
	    } /* end if (alternatives) */
	    if ((rs >= 0) && ((i >= var.maxsymloops) || (i >= ucloops))) {
		rs = SR_LOOP ;
	    }
	    DPRINTF("for-bot rs=%d\n",rs) ;
	} /* end for */
	DPRINTF("ret rs=%d fd=%d\n",rs,fd) ;
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (open_eval) */

local int ucopenex(OI *oip) noex {
	int		rs ;
	int		oflags = oip->oflags ;
	int		fd = -1 ;
	DPRINTF("ent fn=%s\n",oip->fname) ;
	if (oip->opts & FM_LARGEFILE) {
	    oflags |= O_LARGEFILE ;
	}
	if ((oflags & O_NDELAY) || (oip->to < 0)) {
	    DPRINTF("timeout-no\n") ;
	    if ((rs = FUNOPEN(oip->fname,oflags,oip->operms)) >= 0) {
	        fd = rs ;
	    }
	} else {
	    DPRINTF("timeout-yes\n") ;
	    oflags |= O_NDELAY ;
	    if ((rs = FUNOPEN(oip->fname,oflags,oip->operms)) >= 0) {
	        fd = rs ;
	        DPRINTF("FUNOPEN() rs=%d\n",rs) ;
	        if (ustat sb ; (rs = FUNFSTAT(fd,&sb)) >= 0) {
	            cmode	fm = sb.st_mode ;
	            if (S_ISFIFO(fm) && isRdOrWrOnly(oflags)) {
	                rs = waitready(fd,oflags,oip->to) ;
	            } /* end if */
	            if (rs >= 0) {
	                oflags &= (compl O_NDELAY) ;
	                rs = u_fcntl(fd,F_SETFL,oflags) ;
	            } else {
	                u_close(fd) ;
	                fd = -1 ;
	            }
	        } /* end if (stat) */
	        if ((rs < 0) && (fd >= 0)) {
	            u_close(fd) ;
	            fd = -1 ;
	        } /* end if (error) */
	    } /* end if (ok) */
	} /* end if (alternatives) */
	DPRINTF("ret rs=%d fd=%d\n",rs,fd) ;
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (ucopenex) */

local int open_othertry(OI *oip,int *fdp,char *ofname) noex {
	int		rs ;
	int		fd = -1 ;
	DPRINTF("ent ofn=%s\n",ofname) ;
	if ((rs = ucopenex(oip)) >= 0) {
	    fd = rs ;
	} else {
	    DPRINTF("ucopenex() rs=%d\n",rs) ;
	    ustat	sb ;
	    switch (rs) {
	    case SR_OPNOTSUPP:
	        {
	            cint	of = oip->oflags ;
	            cint	to = oip->to ;
	            cchar	*fn = oip->fname ;
	            if ((rs = FUNSTAT(oip->fname,&sb)) >= 0) {
	                if (S_ISSOCK(sb.st_mode)) {
	                    rs = uc_opensocket(fn,of,to) ;
	                    fd = rs ;
	                }
	            } else if (isNotPresent(rs)) {
	                rs = SR_OK ;
	            }
	        }
	        break ;
	    case SR_NOENT:
	        {
	    	    DPRINTF("-NOENT\n") ;
	            if ((rs = FUNSTATL(oip->fname,&sb)) >= 0) {
	                if (S_ISLNK(sb.st_mode)) {
	                    rs = open_otherlink(oip,fdp,ofname) ;
	                } /* end if (S_ISLNK) */
	            } /* end if (existed in some form) */
	        } /* end block */
	        break ;
	    } /* end switch */
	} /* end if */
	if ((rs >= 0) && (fd >= 0)) {
	    *fdp = fd ;
	}
	DPRINTF("ret rs=%d fd=%d\n",rs,fd) ;
	return rs ;
} /* end subroutine (open_othertry) */

local int open_otherlink(OI *oip,int *fdp,char *ofname) noex {
	cint		rlen = MAXPATHLEN ;
	int		rs ;
	int		fd = -1 ;
	char		rbuf[MAXPATHLEN + 1] ;
	cchar		*fn = oip->fname ;
	if ((rs = u_readlink(fn,rbuf,rlen)) >= 0) {
	    rbuf[rs] = '\0' ;
	    if (rbuf[0] == '/') {
	        oip->fname = ofname ;
	        rs = mkpath1(ofname,rbuf) ;
	    } else if ((rs = nonpath(rbuf,-1)) > 0) {
		cint npi = rs ;
	        oip->fname = ofname ;
	        if ((rs = mkpath1(ofname,rbuf)) >= 0) {
	            rs = open_pathfloat(oip,npi) ;
	            fd = rs ;
	        } /* end if (mkpath) */
	    } else if (rs >= 0) {
	        cchar		*cp ;
	        char		dname[MAXPATHLEN + 1] ;
	        if (int cl ; (cl = sfdirname(fn,-1,&cp)) > 0) {
	            cint	plen = MAXPATHLEN ;
	            oip->fname = ofname ;
	            if ((rs = snwcpy(dname,plen,cp,cl)) >= 0) {
	                rs = mkpath2(ofname,dname,rbuf) ;
	            }
	        } else {
	            oip->fname = ofname ;
	            rs = mkpath1(ofname,rbuf) ;
	        } /* end if */
	    } /* end if */
	    oip->clinks += 1 ;
	    if (rs >= 0) {
	        if (oip->clinks >= var.maxsymloops) {
	            rs = SR_MLINK ;
	        }
	    } /* end if (ok) */
	    if ((rs >= 0) && (fd >= 0)) {
		*fdp = fd ;
	    }
	} /* end if (reading symbolic link) */
	return rs ;
} /* end subroutine (open_otherlink) */

local int open_pathfloat(OI *oip,int npi) noex {
	int		rs = SR_OK ;
	switch (npi) {
	case nonpath_dialer:
	case nonpath_fsvc:
	case nonpath_usvc:
	    rs = open_pathnon(oip,npi) ;
	    break ;
	default:
	    rs = SR_NOENT ;
	    break ;
	} /* end if */
	return rs ;
} /* end subroutine (open_pathfloat) */

local int open_pseudopath(OI *oip,cchar *rp,int pi) noex {
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
	        mainv		envv = oip->envv ;
	        int		of = oip->oflags ;
	        int		to = oip->to ;
	        int		opts = oip->opts ;
	        mode_t		om = oip->operms ;
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
} /* end subroutine (open_pseudopath) */

local int open_pathnon(OI *oip,int npi) noex {
	cint		nlen = PRNBUFLEN ;
	cint		nch = mkchar(nonpathchs[npi]) ;
	int		rs = SR_OK ;
	int		fd = -1 ;
	cchar		*fname = oip->fname ;
	char		brkbuf[4] = {} ;
	brkbuf[0] = char(nch) ;
	brkbuf[1] = char(0xAD) ;
	brkbuf[2] = '/' ;
	brkbuf[3] = '\0' ;
	if (cchar *tp ; (tp = strbrk(fname,brkbuf)) != nullptr) {
	    if (mkchar(tp[0]) == nch) {
	        char	prn[PRNBUFLEN+1] ;
		cint	tl = intconv(tp - fname) ;
	        if ((rs = sncpy1w(prn,nlen,fname,tl)) >= 0) {
	            cchar	*sp = (tp+1) ;
	            if (sp[0] != '\0') {
	                rs = open_pathnoner(oip,npi,prn,sp) ;
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
} /* end subroutine (open_pathnon) */

local int open_pathnoner(OI *oip,int npi,cchar *prn,cchar *sp) noex {
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	if (vecstr args ; (rs = vecstr_start(&args,4,0)) >= 0) {
	    cint	prlen = MAXPATHLEN ;
	    int		sl = -1 ;
	    mainv	av = nullptr ;
	    mainv	ev = oip->envv ;
	    char	svc[SVCBUFLEN+1] = { 0 } ;
	    char	brkbuf[4] = { char(0xAD), ':' } ;
	    char	prbuf[MAXPATHLEN+1] ;

	    if (ev == nullptr) ev = var.envv ;

	    if (cchar *tp ; (tp = strbrk(sp,brkbuf)) != nullptr) {
	        cint	ch = mkchar(*tp) ;
	        int		cl = -1 ;
	        cchar		*cp = sp ;

	        if (ch == ':') {
	            sl = intconv(tp - sp) ;
	            cp = (tp+1) ;
	            tp = strchr(cp,0xAD) ;
	        }

	        if (tp != nullptr) {

	            cl = intconv(tp - cp) ;
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
	                char	dn[MAXHOSTNAMELEN+1] ;
	                if ((rs = getinetdomain(dn,dl)) >= 0) {
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
	        switch (npi) {
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
} /* end if (open_pathnoner) */

local int openproger(cchar *fname,int oflags,mainv ev) noex {
    	cnullptr	np{} ;
	cint		sz = (2 * (var.maxpathlen + 1)) ;
	cint		maxpath = var.maxpathlen ;
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	int		ai = 0 ;
	if (ev == nullptr) ev = var.envv ;
	if (char *a ; (rs = libmem.mall(sz,&a)) >= 0) {
	    char	*expfname = (a + (ai++ * (maxpath + 1))) ;
	    if ((rs = mkpathuser(expfname,np,fname,-1)) >= 0) {
	        cchar	*fnp = fname ;
	        cchar	*pfp ;
	        char	*progfname = (a + (ai++ * (maxpath + 1))) ;
	        if (rs > 0) fnp = expfname ;
	        if (vecstr args ; (rs = vecstr_start(&args,4,0)) >= 0) {
	            pfp = fnp ;
	            if (cchar *svcp ; (svcp = strchr(fnp,0xAD)) != np) {
		        cint	tl = intconv(svcp - fnp) ;
	                pfp = progfname ;
	                if ((rs = mkpath1w(progfname,fnp,tl)) >= 0) {
	                    if ((rs = vecstr_add(&args,pfp,rs)) >= 0) {
	                        rs = loadargs(&args,(svcp+1)) ;
	                    }
	                }
	            } else {
	                rs = vecstr_add(&args,pfp,-1) ;
	            }
	            if (rs >= 0) {
	    	        mainv	av = nullptr ;
	                if ((rs = vecstr_getvec(&args,&av)) >= 0) {
	                    if ((pfp[0] == '/') && (pfp[1] == '%')) pfp += 1 ;
	                    rs = uc_openprog(pfp,oflags,av,ev) ;
	                    fd = rs ;
	                }
	            } /* end if (ok) */
	            rs1 = vecstr_finish(&args) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (args) */
	    } /* end if (mkpathuser) */
	    rs1 = libmem.free(a) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
		if ((rs < 0) && (fd >= 0)) {
		    u_close(fd) ;
		} /* end if (error) */
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (openproger) */

local int accmode_get(int oflags) noex {
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
} /* end subroutine (accmode_get) */

local int waitready(int fd,int oflags,int timeout) noex {
	cbool		f_rdonly = (oflags & O_RDONLY) ;
	int		rs = SR_OK ;
	bool		f_wait ;
	int		f = false ; /* return-value */
	f_wait = f_rdonly || (oflags & O_WRONLY) ;
	if ((timeout >= 0) && f_wait) {
	    POLLFD	polls[NPOLLS] = {} ;
	    time_t	ti_timeout ;
	    time_t	daytime = time(nullptr) ;
	    int		pollto = ((timeout > 0) ? POLL_INTMULT : 0) ;
	    polls[0].fd = fd ;
	    polls[0].events = (f_rdonly) ? POLLIN : POLLOUT ;
	    polls[1].fd = -1 ;
	    ti_timeout = daytime + timeout ;
	    while (rs >= 0) {
	        if ((rs = u_poll(polls,NPOLLS,pollto)) > 0) {
	            cint	re = polls[0].revents ;
	            if ((rs = pollok(re)) >= 0) {
	                if (f_rdonly) {
	                    f = !!(re & POLLIN) ;
	                } else {
	                    f = !!(re & POLLOUT) ;
	                }
	            } /* end if (pollok) */
		} else {
	            if (rs == SR_INTR) {
			rs = SR_OK ;
		    }
		    if (rs >= 0) {
			daytime = time(nullptr) ;
	                if (daytime >= ti_timeout) {
	                    rs = SR_TIMEDOUT ;
			}
		    } /* end if (ok) */
	        } /* end if (poll) */
	        if (f) break ;
	    } /* end while */
	} /* end if (waiting) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (waitready) */

local int pollok(int re) noex {
	int		rs = SR_OK ;
	if (re & POLLHUP) {
	    rs = SR_HANGUP ;
	} else if (re & POLLERR) {
	    rs = SR_POLLERR ;
	} else if (re & POLLNVAL) {
	    rs = SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (pollok) */

local int getnormalfs(cchar *fname,cchar **rpp) noex {
	int		rs = SR_OK ;
	int		pi = 0 ;
	*rpp = nullptr ;
	if (fname[0] == '/') {
	    cchar *pp = (fname + 1) ; /* used-afterwards */
	    while (*pp && (pp[0] == '/')) {
	        pp += 1 ;
	    } /* end for */
	    if (cchar *tp = strchr(pp,'/') ; tp) {
		cint	tl = intconv(tp - pp) ;
	        if (int mi = matstr(normalfs,pp,tl) ; mi > 0) {
		    pi = mi ;
		}
	        *rpp = (pi > 0) ? tp : nullptr ;
	    } /* end if (found) */
	} /* end if (suitable file-name) */
	return (rs >= 0) ? pi : rs ;
} /* end subroutine (getnormalfs) */

/* get the prefix-FS index (if there is a prefix-FS) */
local int getprefixfs(cchar *fname,cchar **rpp) noex {
	int		rs = SR_OK ;
	int		pi = 0 ; /* return-value (default == *no-match*) */
	cchar		*tp = nullptr ; /* used-multiple */
	if (fname[0] == '/') {
	    cchar	*pp = (fname + 1) ;
	    int		pl = -1 ;
	    while (*pp && (pp[0] == '/')) {
		pp += 1 ;
	    }
	    if ((tp = strchr(pp,'/')) != nullptr) {
	        pl = intconv(tp - pp) ;
	    } else {
	        pl = lenstr(pp) ;
	        tp = (pp + pl) ;
	    } /* end if */
	    if (pl > 0) {
		if (int mi = matstr(prefix_data.n,pp,pl) ; mi > 0) {
	            switch (mi) {
	            case prefixfs_proto:
	            case prefixfs_prog:
	            case prefixfs_pass:
	            case prefixfs_shm:
	            case prefixfs_user:
	            case prefixfs_sys:
	                if ((rs = noexist(pp,pl)) >= 0) {
			    pi = mi ;
		        }
	                break ;
	            } /* end switch */
	        } /* end if (matstr) */
	    } /* end if (non-zero positive) */
	} /* end if (not suitable file-name) */
	if (rpp) {
	    *rpp = (rs >= 0) ? tp : nullptr ;
	}
	return (rs >= 0) ? pi : rs ;
} /* end subroutine (getprefixfs) */

local int noexist(cchar *pp,int pl) noex {
	int		rs ;
	int		rs1 ;
	if (char *nbuf ; (rs = lm_mp(&nbuf)) >= 0) {
	    cint nlen = rs ;
	    nbuf[0] = '/' ;
	    if ((rs = snwcpy((nbuf+1),(nlen-1),pp,pl)) >= 0) {
	        if (ustat sb ; (rs = FUNSTAT(nbuf,&sb)) >= 0) {
	            if (! S_ISDIR(sb.st_mode)) {
	                rs = SR_EXIST ;
	            }
	        } else if (isNotPresent(rs)) {
	            rs = SR_OK ;
	        }
	    } /* end if (snwcpy) */
	    rs1 = lm_free(nbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
} /* end subroutine (noexist) */

local int loadargs(vecstr *alp,cchar *sp) noex {
    	cnullptr	np{} ;
	int		rs = SR_OK ;
	int		c = 0 ; /* return-value */
	if (sp[0] != '\0') {
	    for (cchar *tp ; (tp = strchr(sp,CHX_STAR)) != np ; ) {
		cint	tl = intconv(tp - sp) ;
	        c += 1 ;
	        rs = vecstr_add(alp,sp,tl) ;
	        sp = (tp+1) ;
	        if (rs < 0) break ;
	    } /* end for */
	    if (rs >= 0) { /* <- do this unconditionally */
	        c += 1 ;
	        rs = vecstr_add(alp,sp,-1) ;
	    } /* end if (last component) */
	} /* end if (non-empty string) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (loadargs) */

#if	CF_ISMORE
local bool isMorePossible(int rs) noex {
	return isOneOf(rsmore,rs) ;
} /* end subroutine (isMorePossible) */
#endif /* CF_ISMORE */

vars::operator int () noex {
    	return start() ;
} /* end method (vars::operator) */

int vars::start() noex {
    	cnullptr	np{} ;
    	int		rs ;
	if ((rs = mx.create) >= 0) {
    	    cint	cmd =_SC_SYMLOOP_MAX ;
	    if ((rs = uc_sysconfval(cmd,np)) >= 0) {
	        maxsymloops = rs ;
	        if ((rs = bufsizeget(bufsize_mp)) >= 0) {
	            maxpathlen = rs ;
		    maxdouble = (rs * 2) ;
	            if ((rs = bufsizeget(bufsize_mn)) >= 0) {
			int ai = 2 ;
		        maxnamelen = rs ;
	                if ((rs = u_getenviron(&envv)) >= 0) {
			    cint sz = (ai * (maxdouble + 1)) ;
		            nev = rs ;
			    if ((rs = libmem.mall(sz,&a)) >= 0) {
			        memclear(a,rs) ;
				ofbuf = a + (--ai * (maxdouble + 1)) ;
				tfbuf = a + (--ai * (maxdouble + 1)) ;
			        oflen = maxdouble ;
			        tflen = maxdouble ;
			    } /* emd if (memory-acquire) */
		        } /* end if (u_getenviron) */
	            } /* end if (bufsizeget) */
	        } /* end if (bufsizeget) */
	    } /* end if (uc_sysconfval) */
	} /* end if (ptm_start) */
    	return rs ;
} /* end method (vars::start) */

int vars::finish() noex {
    	int		rs = SR_OK ;
	int		rs1 ;
    	if (ofbuf) {
	    ofbuf[0] = '\0' ;
	    tfbuf[0] = '\0' ;
	    rs1 = libmem.free(a) ;
	    if (rs >= 0) rs = rs1 ;
	    a = nullptr ;
	    ofbuf = nullptr ;
	    tfbuf = nullptr ;
	    oflen = 0 ;
	    tflen = 0 ;
	} /* end if (memory-release) */
	{
	    rs1 = mx.destroy ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
} /* end method (vars::finish) */

int vars::dacquire(char **rpp) noex {
    	int	rs ;
    	if ((rs = libmem.mall((maxdouble+1),rpp)) >= 0) {
	    rs = maxdouble ;
	} /* end if (memory-acquire) */
    	return rs ;
} /* end method (vars::dacquire) */

int vars::drelease(char *p) noex {
    	p[0] = '\0' ;
    	return libmem.free(p) ; /* memory-release */
} /* end method (vars::drelease) */

void vars::dtor() noex {
    	if (cint rs = finish() ; rs < 0) {
	    ulogerror("ucopeinfo",rs,"fini-finish") ;
	}
} /* end method (vars::dtor) */


