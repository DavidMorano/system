/* propenqotd SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* open a channel (file-descriptor) to the quote-of-the-day (QOTD) */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debugging */
#define	CF_OPENDEF	0		/* ? */
#define	CF_SOURCES	1		/* use sources */
#define	CF_CONFIGCHECK	0		/* |config_check()| */

/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	propenqotd

	Description:
	We sort of form the back-end maintenance end of the QOTD mechansim.

	Synopsis:
	int maintqotd(cchar *pr,int mjd,int of,int to) noex

	Arguments:
	pr		program-root
	mjd		modified-julian-day
	of		open-flags
	to		time-out

	Returns:
	>=0		FD of QOTD
	<0		error (system-return)

	Notes:
	- open flags:
		O_NOTTY
		O_EXCL		
		O_SYNC
		O_NDELAY
		O_NONBLOCK

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<netdb.h>
#include	<tzfile.h>		/* for TM_YEAR_BASE */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<ugetpid.h>
#include	<estrings.h>
#include	<mkfnamesuf.h>
#include	<ids.h>
#include	<sigman.h>
#include	<tmtime.hh>
#include	<storebuf.h>
#include	<vecstr.h>
#include	<vecpstr.h>
#include	<ascii.h>
#include	<paramfile.h>
#include	<expcook.h>
#include	<logfile.h>
#include	<isnot.h>
#include	<ischarx.h>
#include	<localmisc.h>		/* |DIGBUFLEN| */

#include	"propenqotd.h"


/* local defines */

#ifndef	VBUFLEN
#define	VBUFLEN		(2 * MAXPATHLEN)
#endif

#ifndef	EBUFLEN
#define	EBUFLEN		(3 * MAXPATHLEN)
#endif

#ifndef	VARPATH
#define	VARPATH		"PATH"
#endif

#ifndef	LOGCNAME
#define	LOGCNAME	"log"
#endif

#define	CONFIGFNAME	"conf"
#define	QCNAME		"qotd"

#ifndef	MAINTQOTD_SEARCHNAME
#define	MAINTQOTD_SEARCHNAME	"maintqotd"
#endif
#ifndef	MAINTQOTD_PROGEXEC
#define	MAINTQOTD_PROGEXEC	"qotd"
#endif
#ifndef	MAINTQOTD_VARSPOOL
#define	MAINTQOTD_VARSPOOL	"var/spool"
#endif

#define	MAINTQOTD_SEARCHNAME	"maintqotd"
#define	MAINTQOTD_PRNAME	"LOCAL"
#define	MAINTQOTD_PROGEXEC	"qotd"
#define	MAINTQOTD_PROGMKQOTD	"helloworld"
#define	MAINTQOTD_CONFMAGIC	0x8932170

#define	SI		subinfo
#define	SI_FL		subinfo_flags

#define	CONFIG		config


/* external subroutines */

#if	CF_DEBUGS
extern int	debugprintf(cchar *,...) ;
extern int	strlinelen(cchar *,int,int) ;
#endif /* CF_DEBUGS */


/* external variables */

extern cchar	**environ ;


/* local structures */

struct config {
	uint		magic ;
	SI		*sip ;
	paramfile	pf ;
	expcook		cooks ;
	uint		f_p:1 ;
	uint		f_cooks:1 ;
} ;

struct subinfo_flags {
	uint		stores:1 ;
	uint		id:1 ;
	uint		hosts:1 ;
	uint		sources:1 ;
	uint		logsub:1 ;
	uint		logsize:1 ;
	uint		spooldir:1 ;
	uint		lfname:1 ;
	uint		to:1 ;
} ;

struct subinfo_head {
	vecstr		stores ;
	vecpstr		hosts ;
	vecpstr		sources ;
	SI_FL		fl, have, changed, final ;
	SI_FL		open ;
	ids		id ;
	cchar		*pr ;
	cchar		*pn ;		/* program-name */
	cchar		*sn ;		/* search-name */
	cchar		*nn ;		/* node-name */
	cchar		*dn ;		/* domain-name */
	cchar		*un ;		/* user-name */
	cchar		*logid ;	/* log-ID */
	cchar		*lfname ;	/* log file-name */
	cchar		*hostname ;
	cchar		*spooldname ;	/* spool directory */
	cchar		*qdname ;	/* particular quote directory */
	void		*config ;	/* configuration state */
	void		*logsub ;	/* log-file state */
	uid_t		euid ;
	uid_t		uid_pr ;
	gid_t		gid_pr ;
	time_t		dt ;
	int		of ;
	int		to ;
	int		mjd ;
	int		logsize ;
	mode_t		om ;
} ;


/* forward references */

static int	subinfo_start(SI *,time_t,cchar *,int,int,int) noex ;
static int	subinfo_finish(SI *) noex ;
static int	subinfo_confbegin(SI *) noex ;
static int	subinfo_confend(SI *) noex ;
static int	subinfo_setentry(SI *,cchar **,cchar *,int) noex ;
static int	subinfo_envbegin(SI *) noex ;
static int	subinfo_envend(SI *) noex ;
static int	subinfo_logfile(SI *,cchar *,int) noex ;
static int	subinfo_hostname(SI *,cchar *,int) noex ;
static int	subinfo_source(SI *,cchar *,int) noex ;
static int	subinfo_logbegin(SI *) noex ;
static int	subinfo_logend(SI *) noex ;
static int	subinfo_logenv(SI *) noex ;
static int	subinfo_defaults(SI *) noex ;
static int	subinfo_spooldir(SI *,cchar *,int) noex ;
static int	subinfo_spoolcheck(SI *) noex ;
static int	subinfo_qdirname(SI *,int) noex ;
static int	subinfo_gather(SI *,cchar *,mode_t) noex ;
static int	subinfo_opensource(SI *,cchar *,cchar *) noex ;
static int	subinfo_opensourceprog(SI *,cchar *,cchar *) noex ;

static int subinfo_defprog(SI *,cchar *) noex ;
static int subinfo_defproger(SI *,vecstr *,cchar *,cchar *) noex ;
static int subinfo_addourpath(SI *,vecstr *) noex ;
static int subinfo_addprbins(SI *,vecstr *) noex ;
static int subinfo_addprbin(SI *,vecstr *,cchar *,cchar *) noex ;
static int subinfo_id(SI *) noex ;
static int subinfo_dircheck(SI *,cchar *) noex ;
static int subinfo_dirminmode(SI *,cchar *,mode_t) noex ;

static int	config_start(struct config *,SI *,cchar *) noex ;
static int	config_findfile(struct config *,char *,cchar *) noex ;
static int	config_cookbegin(struct config *) noex ;
static int	config_cookend(struct config *) noex ;
static int	config_read(struct config *) noex ;
static int	config_reader(struct config *,char *,int) noex ;
static int	config_finish(struct config *) noex ;

#if	CF_CONFIGCHECK
static int	config_check(struct config *) noex ;
#endif

static int	getdefmjd(time_t) noex ;
static int	mkqfname(char *,cchar *,int) noex ;

static int	setfname(SI *,char *,cchar *,int,
			int,cchar *,cchar *,cchar *) noex ;

static int	mkourname(char *,cchar *,cchar *,cchar *,int) noex ;

#if	CF_DEBUGS && CF_OPENDEF
static int opendef(SI *) noex ;
#endif

#if	CF_DEBUGS
static int debugmode(cchar *,cchar *,cchar *) noex ;
#endif

#if	CF_DEBUGS
static int debugfmode(cchar *,cchar *,int) noex ;
#endif


/* local variables */

constexpr cpcchar	csched[] = {
	"%p/%e/%n/%n.%f",
	"%p/%e/%n/%f",
	"%p/%e/%n.%f",
	"%p/%n.%f",
	nullptr
} ;

enum cparams {
	cparam_spooldir,
	cparam_logsize,
	cparam_logfile,
	cparam_hostname,
	cparam_svcname,
	cparam_to,
	cparam_source,
	cparam_overlast
} ;

constexpr cpcchar	cparams[] = {
	"spooldir",
	"logsize",
	"logfile",
	"hostname",
	"svcname",
	"to",
	"source",
	nullptr
} ;

enum sources {
	source_prog,
	source_systems,
	source_uqotd,
	source_overlast
} ;

constexpr cpcchar	sources[] = {
	"prog",
	"systems",
	"uqotd",
	nullptr
} ;

constexpr cpcchar	prbins[] = {
	"bin",
	"sbin",
	nullptr
} ;

constexpr cpcchar	defprogs[] = {
	"mkqotd",
	"fortune",
	"/swd/local/bin/fortune",
	"/usr/extra/bin/fortune",
	"/usr/games/fortune",
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int maintqotd(cchar *pr,int mjd,int of,int to) noex {
	MAINTQOTD	si, *sip = &si ;
	time_t		dt = 0 ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		fd = -1 ;

	if (pr == nullptr) return SR_FAULT ;

	if (mjd <= 0) {
	    if (dt == 0) dt = getustime ;
	    rs = getdefmjd(dt) ;
	    mjd = rs ;
	}

	if (rs >= 0) {
	    if ((rs = subinfo_start(sip,dt,pr,of,to,mjd)) >= 0) {
	        if ((rs = subinfo_defaults(sip)) >= 0) {
	            if ((rs = subinfo_logbegin(sip)) >= 0) {
	                if ((rs = subinfo_spoolcheck(sip)) >= 0) {
	                    if ((rs = subinfo_qdirname(sip,mjd)) >= 0) {
	                        cchar	*qd = sip->qdname ;
	                        char	qfname[MAXPATHLEN+1] ;
	                        if ((rs = mkqfname(qfname,qd,mjd)) >= 0) {
	                            cmode	om = 0664 ;

	                            of &= (~ OM_SPECIAL) ;
    
	                            rs = u_open(qfname,of,om) ;
	                            fd = rs ;
	                            if (rs == SR_NOENT) {
	                                rs = subinfo_gather(sip,qfname,om) ;
	                                fd = rs ;
					if (rs < 0) {
					    uc_unlink(qfname) ;
					}
	                            } /* end if (NOENT) */

	                        } /* end if (mkqfname) */
			    } /* end if (qdirname) */
	                } /* end if (spoolcheck) */
	                rs1 = subinfo_logend(sip) ;
	        	if (rs >= 0) rs = rs1 ;
		    } /* end if (logging) */
	        } /* end if (defaults) */
	        rs1 = subinfo_finish(sip) ;
	        if (rs >= 0) rs = rs1 ;
	        if ((rs < 0) && (fd >= 0)) u_close(fd) ;
	    } /* end if (subinfo) */
	} /* end if (ok) */

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (maintqotd) */


/* local subroutines */

static int subinfo_start(SI *sip,time_t dt,cchar *pr,
		int of,int to,int mjd) noex {
	USTAT		sb ;
	int		rs ;

	if (dt == 0) dt = getustime ;

	memclear(sip) ;
	sip->pr = pr ;
	sip->of = of ;
	sip->om = 0666 ;
	sip->to = to ;
	sip->mjd = mjd ;
	sip->sn = MAINTQOTD_SEARCHNAME ;
	sip->dt = dt ;
	sip->euid = geteuid() ;

	sip->fl.logsub = true ;

	if ((rs = u_stat(pr,&sb)) >= 0) {
	    sip->uid_pr = sb.st_uid ;
	    sip->gid_pr = sb.st_gid ;

	    if ((rs = subinfo_envbegin(sip)) >= 0) {
	        if ((rs = subinfo_confbegin(sip)) >= 0) {
		    cint	llen = LOGIDLEN ;
		    cint	v = (int) ugetpid() ;
		    cchar	*nn = sip->nn ;
		    char	lbuf[LOGIDLEN+1] ;
		    if ((rs = mklogid(lbuf,llen,nn,5,v)) >= 0) {
			cchar	**vpp = &sip->logid ;
			rs = subinfo_setentry(sip,vpp,lbuf,rs) ;
		    }
		    if (rs < 0) {
	        	subinfo_confend(sip) ;
		    }
		}
	        if (rs < 0)
	            subinfo_envend(sip) ;
	    }
	    if (rs < 0) {
	        if (sip->open.stores) {
	            sip->open.stores = false ;
	            vecstr_finish(&sip->stores) ;
	        }
	    }

	} /* end if (stat-pr) */

	return rs ;
}
/* end subroutine (subinfo_start) */

static int subinfo_finish(SI *sip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	rs1 = subinfo_confend(sip) ;
	if (rs >= 0) rs = rs1 ;
	}
	if (sip->open.sources) {
	    sip->open.sources = false ;
	    rs1 = vecpstr_finish(&sip->sources) ;
	    if (rs >= 0) rs = rs1 ;
	}
	if (sip->open.hosts) {
	    sip->open.hosts = false ;
	    rs1 = vecpstr_finish(&sip->hosts) ;
	    if (rs >= 0) rs = rs1 ;
	}
	if (sip->open.id) {
	    sip->open.id = false ;
	    rs1 = ids_release(&sip->id) ;
	    if (rs >= 0) rs = rs1 ;
	}
	if (sip->open.stores) {
	    sip->open.stores = false ;
	    rs1 = vecstr_finish(&sip->stores) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (subinfo_finish) */

int subinfo_setentry(SI *lip,cchar **epp,cchar *vp,int vl) noex {
	int		rs = SR_OK ;
	int		vnlen = 0 ;

	if (lip == nullptr) return SR_FAULT ;
	if (epp == nullptr) return SR_FAULT ;

	if (! lip->open.stores) {
	    rs = vecstr_start(&lip->stores,4,0) ;
	    lip->open.stores = (rs >= 0) ;
	}

	if (rs >= 0) {
	    int	oi = -1 ;

	    if (*epp != nullptr)
	        oi = vecstr_findaddr(&lip->stores,*epp) ;

	    if (vp != nullptr) {
	        vnlen = strnlen(vp,vl) ;
	        rs = vecstr_store(&lip->stores,vp,vnlen,epp) ;
	    } else
		*epp = nullptr ;

	    if ((rs >= 0) && (oi >= 0))
	        vecstr_del(&lip->stores,oi) ;

	} /* end if (ok) */

	return (rs >= 0) ? vnlen : rs ;
}
/* end subroutine (subinfo_setentry) */

static int subinfo_envbegin(SI *sip) noex {
	int		rs = SR_OK ;
	int		cl ;
	cchar		*cp ;

#if	CF_DEBUGS
	debugprintf("maintqotd/subinfo_envbegin: ent\n") ;
#endif

	{
	    cint	elen = MAXPATHLEN ;
	    int		el = -1 ;
	    cchar	*en ;
	    char	ebuf[MAXPATHLEN+1] ;
	    en = ebuf ;
	    if ((rs = getprogexec(ebuf,elen)) == SR_NOSYS) {
	        rs = SR_OK ;
	        en = MAINTQOTD_PROGEXEC ;
	    } else
	        el = rs ;
	    if (rs >= 0) {
	        cchar	**vpp = &sip->pn ;
	        if ((cl = sfbasename(en,el,&cp)) > 0) {
		    cchar	*tp ;
		    if ((tp = strnchr(cp,cl,'.')) != nullptr) cl = (tp-cp) ;
	            rs = subinfo_setentry(sip,vpp,cp,cl) ;
	        }
	    }
	}

	if (rs >= 0) {
	    char	nn[NODENAMELEN+1] ;
	    char	dn[MAXHOSTNAMELEN+1] ;
	    if ((rs = getnodedomain(nn,dn)) >= 0) {
	        cchar	**vpp = &sip->nn ;
#if	CF_DEBUGS
	        debugprintf("maintqotd/subinfo_envbegin: nn=%s\n",nn) ;
	        debugprintf("maintqotd/subinfo_envbegin: dn=%s\n",dn) ;
#endif
	        if ((rs = subinfo_setentry(sip,vpp,nn,-1)) >= 0) {
	            cchar	**vpp = &sip->dn ;
	            rs = subinfo_setentry(sip,vpp,dn,-1) ;
	        }
	    }
	}

	if (rs >= 0) {
	    cint	ulen = USERNAMELEN ;
	    char	ubuf[USERNAMELEN+1] ;
	    if ((rs = getusername(ubuf,ulen,-1)) >= 0) {
	        cchar	**vpp = &sip->un ;
	        rs = subinfo_setentry(sip,vpp,ubuf,rs) ;
	    }
	}

	return rs ;
}
/* end subroutine (subinfo_envbegin) */

static int subinfo_envend(SI *sip) noex {
	int		rs = SR_OK ;
	if (sip == nullptr) return SR_FAULT ;
	return rs ;
}
/* end subroutine (subinfo_envend) */

static int subinfo_confbegin(SI *sip) noex {
	cint		csz = szof(struct config)  ;
	int		rs = SR_OK ;
	cchar		*cfname = CONFIGFNAME ;
	void		*p ;

#if	CF_DEBUGS
	    debugprintf("maintqotd/subinfo_confbegin: ent\n") ;
#endif

	if ((rs = uc_malloc(csz,&p)) >= 0) {
	    CONFIG	*csp = p ;
	    sip->config = csp ;
	    if ((rs = config_start(csp,sip,cfname)) >= 0) {
	        if ((rs = config_read(csp)) >= 0) {
	            rs = 1 ;
	        }
	        if (rs < 0)
	            config_finish(csp) ;
	    } /* end if (config) */
	    if (rs < 0) {
	        uc_free(p) ;
	        sip->config = nullptr ;
	    }
	} /* end if (memory-allocation) */

#if	CF_DEBUGS
	    debugprintf("maintqotd/subinfo_confbegin: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (subinfo_confbegin) */

static int subinfo_confend(SI *sip) noex {
	int		rs = SR_OK ;
	int		rs1 ;

#if	CF_DEBUGS
	    debugprintf("maintqotd/subinfo_confend: config=%u\n",
	        (sip->config != nullptr)) ;
#endif

	if (sip->config != nullptr) {
	    CONFIG	*csp = sip->config ;
	    rs1 = config_finish(csp) ;
	    if (rs >= 0) rs = rs1 ;
	    rs1 = uc_free(sip->config) ;
	    if (rs >= 0) rs = rs1 ;
	    sip->config = nullptr ;
	}

	return rs ;
}
/* end subroutine (subinfo_confend) */

static int subinfo_defaults(SI *sip) noex {
	int		rs = SR_OK ;

	if (sip->spooldname == nullptr) {
	    cchar	*vp = sip->sn ;
	    cint	vl = -1 ;
	    rs = subinfo_spooldir(sip,vp,vl) ;
	}

	{
	    cchar	*lf = sip->lfname ;
	    if (((lf == nullptr) || (lf[0] == '+')) && sip->fl.logsub) {
	        cchar	*vp = sip->sn ;
	        cint	vl = -1 ;
	        rs = subinfo_logfile(sip,vp,vl) ;
	    }
	}

#if	CF_DEBUGS
	debugprintf("subinfo_defaults: lfname=%s\n",sip->lfname) ;
#endif

	return rs ;
}
/* end subroutine (subinfo_defaults) */

static int subinfo_spooldir(SI *sip,cchar *vp,int vl) noex {
	int		rs ;
	cchar		*pr = sip->pr ;
	cchar		*inter = MAINTQOTD_VARSPOOL ;
	char		tbuf[MAXPATHLEN+1] ;

	if ((rs = mkourname(tbuf,pr,inter,vp,vl)) >= 0) {
	    cchar	**vpp = &sip->spooldname ;
	    rs = subinfo_setentry(sip,vpp,tbuf,rs) ;
	}

	return rs ;
}
/* end subroutine (subinfo_spooldir) */

static int subinfo_logfile(SI *sip,cchar *vp,int vl) noex {
	int		rs ;
	cchar		*pr = sip->pr ;
	cchar		*inter = LOGCNAME ;
	char		tbuf[MAXPATHLEN+1] ;

	if ((rs = mkourname(tbuf,pr,inter,vp,vl)) >= 0) {
	    cchar	**vpp = &sip->lfname ;
	    rs = subinfo_setentry(sip,vpp,tbuf,rs) ;
	}

	return rs ;
}
/* end subroutine (subinfo_logfile) */

static int subinfo_hostname(SI *sip,cchar *vp,int vl) noex {
	int	rs = SR_OK ;

	if (! sip->open.hosts)  {
	    rs = vecpstr_start(&sip->hosts,0,0,0) ;
	    sip->open.hosts = (rs >= 0) ;
	}

	if ((rs >= 0) && (vp != nullptr)) {
	    rs = vecpstr_adduniq(&sip->hosts,vp,vl) ;
	}

	return rs ;
}
/* end subroutine (subinfo_hostname) */

static int subinfo_source(SI *sip,cchar *vp,int vl) noex {
	int		rs = SR_OK ;

	if (! sip->open.sources)  {
	    rs = vecpstr_start(&sip->sources,0,0,0) ;
	    sip->open.sources = (rs >= 0) ;
	}

	if ((rs >= 0) && (vp != nullptr)) {
	    rs = vecpstr_adduniq(&sip->sources,vp,vl) ;
	}

	return rs ;
}
/* end subroutine (subinfo_source) */

static int subinfo_logbegin(SI *sip) noex {
	int		rs = SR_OK ;
	cchar		*lf = sip->lfname ;

	if ((lf != nullptr) && (lf[0] != '-')) {
	    cint	size = szof(LOGFILE) ;
	    void	*p ;
	    if ((rs = uc_malloc(size,&p)) >= 0) {
		LOGFILE	*lhp = p ;
	        cchar	*logid = sip->logid ;
	        sip->logsub = p ;
#if	CF_DEBUGS
	debugprintf("maintqotd/_logbegin: lf=%s\n",lf) ;
	debugprintf("maintqotd/_logbegin: logid=%s\n",logid) ;
#endif /* CF_DEBUGS */
	        if ((rs = logfile_open(lhp,lf,0,0666,logid)) >= 0) {
		    sip->open.logsub = true ;
		    rs = subinfo_logenv(sip) ;
		    if (rs < 0) {
			sip->open.logsub = false ;
			logfile_close(lhp) ;
		    }
		} else if (isNotPresent(rs)) {
		    rs = SR_OK ;
		}
#if	CF_DEBUGS
	debugprintf("maintqotd/_logbegin: logfile_open-out rs=%d\n",rs) ;
#endif /* CF_DEBUGS */
		if (rs < 0) {
		    uc_free(sip->logsub) ;
		    sip->logsub = nullptr ;
		}
	    } /* end if (memory-allocation) */
	} /* end if (log-file) */

#if	CF_DEBUGS
	debugprintf("maintqotd/_logbegin: ret rs=%d\n",rs) ;
#endif /* CF_DEBUGS */

	return rs ;
}
/* end subroutine (subinfo_logbegin) */

static int subinfo_logend(SI *sip) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (sip->logsub != nullptr) {
	    LOGFILE	*lhp = sip->logsub ;
	    rs1 = logfile_close(lhp) ;
	    if (rs >= 0) rs = rs1 ;
	    rs1 = uc_free(sip->logsub) ;
	    if (rs >= 0) rs = rs1 ;
	    sip->logsub = nullptr ;
	}

	return rs ;
}
/* end subroutine (subinfo_logend) */

static int subinfo_logenv(SI *sip) noex {
	int		rs = SR_OK ;
	char		tbuf[TIMEBUFLEN+1] ;

	if (sip->open.logsub) {
	    LOGFILE	*lhp = sip->logsub ;
	    timestr_logz(sip->dt,tbuf) ;
	    logfile_printf(lhp,"%s %s %s!%s",tbuf,sip->dn,sip->nn,sip->un) ;
#ifdef	COMMENT
	    logfile_printf(lhp,"pr=%s",sip->pr) ;
	    logfile_printf(lhp,"pn=%s",sip->pn) ;
#endif /* COMMENT */
	    logfile_printf(lhp,"mjd=%d",sip->mjd) ;
	}

	return rs ;
}
/* end subroutine (subinfo_logenv) */

static int subinfo_spoolcheck(SI *sip) noex {
	int		rs ;
	cchar		*sdname = sip->spooldname ;
	rs = subinfo_dircheck(sip,sdname) ;
	return rs ;
}
/* end subroutine (subinfo_spoolcheck) */

static int subinfo_qdirname(SI *sip,int mjd) noex {
	cint		dlen = DIGBUFLEN ;
	cint		prec = 3 ; /* digit precision for another 100 years */
	int		rs ;
	int		len = 0 ;
	cchar		*sdname = sip->spooldname ;
	char		dbuf[DIGBUFLEN+1] ;

#if	CF_DEBUGS
	debugprintf("maintqotd/subinfo_qdirname: ent mjd=%u\n",mjd) ;
#endif

	if ((rs = ctdecpi((dbuf+1),(dlen-1),prec,(mjd/100))) > 0) {
	    char	tbuf[MAXPATHLEN+1] ;
	    dbuf[0] = 'd' ;
	    if ((rs = mkpath2w(tbuf,sdname,dbuf,(rs+1))) >= 0) {
	        cchar	**vpp = &sip->qdname ;
		len = rs ;
		if ((rs = subinfo_setentry(sip,vpp,tbuf,len)) >= 0) {
		    rs = subinfo_dircheck(sip,tbuf) ;
		} /* end if (subinfo_setentry) */
	    } /* end if (mkpath) */
	} /* end if (ctdeci) */

#if	CF_DEBUGS
	debugprintf("maintqotd/subinfo_qdirname: ret rs=%d len=%u\n",rs,len) ;
#endif

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subinfo_qdirname) */

#if	CF_SOURCES
static int subinfo_gather(SI *sip,cchar *qfname,mode_t om) noex {
	int		rs = SR_OK ;
	int		fd = -1 ;

#if	CF_DEBUGS
	debugprintf("maintqotd/subinfo_gather: ent\n") ;
	debugprintf("maintqotd/subinfo_gather: qf=%s\n",qfname) ;
#endif

	if (sip->open.sources) {
	    VECPSTR	*slp = &sip->sources ;
	    int		i ;
	    cchar	*sep ;
#if	CF_DEBUGS
	debugprintf("maintqotd/subinfo_gather: n=%d\n",vecpstr_count(slp)) ;
#endif
	    for (i = 0 ; (rs = vecpstr_get(slp,i,&sep)) >= 0 ; i += 1) {
	        if (sep != nullptr) {
	            rs = subinfo_opensource(sip,qfname,sep) ;
	            fd = rs ;
	        }
	        if (rs >= 0) break ;
	        if (! isNotPresent(rs)) break ;
	    } /* end for */
	    if (isNotPresent(rs)) {
	        rs = subinfo_defprog(sip,qfname) ;
	        fd = rs ;
	    }
	} else {
	    rs = subinfo_defprog(sip,qfname) ;
	    fd = rs ;
	}
#if	CF_DEBUGS
	debugprintf("maintqotd/subinfo_gather: mid rs=%d fd=%u\n",rs,fd) ;
#endif
	if (rs >= 0) {
	    if ((rs = u_rewind(fd)) >= 0) {
		if ((rs = uc_fminmod(fd,om)) >= 0) {
		    uid_t	u = sip->uid_pr ;
		    gid_t	g = sip->gid_pr ;
		    if ((rs = u_fchown(fd,u,g)) == SR_PERM) rs = SR_OK ;
		}
	    } /* end if (rewind) */
	    if (rs < 0) u_close(fd) ;
	} /* end if (got a source) */
#if	CF_DEBUGS
	debugprintf("maintqotd/subinfo_gather: ret rs=%d\n",rs) ;
#endif
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (subinfo_gather) */
#else /* CF_SOURCES */
static int subinfo_gather(SI *sip,cchar *qfname,mode_t om) noex {
	cint		of = (O_RDWR|O_CREAT|O_TRUNC) ;
	int		rs ;
	int		fd = -1 ;
	cmode		om = 0664 ;
	if ((rs = u_open(qfname,of,om)) >= 0) {
	    cchar	*mp = "hello world!\n" ;
	    int		ml ;
	    fd = rs ;
	    ml = strlen(mp) ;
	    rs = u_write(fd,mp,ml) ;
	    if (rs >= 0) rs = u_rewind(fd) ;
	    if (rs < 0) u_close(fd) ;
	}
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (subinfo_gather) */
#endif /* CF_SOURCES */

static int subinfo_opensource(SI *sip,cchar *qf,cchar *sep) noex {
	int		rs = SR_OK ;
	int		fd = -1 ;
	int		sl = -1 ;
	int		si ;
	cchar		*sp = sep ;
	cchar		*ap ;
	cchar		*tp ;
	if ((tp = strchr(sp,CH_FS)) != nullptr) {
	    sl = (tp-sp) ;
	    ap = (tp+1) ;
	} else {
	    ap = (sp + strlen(sp)) ;
	}
#if	CF_DEBUGS
	debugprintf("maintqotd/subinfo_opensource: svc=%r\n",sp,sl) ;
	debugprintf("maintqotd/subinfo_opensource: a=%s\n",ap) ;
#endif

	if ((si = matostr(sources,3,sp,sl)) >= 0) {
	    switch (si) {
	    case source_prog:
	        rs = subinfo_opensourceprog(sip,qf,ap) ;
	        break ;
	    default:
	        rs = SR_NOENT ;
	        break ;
	    } /* end switch */
	    fd = rs ;
	} else
	    rs = SR_NOENT ;

#if	CF_DEBUGS && CF_OPENDEF
	if (rs == SR_NOENT) {
	    rs = opendef(sip) ;
	    fd = rs ;
	}
#endif

#if	CF_DEBUGS
	debugprintf("maintqotd/subinfo_opensource: ret rs=%d fd=%u\n",rs,fd) ;
#endif

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (subinfo_opensource) */

static int subinfo_opensourceprog(SI *sip,cchar *qf,cchar *ap) noex {
	int		rs ;
	int		fd = -1 ;

#if	CF_DEBUGS
	debugprintf("maintqotd/subinfo_opensourceprog: ent\n") ;
	debugprintf("maintqotd/subinfo_opensourceprog: a=%s\n",ap) ;
#endif

	rs = maintqotd_prog(sip,qf,ap) ;
	fd = rs ;

#if	CF_DEBUGS
	if (rs >= 0)
	debugfmode("maintqotd/subinfo_opensourceprog","_prog()",fd) ;
	debugprintf("maintqotd/subinfo_opensourceprog: _prog() rs=%d\n",rs) ;
#endif

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (subinfo_opensourceprog) */

static int subinfo_defprog(SI *sip,cchar *qfn) noex {
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	if ((rs = subinfo_id(sip)) >= 0) {
	    vecstr	path, *plp = &path ;
	    if ((rs = vecstr_start(plp,5,0)) >= 0) {
	        if ((rs = subinfo_addourpath(sip,plp)) >= 0) {
	            int		i ;
	            for (i = 0 ; defprogs[i] != nullptr ; i += 1) {
	                cchar	*prog = defprogs[i] ;
	                if ((rs = subinfo_defproger(sip,plp,prog,qfn)) >= 0) {
			    fd = rs ;
			    break ;
		        } else if (isNotPresent(rs)) {
			    rs = SR_OK ;
			}
			if (rs < 0) break ;
	            } /* end for */
		    if ((rs >= 0) && (fd < 0)) rs = SR_NOENT ;
	        } /* end if (subinfo_addourpath) */
	        rs1 = vecstr_finish(plp) ;
	        if (rs >= 0) rs = rs1 ;
	        if ((rs < 0) && (fd >= 0)) u_close(fd) ;
	    } /* end if (vecstr) */
	} /* end if (subinfo_id) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (subinfo_defprog) */

static int subinfo_defproger(SI *sip,vecstr *plp,cc *prog,cc *qfn) noex {
	ids		*idp = &sip->id ;
	int		rs ;
	int		fd = -1 ;
	char		rbuf[MAXPATHLEN+1] ;
	if ((rs = getprogpath(idp,plp,rbuf,prog,-1)) >= 0) {
	    cint	alen = MAXNAMELEN ;
	    cint	rl = rs ;
	    char	abuf[MAXNAMELEN+1] ;
	    if (rl == 0) rs = mkpath1(rbuf,prog) ;
	    if (rs >= 0) {
		int	cl ;
		cchar	*cp ;
	        if ((cl = sfbasename(prog,-1,&cp)) > 0) {
		    if ((rs = sncpy1w(abuf,alen,cp,cl)) >= 0) {
			cint	of = O_RDONLY ;
		        cchar		*av[2] ;
		        cchar		**ev = nullptr ;
	                av[0] = abuf ;
		        av[1] = nullptr ;
	                if ((rs = uc_openprog(rbuf,of,av,ev)) >= 0) {
			    cmode	om = 0664 ;
			    cint		qof = (O_CREAT|O_TRUNC|O_RDWR) ;
		            cint		pfd = rs ;
			    if ((rs = uc_open(qfn,qof,om)) >= 0) {
				fd = rs ;
				rs = uc_writedesc(fd,pfd,-1) ;
				if (rs < 0) {
				    u_close(fd) ;
				    fd = -1 ;
				}
			    } /* end if (uc_open) */
			    u_close(pfd) ;
		        } /* end if (uc_openprog) */
	            } /* end if (sncpy) */
	        } else
		    rs = SR_NOENT ;
	    } /* end if (mkpath) */
	} /* end if (getprogpath) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (subinfo_defproger) */

static int subinfo_addourpath(SI *sip,vecstr *plp) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	cchar		*path = getenv(VARPATH) ;
	if (path != nullptr) {
	    rs = vecstr_addpathclean(plp,path,-1) ;
	    c += rs ;
	}
	if (rs >= 0) {
	    rs = subinfo_addprbins(sip,plp) ;
	    c += rs ;
	}
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (subinfo_addourpath) */

static int subinfo_addprbins(SI *sip,vecstr *plp) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	cchar		*pr = sip->pr ;
	for (int i = 0 ; (rs >= 0) && (prbins[i] != nullptr) ; i += 1) {
	    cchar	*prbin = prbins[i] ;
	    rs = subinfo_addprbin(sip,plp,pr,prbin) ;
	    c += rs ;
	} /* end for */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (subinfo_addprbins) */

static int subinfo_addprbin(SI *sip,vecstr *plp,cchar *pr,
		cchar *prbin) noex {
	int		rs ;
	int		c = 0 ;
	char		tbuf[MAXPATHLEN+1] ;
	if ((rs = mkpath2(tbuf,pr,prbin)) >= 0) {
	    USTAT		sb ;
	    cint		tl = rs ;
	    if ((rs = u_stat(tbuf,&sb)) >= 0) {
		if (S_ISDIR(sb.st_mode)) {
		    cint	am = (R_OK|X_OK) ;
		    if ((rs = permid(&sip->id,&sb,am)) >= 0) {
			rs = vecstr_adduniq(plp,tbuf,tl) ;
			if (rs < INT_MAX) c += 1 ;
		    } else if (isNotPresent(rs)) {
			rs = SR_OK ;
		    }
		} /* end if (is-dir) */
	    } else if (isNotPresent(rs)) {
		rs = SR_OK ;
	    }
	} /* end if (mkpath) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (subinfo_addprbin) */

static int subinfo_id(SI *sip) noex {
	int		rs = SR_OK ;
	if (! sip->open.id) {
	    sip->open.id = true ;
	    rs = ids_load(&sip->id) ;
	}
	return rs ;
}
/* end subroutine (subinfo_id) */

static int subinfo_dircheck(SI *sip,cchar *dname) noex {
	USTAT		sb ;
	const uid_t	euid = sip->euid ;
	cint		nrs = SR_NOENT ;
	int		rs ;
	cmode		dm = (0777 | S_ISGID) ;

	if ((rs = u_stat(dname,&sb)) >= 0) {
	    if (sb.st_uid == euid) {
		rs = subinfo_dirminmode(sip,dname,dm) ;
	    }
	} else if (rs == nrs) {
	    if ((rs = mkdirs(dname,dm)) >= 0) {
		rs = subinfo_dirminmode(sip,dname,dm) ;
	    } /* end if (mkdirs) */
	} /* end if (stat) */

	return rs ;
}
/* end subroutine (subinfo_dircheck) */

static int subinfo_dirminmode(SI *sip,cchar *dname,mode_t dm) noex {
	const uid_t	euid = sip->euid ;
	int		rs ;
	if ((rs = uc_minmod(dname,dm)) >= 0) {
	    if (sip->uid_pr != euid) {
		u_chown(dname,sip->uid_pr,sip->gid_pr) ;
	    }
	} /* end if (uc_minmod) */
	return rs ;
}
/* end subroutine (subinfo_dirminmode) */

static int config_start(CONFIG *csp,SI *sip,cchar *cfname) noex {
	int		rs ;
	char		tmpfname[MAXPATHLEN+1] = { 0 } ;

	if (cfname == nullptr) return SR_FAULT ;

	memset(csp,0,szof(struct config)) ;
	csp->sip = sip ;

	if ((rs = config_findfile(csp,tmpfname,cfname)) >= 0) {
	    cchar	**envv = (cchar **) environ ;
	    if (rs > 0) cfname = tmpfname ;

#if	CF_DEBUGS
	    debugprintf("maintqotd/config_start: mid rs=%d cfname=%s\n",
		rs,cfname) ;
#endif

	    if ((rs = paramfile_open(&csp->p,envv,cfname)) >= 0) {
	        if ((rs = config_cookbegin(csp)) >= 0) {
	            csp->f_p = true ;
	        }
	        if (rs < 0)
	            paramfile_close(&csp->p) ;
	    } else if (isNotPresent(rs))
	        rs = SR_OK ;
	} else if (isNotPresent(rs))
	    rs = SR_OK ;

	if (rs >= 0) csp->magic = MAINTQOTD_CONFMAGIC ;

#if	CF_DEBUGS
	debugprintf("maintqotd/config_start: ret rs=%d f=%u\n",rs,csp->f_p) ;
#endif

	return rs ;
}
/* end subroutine (config_start) */

static int config_findfile(CONFIG *csp,char tbuf[],cchar *cfname) noex {
	MAINTQOTD	*sip = csp->sip ;
	int		rs ;
	int		pl = 0 ;

	tbuf[0] = '\0' ;
	if (vecstr sv ; (rs = vecstr_start(&sv,6,0)) >= 0) {
	    cint	tlen = MAXPATHLEN ;

	    vecstr_envset(&sv,"p",sip->pr,-1) ;
	    vecstr_envset(&sv,"e","etc",-1) ;
	    vecstr_envset(&sv,"n",sip->sn,-1) ;

	    rs = permsched(csched,&sv,tbuf,tlen,cfname,R_OK) ;
	    pl = rs ;

	    vecstr_finish(&sv) ;
	} /* end if (finding file) */

	return (rs >= 0) ? pl : rs ;
}
/* end subroutine (config_findfile) */

static int config_cookbegin(CONFIG *csp) noex {
	MAINTQOTD	*sip = csp->sip ;
	cint	hlen = MAXHOSTNAMELEN ;
	int		rs ;
	char		hbuf[MAXHOSTNAMELEN+1] ;

	if ((rs = expcook_start(&csp->cooks)) >= 0) {
	    int		i ;
	    int		kch ;
	    int		vl ;
	    cchar	*ks = "PSNDHRU" ;
	    cchar	*vp ;
	    char	kbuf[2] ;

	    kbuf[1] = '\0' ;
	    for (i = 0 ; (rs >= 0) && (ks[i] != '\0') ; i += 1) {
	        kch = MKCHAR(ks[i]) ;
	        vp = nullptr ;
	        vl = -1 ;
	        switch (kch) {
	        case 'P':
	            vp = sip->pn ;
	            break ;
	        case 'S':
	            vp = sip->sn ;
	            break ;
	        case 'N':
	            vp = sip->nn ;
	            break ;
	        case 'D':
	            vp = sip->dn ;
	            break ;
	        case 'H':
	            {
	                cchar	*nn = sip->nn ;
	                cchar	*dn = sip->dn ;
	                rs = snsds(hbuf,hlen,nn,dn) ;
	                vl = rs ;
	                vp = hbuf ;
	            }
	            break ;
	        case 'R':
	            vp = sip->pr ;
	            break ;
	        case 'U':
	            vp = sip->un ;
	            break ;
	        } /* end switch */
	        if ((rs >= 0) && (vp != nullptr)) {
	            kbuf[0] = kch ;
	            rs = expcook_add(&csp->cooks,kbuf,vp,vl) ;
	        }
	    } /* end for */

	    if (rs >= 0) {
	        if ((vl = sfbasename(sip->pr,-1,&vp)) > 0) {
	            rs = expcook_add(&csp->cooks,"RN",vp,vl) ;
	        }
	    }

	    if (rs >= 0) {
	        if ((rs = ctdeci(hbuf,hlen,sip->mjd)) >= 0) {
	            rs = expcook_add(&csp->cooks,"MJD",hbuf,rs) ;
	        }
	    }

	    if (rs >= 0) {
	        csp->f_cooks = true ;
	    } else
	        expcook_finish(&csp->cooks) ;
	} /* end if (expcook_start) */

	return rs ;
}
/* end subroutine (config_cookbegin) */

static int config_cookend(CONFIG *csp) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (csp->f_cooks) {
	    csp->f_cooks = false ;
	    rs1 = expcook_finish(&csp->cooks) ;
	    if (rs >= 0) rs = rs1 ;
	}

	return rs ;
}
/* end subroutine (config_cookend) */

static int config_finish(CONFIG *csp) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (csp == nullptr) return SR_FAULT ;
	if (csp->magic != MAINTQOTD_CONFMAGIC) return SR_NOTOPEN ;

	if (csp->f_p) {

	    if (csp->f_cooks) {
	        rs1 = config_cookend(csp) ;
	        if (rs >= 0) rs = rs1 ;
	    }

	    rs1 = paramfile_close(&csp->p) ;
	    if (rs >= 0) rs = rs1 ;

	    csp->f_p = false ;
	} /* end if */

	return rs ;
}
/* end subroutine (config_finish) */

#if	CF_CONFIGCHECK
static int config_check(CONFIG *csp) noex {
	MAINTQOTD	*sip = csp->sip ;
	int		rs = SR_OK ;

	if (csp == nullptr) return SR_FAULT ;
	if (csp->magic != MAINTQOTD_CONFMAGIC) return SR_NOTOPEN ;

	if (csp->f_p) {
	    time_t	dt = sip->dt ;
	    if ((rs = paramfile_check(&csp->p,dt)) > 0)
	        rs = config_read(csp) ;
	}

	return rs ;
}
/* end subroutine (config_check) */
#endif /* CF_CONFIGCHECK */

static int config_read(CONFIG *csp) noex {
	MAINTQOTD	*sip = csp->sip ;
	int		rs = SR_OK ;

	if (csp == nullptr) return SR_FAULT ;
	if (csp->magic != MAINTQOTD_CONFMAGIC) return SR_NOTOPEN ;

	sip = csp->sip ;
	if (sip == nullptr) return SR_FAULT ;

	if (csp->f_p) {
	    cint	elen = EBUFLEN ;
	    char	*ebuf ;
	    if ((rs = uc_malloc((elen+1),&ebuf)) >= 0) {
		rs = config_reader(csp,ebuf,elen) ;
		uc_free(ebuf) ;
	    } /* end if (memory-allocation) */
	} /* end if (avtive) */

#if	CF_DEBUGS
	debugprintf("maintqotd/config_read: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (config_read) */

static int config_reader(CONFIG *csp,char *ebuf,int elen) noex {
	MAINTQOTD	*sip = csp->sip ;
	PARAMFILE	*pfp = &csp->p ;
	PARAMFILE_CUR	cur ;
	cint	vlen = VBUFLEN ;
	int		rs = SR_OK ;
	int		i ;
	int		vl, el ;
	int		v ;
	int		ml ;
	int		c = 0 ;
	char		vbuf[VBUFLEN + 1] ;

#if	CF_DEBUGS
	debugprintf("maintqotd/config_reader: ent f_active=%u\n",
		csp->f_p) ;
#endif
	if (sip == nullptr) return SR_FAULT ;

	if (csp->f_p) {
	    for (i = 0 ; cparams[i] != nullptr ; i += 1) {
		cchar	*cparam = cparams[i] ;

#if	CF_DEBUGS
	        debugprintf("mqintqotd/config_read: cparam=%s\n",cparam) ;
#endif

	        if ((rs = paramfile_curbegin(pfp,&cur)) >= 0) {

	            while (rs >= 0) {
	                vl = paramfile_fetch(pfp,cparam,&cur,vbuf,vlen) ;
#if	CF_DEBUGS
	                debugprintf("mqintqotd/config_read: "
			"paramfile_fetch() rs=%d\n",vl) ;
#endif
	                if (vl == SR_NOTFOUND) break ;
	                rs = vl ;
	                if (rs < 0) break ;

#if	CF_DEBUGS
	                    debugprintf("mqintqotd/config_read: "
				"vbuf=>%r<\n",vbuf,vl) ;
#endif

	                ebuf[0] = '\0' ;
	                el = 0 ;
	                if (vl > 0) {
	                    el = expcook_exp(&csp->cooks,0,ebuf,elen,vbuf,vl) ;
	                    if (el >= 0) ebuf[el] = '\0' ;
	                }

#if	CF_DEBUGS
	                debugprintf("maintqotd/config_read: "
				"ebuf=>%r<\n",ebuf,el) ;
#endif

	                if (el > 0) {
	                    cchar	*sn = sip->sn ;
	                    char	tbuf[MAXPATHLEN + 1] ;

			    c += 1 ;
	                    switch (i) {

	                    case cparam_logsize:
	                        if (cfdecmfi(ebuf,el,&v) >= 0) {
	                            if (v >= 0) {
	                                switch (i) {
	                                case cparam_logsize:
	                                    sip->logsize = v ;
	                                    break ;
	                                } /* end switch */
	                            }
	                        } /* end if (valid number) */
	                        break ;

	                    case cparam_to:
	                        if (cfdecti(ebuf,el,&v) >= 0) {
	                            if (v >= 0) {
	                                sip->to = v ;
	                            }
	                        } /* end if (valid number) */
	                        break ;

	                    case cparam_logfile:
	                        if (! sip->final.lfname) {
	                            cchar *lfn = sip->lfname ;
	                            cchar	*tfn = tbuf ;
	                            sip->final.lfname = true ;
	                            sip->have.lfname = true ;
	                            ml = setfname(sip,tbuf,ebuf,el,true,
	                                LOGCNAME,sn,"") ;
	                            if ((lfn == nullptr) || 
	                                (strcmp(lfn,tfn) != 0)) {
	                                cchar	**vpp = &sip->lfname ;
	                                sip->changed.lfname = true ;
	                                rs = subinfo_setentry(sip,vpp,tbuf,ml) ;
	                            }
	                        }
	                        break ;

	                    case cparam_spooldir:
	                        if (sip->spooldname == nullptr) {
	                            rs = subinfo_spooldir(sip,ebuf,el) ;
	                        }
	                        break ;

	                    case cparam_hostname:
	                        if (sip->hostname == nullptr) {
	                            rs = subinfo_hostname(sip,ebuf,el) ;
	                        }
	                        break ;

	                    case cparam_source:
	                        rs = subinfo_source(sip,ebuf,el) ;
	                        break ;

	                    } /* end switch */

	                } /* end if (got one) */

	            } /* end while (fetching) */

	            paramfile_curend(pfp,&cur) ;
	        } /* end if (cursor) */

	        if (rs < 0) break ;
	    } /* end for (parameters) */
	} /* end if (active) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (config_reader) */

static int getdefmjd(time_t dt) noex {
	int		rs ;
	if (dt == 0) dt = getustime ;
	if (TMTIME ct ; (rs = tmtime_localtime(&ct,dt)) >= 0) {
	    int	y = (ct.year + TM_YEAR_BASE) ;
	    int	m = ct.mon ;
	    int	d = ct.mday ;
	    rs = getmjd(y,m,d) ;
	}
	return rs ;
}
/* end subroutine (getdefmjd) */

static int mkqfname(char *rbuf,cchar *qdname,int mjd) noex {
	int		rlen = MAXPATHLEN ;
	int		rs = SR_OK ;
	int		i = 0 ;
	if (rs >= 0) {
	    rs = storebuf_strw(rbuf,rlen,i,qdname,-1) ;
	    i += rs ;
	}
	if ((rs >= 0) && (i > 0) && (rbuf[i-1] != '/')) {
	    rs = storebuf_chr(rbuf,rlen,i,'/') ;
	    i += rs ;
	}
	if (rs >= 0) {
	    rs = storebuf_chr(rbuf,rlen,i,'q') ;
	    i += rs ;
	}
	if (rs >= 0) {
	    rs = storebuf_deci(rbuf,rlen,i,mjd) ;
	    i += rs ;
	}
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (mkqfname) */

static int setfname(SI *sip,char *fname,cc *ebuf,int el,int f_def,
		cchar *dname,cchar *name,cchar *suf) noex {
	int		rs = SR_OK ;
	int		ml ;
	cchar		*np ;
	char		tmpname[MAXNAMELEN + 1] ;

	if ((f_def && (ebuf[0] == '\0')) ||
	    (strcmp(ebuf,"+") == 0)) {

	    np = name ;
	    if ((suf != nullptr) && (suf[0] != '\0')) {
	        np = tmpname ;
	        mkfnamesuf1(tmpname,name,suf) ;
	    }

	    if (np[0] != '/') {
	        if ((dname != nullptr) && (dname[0] != '\0')) {
	            rs = mkpath3(fname,sip->pr,dname,np) ;
	        } else
	            rs = mkpath2(fname,sip->pr,np) ;
	    } else
	        rs = mkpath1(fname,np) ;

	} else if (strcmp(ebuf,"-") == 0) {

	    fname[0] = '\0' ;

	} else if (ebuf[0] != '\0') {

	    np = ebuf ;
	    if (el >= 0) {
	        np = tmpname ;
	        ml = MIN(MAXPATHLEN,el) ;
	        strwcpy(tmpname,ebuf,ml) ;
	    }

	    if (ebuf[0] != '/') {
	        if (strchr(np,'/') != nullptr) {
	            rs = mkpath2(fname,sip->pr,np) ;
	        } else {
	            if ((dname != nullptr) && (dname[0] != '\0')) {
	                rs = mkpath3(fname,sip->pr,dname,np) ;
	            } else
	                rs = mkpath2(fname,sip->pr,np) ;
	        } /* end if */
	    } else
	        rs = mkpath1(fname,np) ;

	} /* end if */

	return rs ;
}
/* end subroutine (setfname) */

static int mkourname(char *rbuf,cchar *pr,cchar *inter,cchar *sp,int sl) noex {
	int		rs = SR_OK ;

	if (strnchr(sp,sl,'/') != nullptr) {
	    if (sp[0] != '/') {
	        rs = mkpath2w(rbuf,pr,sp,sl) ;
	    } else {
	        rs = mkpath1w(rbuf,sp,sl) ;
	    }
	} else {
	    rs = mkpath3w(rbuf,pr,inter,sp,sl) ;
	}

	return rs ;
}
/* end subroutine (mkourname) */

#if	CF_DEBUGS && CF_OPENDEF
/* ARGSUSED */
static int opendef(SI *sip) noex {
	int		rs ;
	int		pipes[2] ;
	int		fd = -1 ;
	(void) sip ;
	if ((rs = uc_piper(pipes,0,3)) >= 0) {
	    int		wfd = pipes[0] ;
	    int		sl ;
	    cchar	*sp = "hello world!\n" ;
	    fd = pipes[1] ;
	    sl = strlen(sp) ;
	    rs = uc_writen(wfd,sp,sl) ;
	    u_close(wfd) ;
	}
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (opendef) */
#endif /* CF_DEBUGS */

#if	CF_DEBUGS
static int debugmode(cchar *ids,cchar *s,cchar *fname) noex {
	USTAT		sb ;
	int		rs ;
	if ((rs = u_stat(fname,&sb)) >= 0) {
	    char	mstr[100+1] ;
	    snfilemode(mstr,100,sb.st_mode) ;
	    debugprintf("%s: %s %s\n",ids,s,mstr) ;
	} else {
	    debugprintf("%s: %s rs=%d\n",ids,s,rs) ;
	}
	return rs ;
}
/* end subroutine (debugmode) */
#endif /* CF_DEBUGS */

#if	CF_DEBUGS
static int debugfmode(cchar *id,cchar *s,int fd) noex {
	USTAT		sb ;
	int		rs ;
		char	mstr[100+1] ;
		u_fstat(fd,&sb) ;
		snfilemode(mstr,100,sb.st_mode) ;
	rs = debugprintf("%s: %s m=%s\n",id,s,mstr) ;
	return rs ;
}
/* end subroutine (debugfmode) */
#endif /* CF_DEBUGS */


