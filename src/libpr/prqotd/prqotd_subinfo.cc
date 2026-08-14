/* prqotd_subinfo SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* open a channel (file-descriptor) to the quote-of-the-day (QOTD) */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* compile-time debugging */
#define	CF_OPENDEF	0		/* debugging support? */

/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	subinfo

	Description:
	This object supports (is really the guts of) the |prqotd_maint|
	subroutine.

	Synopsis:

	Arguments:

	Returns:

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<fcntl.h>		/* |O_{x}| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strchr(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<ucopen.h>
#include	<ucdesc.h>
#include	<ucfileop.h>		/* |uc_minmod(3uc)| */
#include	<getx.h>		/* |getprogpath(3uc)| */
#include	<getxname.h>		/* |getusername(3uc)| */
#include	<estrings.h>
#include	<ids.h>
#include	<permx.h>
#include	<sigman.h>
#include	<tmtime.hh>
#include	<storebuf.h>
#include	<vecstr.h>
#include	<vecpstr.h>
#include	<ascii.h>
#include	<paramfile.h>
#include	<expcook.h>
#include	<logfile.h>
#include	<mkdirs.h>
#include	<mkfnamesuf.h>
#include	<mklogid.h>
#include	<ctdecp.h>
#include	<timestr.h>
#include	<matxstr.h>		/* |matostr(3uc)| */
#include	<isnot.h>
#include	<localmisc.h>		/* |DIGBUFLEN| + |TIEBUFLEN| */
#include	<libdebug.h>		/* LIBDEBUG */

#include	"prqotd.h"
#include	"prqotd_util.hh"
#include	"prqotd_config.hh"
#include	"prqotd_subinfo.hh"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/uconstants.ccm"

import libutil ;			/* |lenstr(3u)| */
import uconstants ;			/* |varname(3u)| */

/* local defines */

#ifndef	LOGCNAME
#define	LOGCNAME	"log"
#endif

#define	CONFIGFNAME	"conf"
#define	QCNAME		"qotd"

#define	SUB		subinfo
#define	CF		config

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* compile-time debugging */
#endif


/* imported namespaces */

using prqotd::subinfo ;			/* type */
using prqotd::config ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using prqotd::config_start ;		/* subroutine */
using prqotd::config_finish ;		/* subroutine */
using prqotd::config_read ;		/* subroutine */
using prqotd::config_check ;		/* subroutine */
using prqotd::getdefmjd ;		/* subroutine */
using prqotd::mkqfname ;		/* subroutine */
using prqotd::mkourname ;		/* subroutine */
using libuc::libmem ;			/* variable */
using prqotd::var ;			/* type */


/* local typedefs */

typedef config *	configp ;
typedef logfile *	logfilep ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int subinfo_envbegin	(SUB *) noex ;
local int subinfo_envend	(SUB *) noex ;
local int subinfo_confbegin	(SUB *) noex ;
local int subinfo_confend	(SUB *) noex ;
local int subinfo_logfile	(SUB *,cchar *,int) noex ;
local int subinfo_logenv	(SUB *) noex ;
local int subinfo_opensrc	(SUB *,cchar *,cchar *) noex ;
local int subinfo_opensrcprog	(SUB *,cchar *,cchar *) noex ;
local int subinfo_defprog	(SUB *,cchar *) noex ;
local int subinfo_defproger	(SUB *,vecstr *,cc *,cc *) noex ;
local int subinfo_addourpath	(SUB *,vecstr *) noex ;
local int subinfo_addprbins	(SUB *,vecstr *) noex ;
local int subinfo_addprbin	(SUB *,vecstr *,cc *,cc *) noex ;
local int subinfo_id		(SUB *) noex ;
local int subinfo_dircheck	(SUB *,cchar *) noex ;
local int subinfo_dirminmode	(SUB *,cchar *,mode_t) noex ;

#if	CF_DEBUG && CF_OPENDEF
local int opendef(SUB *) noex ;
#endif

#if	CF_DEBUG
local int debugmode(cchar *,cchar *,cchar *) noex ;
#endif

#if	CF_DEBUG
local int debugfmode(cchar *,cchar *,int) noex ;
#endif


/* local variables */

constexpr cpcchar	csched[] = {
	"%p/%e/%n/%n.%f",
	"%p/%e/%n/%f",
	"%p/%e/%n.%f",
	"%p/%n.%f",
	nullptr
} ; /* end array (csched) */

enum sources {
	source_prog,
	source_systems,
	source_uqotd,
	source_overlast
} ; /* end enum (sources) */

constexpr cpcchar	sources[] = {
	"prog",
	"systems",
	"uqotd",
	nullptr
} ; /* end array */

constexpr cpcchar	prbins[] = {
	"bin",
	"sbin",
	nullptr
} ; /* end array */

constexpr cpcchar	defprogs[] = {
	"mkqotd",
	"fortune",
	"/swd/local/bin/fortune",
	"/usr/extra/bin/fortune",
	"/usr/games/fortune",
	nullptr
} ; /* end array */

cbool			f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

namespace prqotd {
    int subinfo_start(SUB *sip,time_t dt,cc *pr,int of,int to,int mjd) noex {
	int		rs ;
	if (dt == 0) dt = getustime ;
	memclear(sip) ;
	sip->pr = pr ;
	sip->of = of ;
	sip->om = 0666 ;
	sip->to = to ;
	sip->mjd = mjd ;
	sip->sn = PRQOTD_SN ;
	sip->dt = dt ;
	sip->euid = geteuid() ;
	sip->fl.logsub = true ;
	if (ustat sb ; (rs = u_stat(pr,&sb)) >= 0) ylikely {
	    sip->uid_pr = sb.st_uid ;
	    sip->gid_pr = sb.st_gid ;
	    if ((rs = subinfo_envbegin(sip)) >= 0) ylikely {
	        if ((rs = subinfo_confbegin(sip)) >= 0) ylikely {
		    cint	llen = LOGIDLEN ;
		    cint	v = var.pid ;
		    cchar	*nn = sip->nn ;
		    char	lbuf[LOGIDLEN+1] ;
		    if ((rs = mklogid(lbuf,llen,nn,5,v)) >= 0) ylikely {
			cchar	**vpp = &sip->logid ;
			rs = subinfo_setentry(sip,vpp,lbuf,rs) ;
		    }
		    if (rs < 0) nlikely {
	        	subinfo_confend(sip) ;
		    } /* end if (error) */
		} /* end if (subinfo_confbegin) */
	        if (rs < 0) nlikely {
	            subinfo_envend(sip) ;
		} /* end if (error) */
	    } /* end if (subinfo_envbegin) */
	    if (rs < 0) nlikely {
	        if (sip->open.stores) {
	            sip->open.stores = false ;
	            vecstr_finish(&sip->stores) ;
	        }
	    } /* end if (error)*/
	} /* end if (stat-pr) */
	return rs ;
    } /* end subroutine (subinfo_start) */
    int subinfo_finish(SUB *sip) noex {
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
    } /* end subroutine (subinfo_finish) */
    int subinfo_defaults(SUB *sip) noex {
	int		rs = SR_OK ;
	if (sip->spooldname == nullptr) {
	    cchar	*vp = sip->sn ;
	    cint	vl = -1 ;
	    rs = subinfo_spooldir(sip,vp,vl) ;
	} /* end if */
	if (rs >= 0) ylikely {
	    cchar	*lf = sip->lfname ;
	    if (((lf == nullptr) || (lf[0] == '+')) && sip->fl.logsub) {
	        cchar	*vp = sip->sn ;
	        cint	vl = -1 ;
	        rs = subinfo_logfile(sip,vp,vl) ;
	    } /* end if */
	} /* end if (ok) */
	return rs ;
    } /* end subroutine (subinfo_defaults) */
    int subinfo_logbegin(SUB *sip) noex {
	int		rs = SR_OK ;
	cchar		*lf = sip->lfname ;
	DEBUGPRINTF("ent\n") ;
	if ((lf != nullptr) && (lf[0] != '-')) {
	    cint	sz = szof(logfile) ;
	    if (void *p ; (rs = lm_mall(sz,&p)) >= 0) ylikely {
		logfile	*lfp = (logfile *) p ;
	        cchar	*logid = sip->logid ;
	        sip->lfp = (logfile *) p ;
		DEBUGPRINTF("lf=%s\n",lf) ;
	        if ((rs = logfile_open(lfp,lf,0,0666,logid)) >= 0) {
		    sip->open.logsub = true ;
		    rs = subinfo_logenv(sip) ;
		    if (rs < 0) nlikely {
			sip->open.logsub = false ;
			logfile_close(lfp) ;
		    } /* end if (error) */
		} else if (isNotPresent(rs)) {
		    rs = SR_OK ;
		}
		DEBUGPRINTF("out rs=%d\n",rs) ;
		if (rs < 0) nlikely {
		    lm_free(sip->lfp) ;
		    sip->lfp = nullptr ;
		} /* end if (error) */
	    } /* end if (memory-allocation) */
	} /* end if (log-file) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
    } /* end subroutine (subinfo_logbegin) */
    int subinfo_logend(SUB *sip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (sip->lfp) {
	    logfile	*lfp = (logfile *) sip->lfp ;
	    {
	        rs1 = logfile_close(lfp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = lm_free(sip->lfp) ;
	        if (rs >= 0) rs = rs1 ;
	        sip->lfp = nullptr ;
	    }
	} /* end if */
	return rs ;
    } /* end subroutine (subinfo_logend) */
    int subinfo_spoolcheck(SUB *sip) noex {
	int		rs ;
	cchar		*sdname = sip->spooldname ;
	{
	    rs = subinfo_dircheck(sip,sdname) ;
	}
	return rs ;
    } /* end subroutine (subinfo_spoolcheck) */
    int subinfo_qdirname(SUB *sip,int mjd) noex {
	cint		dlen = DIGBUFLEN ;
	cint		prec = 3 ; /* digit precision for another 100 years */
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	cchar		*sdname = sip->spooldname ;
	char		dbuf[DIGBUFLEN+1] ;
	DEBUGPRINTF("ent mjd=%d\n",mjd) ;
	if ((rs = ctdecpi((dbuf+1),(dlen-1),prec,(mjd/100))) > 0) ylikely {
	    if (char *tbuf ; (rs = lm_mp(&tbuf)) >= 0) ylikely {
	        dbuf[0] = 'd' ;
	        if ((rs = mkpath2w(tbuf,sdname,dbuf,(rs+1))) >= 0) ylikely {
	            cchar	**vpp = &sip->qdname ;
		    len = rs ;
		    if ((rs = subinfo_setentry(sip,vpp,tbuf,len)) >= 0) {
		        rs = subinfo_dircheck(sip,tbuf) ;
		    } /* end if (subinfo_setentry) */
	        } /* end if (mkpath) */
	        rs1 = lm_free(tbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (ctdeci) */
	DEBUGPRINTF("ret rs=%d len=%d\n",rs,len) ;
	return (rs >= 0) ? len : rs ;
    } /* end subroutine (subinfo_qdirname) */
    int subinfo_gather(SUB *sip,cchar *qfname,mode_t om) noex {
	int		rs = SR_OK ;
	int		fd = -1 ;
	DEBUGPRINTF("ent qfn=%s\n",qfname) ;
	if (sip->open.sources) {
	    vecpstr	*slp = &sip->sources ;
	    cchar	*sep ;
	    DEBUGPRINTF("cnt n=%d\n",vecpstr_count(slp)) ;
	    for (int i = 0 ; (rs = vecpstr_get(slp,i,&sep)) >= 0 ; i += 1) {
	        if (sep != nullptr) {
	            rs = subinfo_opensrc(sip,qfname,sep) ;
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
	} /* end if */
	DEBUGPRINTF("mid rs=%d fd=%u\n",rs,fd) ;
	if (rs >= 0) ylikely {
	    if ((rs = u_rewind(fd)) >= 0) ylikely {
		if ((rs = uc_fminmod(fd,om)) >= 0) ylikely {
		    uid_t	u = sip->uid_pr ;
		    gid_t	g = sip->gid_pr ;
		    if ((rs = u_fchown(fd,u,g)) == SR_PERM) {
			rs = SR_OK ;
		    }
		} /* end if */
	    } /* end if (rewind) */
	    if (rs < 0) u_close(fd) ;
	} /* end if (got a source) */
	DEBUGPRINTF("ret rs=%d fd=%d\n",rs,fd) ;
	return (rs >= 0) ? fd : rs ;
    } /* end subroutine (subinfo_gather) */
    int subinfo_setfname(SUB *sip,char *fnamep,cc *ebuf,int el,int f_def,
		cchar *dname,cchar *name,cchar *suf) noex {
	cint		fnamel = var.maxpathlen ;
	int		rs ;
	int		rs1 ;
	if (char *tmpname ; (rs = lm_mn(&tmpname)) >= 0) ylikely {
	    cchar	*namp ; /* used-multiple */
	    if ((f_def && (ebuf[0] == '\0')) || (strcmp(ebuf,"+") == 0)) {
	        namp = name ;
	        if ((suf != nullptr) && (suf[0] != '\0')) {
	            namp = tmpname ;
	            rs = mkfnamesuf1(tmpname,name,suf) ;
	        } /* end if */
		if (rs >= 0) ylikely {
	            if (namp[0] != '/') {
	                if ((dname != nullptr) && (dname[0] != '\0')) {
	                    rs = mkpath3(fnamep,sip->pr,dname,namp) ;
	                } else {
	                    rs = mkpath2(fnamep,sip->pr,namp) ;
		        }
	            } else {
	                rs = mkpath1(fnamep,namp) ;
	            }
		} /* end if (ok) */
	    } else if (strcmp(ebuf,"-") == 0) {
	        fnamep[0] = '\0' ;
	    } else if (ebuf[0] != '\0') {
	        namp = ebuf ;
	        if (el >= 0) {
	            cint ml = min(fnamel,el) ;
	            namp = tmpname ;
	            strwcpy(tmpname,ebuf,ml) ;
	        } /* end if */
	        if (ebuf[0] != '/') {
	            if (strchr(namp,'/') != nullptr) {
	                rs = mkpath2(fnamep,sip->pr,namp) ;
	            } else {
	                if ((dname != nullptr) && (dname[0] != '\0')) {
	                    rs = mkpath3(fnamep,sip->pr,dname,namp) ;
	                } else {
	                    rs = mkpath2(fnamep,sip->pr,namp) ;
		        }
	            } /* end if */
	        } else {
	            rs = mkpath1(fnamep,namp) ;
	        }
	    } /* end if */
	    rs1 = lm_free(tmpname) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
    } /* end subroutine (subinfo_setfname) */
    int subinfo_setentry(SUB *lip,cchar **epp,cchar *vp,int vl) noex {
	int		rs = SR_FAULT ;
	int		vnlen = 0 ; /* return-value */
	if (lip && epp) ylikely {
	    rs = SR_OK ;
	    if (! lip->open.stores) {
	        rs = vecstr_start(&lip->stores,4,0) ;
	        lip->open.stores = (rs >= 0) ;
	    }
	    if (rs >= 0) ylikely {
	        int	oi = -1 ;
	        if (*epp != nullptr) {
	            oi = vecstr_findaddr(&lip->stores,*epp) ;
	        }
	        if (vp != nullptr) {
	            vnlen = lenstr(vp,vl) ;
	            rs = vecstr_store(&lip->stores,vp,vnlen,epp) ;
	        } else {
		    *epp = nullptr ;
	        }
	        if ((rs >= 0) && (oi >= 0)) {
	            vecstr_del(&lip->stores,oi) ;
	        }
	    } /* end if (ok) */
	} /* end if (non-null) */
	return (rs >= 0) ? vnlen : rs ;
    } /* end subroutine (subinfo_setentry) */
    int subinfo_spooldir(SUB *sip,cchar *vp,int vl) noex {
	int		rs ;
	int		rs1 ;
	cchar		*pr = sip->pr ;
	cchar		*inter = PRQOTD_VARSPOOL ;
	if (char *tbuf ; (rs = lm_mp(&tbuf)) >= 0) {
	    if ((rs = mkourname(tbuf,pr,inter,vp,vl)) >= 0) ylikely {
	        cchar	**vpp = &sip->spooldname ;
	        rs = subinfo_setentry(sip,vpp,tbuf,rs) ;
	    } /* end if (mkourname) */
	    rs1 = lm_free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
    } /* end subroutine (subinfo_spooldir) */
    int subinfo_hostname(SUB *sip,cchar *vp,int vl) noex {
	int		rs = SR_OK ;
	if (! sip->open.hosts)  {
	    rs = vecpstr_start(&sip->hosts,0,0,0) ;
	    sip->open.hosts = (rs >= 0) ;
	}
	if ((rs >= 0) && vp) {
	    rs = vecpstr_adduniq(&sip->hosts,vp,vl) ;
	}
	return rs ;
    } /* end subroutine (subinfo_hostname) */
    int subinfo_source(SUB *sip,cchar *vp,int vl) noex {
	int		rs = SR_OK ;
	if (! sip->open.sources)  {
	    rs = vecpstr_start(&sip->sources,0,0,0) ;
	    sip->open.sources = (rs >= 0) ;
	}
	if ((rs >= 0) && (vp != nullptr)) {
	    rs = vecpstr_adduniq(&sip->sources,vp,vl) ;
	}
	return rs ;
    } /* end subroutine (subinfo_source) */
} /* end namespace (prqotd) */


/* private subroutines */

local int subinfo_envbegin(SUB *sip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	DEBUGPRINTF("ent\n") ;
	{
	    if (char *ebuf ; (rs = lm_mp(&ebuf)) >= 0) ylikely {
	        cint	elen = rs ;
	        int	el = -1 ;
	        cchar	*en = ebuf ;
	        if ((rs = getprogexec(ebuf,elen)) == SR_NOSYS) {
	            rs = SR_OK ;
	            en = PRQOTD_PROGEXEC ;
	        } else {
	            el = rs ;
	        } /* end if */
	        if (rs >= 0) ylikely {
	            cchar	**vpp = &sip->pn ;
		    cchar	*cp ;
	            if (int cl ; (cl = sfbasename(en,el,&cp)) > 0) ylikely {
		        if (char *tp = strnchr(cp,cl,'.') ; tp) {
			    cl = intconv(tp - cp) ;
		        }
	                rs = subinfo_setentry(sip,vpp,cp,cl) ;
	            } /* end if (sfbasename) */
	        } /* end if (ok) */
		rs1 = lm_free(ebuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end block */
	if (rs >= 0) ylikely {
	    cint sz = ((var.nodenamelen + 1) + (var.hostnamelen + 1)) ;
	    int ai = 0 ;
	    if (char *a ; (rs = lm_mall(sz,&a)) >= 0) ylikely {
	        char	*nn = (a + (ai++ * (var.nodenamelen + 1))) ;
	        char	*dn = (a + (ai++ * (var.nodenamelen + 1))) ;
	        if ((rs = getnodedomain(nn,dn)) >= 0) {
	            cchar	**vpp = &sip->nn ;
	            if ((rs = subinfo_setentry(sip,vpp,nn,-1)) >= 0) {
	                vpp = &sip->dn ;
	                rs = subinfo_setentry(sip,vpp,dn,-1) ;
	            }
	        } /* end if (getnodename) */
	        rs1 = lm_free(a) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (ok) */
	if (rs >= 0) ylikely {
	    if (char *ubuf ; (rs = lm_un(&ubuf)) >= 0) ylikely {
		cint ulen = rs ;
	        if ((rs = getusername(ubuf,ulen,-1)) >= 0) ylikely {
	            cchar	**vpp = &sip->un ;
	            rs = subinfo_setentry(sip,vpp,ubuf,rs) ;
	        }
	        rs1 = lm_free(ubuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (ok) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (subinfo_envbegin) */

local int subinfo_envend(SUB *sip) noex {
	int		rs = SR_FAULT ;
	if (sip) {
	    rs = SR_OK ;
	}
	return rs ;
} /* end subroutine (subinfo_envend) */

local int subinfo_confbegin(SUB *sip) noex {
	cint		csz = szof(config)  ;
	int		rs = SR_OK ;
	cchar		*cfname = CONFIGFNAME ;
	DEBUGPRINTF("ent\n") ;
	if (void *p ; (rs = lm_mall(csz,&p)) >= 0) ylikely {
	    CF	*csp = configp(p) ;
	    sip->cfp = csp ;
	    if ((rs = config_start(csp,sip,cfname)) >= 0) ylikely {
	        if ((rs = config_read(csp)) >= 0) ylikely {
	            rs = 1 ;
	        }
	        if (rs < 0) nlikely {
	            config_finish(csp) ;
		} /* end if (error) */
	    } /* end if (config) */
	    if (rs < 0) nlikely {
	        lm_free(p) ;
	        sip->cfp = nullptr ;
	    } /* end if (error) */
	} /* end if (memory-allocation) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (subinfo_confbegin) */

local int subinfo_confend(SUB *sip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	DEBUGPRINTF("ent\n") ;
	if (sip->cfp) ylikely {
	    CF	*csp = configp(sip->cfp) ;
	    {
	        rs1 = config_finish(csp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = lm_free(sip->cfp) ;
	        if (rs >= 0) rs = rs1 ;
	        sip->cfp = nullptr ;
	    }
	} /* end if */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (subinfo_confend) */

local int subinfo_logfile(SUB *sip,cchar *vp,int vl) noex {
	int		rs ;
	int		rs1 ;
	cchar		*pr = sip->pr ;
	cchar		*inter = LOGCNAME ;
	if (char *tbuf ; (rs = lm_mp(&tbuf)) >= 0) ylikely {
	    if ((rs = mkourname(tbuf,pr,inter,vp,vl)) >= 0) ylikely {
	        cchar	**vpp = &sip->lfname ;
	        rs = subinfo_setentry(sip,vpp,tbuf,rs) ;
	    } /* end if (mkourname) */
	    rs1 = lm_free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
} /* end subroutine (subinfo_logfile) */

local int subinfo_logenv(SUB *sip) noex {
	int		rs = SR_OK ;
	char		tbuf[TIMEBUFLEN+1] ;
	if (sip->open.logsub) {
	    logfile	*lfp = logfilep(sip->lfp) ;
	    timestr_logz(sip->dt,tbuf) ;
	    logfile_printf(lfp,"%s %s %s!%s",tbuf,sip->dn,sip->nn,sip->un) ;
#ifdef	COMMENT
	    logfile_printf(lfp,"pr=%s",sip->pr) ;
	    logfile_printf(lfp,"pn=%s",sip->pn) ;
#endif /* COMMENT */
	    logfile_printf(lfp,"mjd=%d",sip->mjd) ;
	}
	return rs ;
} /* end subroutine (subinfo_logenv) */

local int subinfo_opensrc(SUB *sip,cchar *qf,cchar *sep) noex {
	int		rs = SR_OK ;
	int		fd = -1 ; /* return-value */
	int		sl = -1 ;
	cchar		*sp = sep ;
	cchar		*ap ;
	DEBUGPRINTF("ent qf=%s sep=%s\n",qf,sep) ;
	if (cchar *tp ; (tp = strchr(sp,CH_FS)) != nullptr) {
	    sl = intconv(tp - sp) ;
	    ap = (tp + 1) ;
	} else {
	    ap = (sp + lenstr(sp)) ; /* <- set to NUL char */
	}
#if	CF_DEBUG
	{
	    strnum ps(sp,sl) ;
	    DEBUGPRINTF("svc=%s\n",ccp(ps)) ;
	}
#endif /* CF_DEBUG */
	DEBUGPRINTF("ap=%s\n",ap) ;
	if (int si ; (si = matostr(sources,3,sp,sl)) >= 0) {
	    switch (si) {
	    case source_prog:
	        rs = subinfo_opensrcprog(sip,qf,ap) ;
	        break ;
	    default:
	        rs = SR_NOENT ;
	        break ;
	    } /* end switch */
	    fd = rs ;
	} else {
	    rs = SR_NOENT ;
	}
#if	CF_DEBUG && CF_OPENDEF
	if (rs == SR_NOENT) {
	    rs = opendef(sip) ;
	    fd = rs ;
	} /* end if (opendef) */
#endif /* CF_DEBUG */
	DEBUGPRINTF("ret rs=%d fd=%d\n",rs,fd) ;
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (subinfo_opensrc) */

local int subinfo_opensrcprog(SUB *sip,cchar *qf,cchar *ap) noex {
	int		rs ;
	int		fd = -1 ;
	DEBUGPRINTF("ent a=%s\n",ap) ;
	{
	    rs = subinfo_prog(sip,qf,ap) ;
	    fd = rs ;
	}
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (subinfo_opensrcprog) */

local int subinfo_defprog(SUB *sip,cchar *qfn) noex {
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	if ((rs = subinfo_id(sip)) >= 0) ylikely {
	    vecstr	path, *plp = &path ;
	    if ((rs = vecstr_start(plp,5,0)) >= 0) ylikely {
	        if ((rs = subinfo_addourpath(sip,plp)) >= 0) ylikely {
	            for (int i = 0 ; defprogs[i] ; i += 1) {
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
	    } /* end if (vecstr) */
	    if ((rs < 0) && (fd >= 0)) {
		u_close(fd) ;
	    }
	} /* end if (subinfo_id) */
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (subinfo_defprog) */

/****
  The memory allocation below is a combination of a 
  MAXPATHLEN and a MAXNAMELEN, in that order.
****/
local int subinfo_defproger(SUB *sip,vecstr *plp,cc *prog,cc *qfn) noex {
	ids		*idp = &sip->id ;
	cint		sz = ((var.maxpathlen + 1) + (var.maxnamelen + 1)) ;
	cint		maxpath = var.maxpathlen ;
	int		rs ;
	int		rs1 ;
	int		fd = -1 ; /* return-value */
	int		ai = 0 ;
	if (char *a ; (rs = lm_mall(sz,&a)) >= 0) ylikely {
	    char	*rbuf = (a + (ai++ * (maxpath + 1))) ;
	    if ((rs = getprogpath(idp,plp,rbuf,prog,-1)) >= 0) ylikely {
	        cint	alen = maxpath ;
	        cint	rl = rs ;
	        char	*abuf = (a + (ai++ * (maxpath + 1))) ;
	        if (rl == 0) {
		    rs = mkpath1(rbuf,prog) ;
	        }
	        if (rs >= 0) ylikely {
		    cchar	*cp ;
	            if (int cl ; (cl = sfbasename(prog,-1,&cp)) > 0) ylikely {
		        if ((rs = sncpy1w(abuf,alen,cp,cl)) >= 0) ylikely {
			    cint	of = O_RDONLY ;
		            cchar	*av[2] ;
		            cchar	**ev = nullptr ;
	                    av[0] = abuf ;
		            av[1] = nullptr ;
	                    if ((rs = uc_openprog(rbuf,of,av,ev)) >= 0) {
			        cmode	om = 0664 ;
			        cint	qof = (O_CREAT|O_TRUNC|O_RDWR) ;
		                cint	pfd = rs ;
			        if ((rs = uc_open(qfn,qof,om)) >= 0) {
				    fd = rs ;
				    rs = uc_writedesc(fd,pfd,-1) ;
				    if (rs < 0) {
				        u_close(fd) ;
				        fd = -1 ;
				    } /* end if (error) */
			        } /* end if (uc_open) */
			        rs1 = u_close(pfd) ;
			        if (rs >= 0) rs = rs1 ;
		            } /* end if (uc_openprog) */
	                } /* end if (sncpy) */
	            } else {
		        rs = SR_NOENT ;
		    }
	        } /* end if (mkpath) */
	    } /* end if (getprogpath) */
	    rs1 = lm_free(a) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (subinfo_defproger) */

local int subinfo_addourpath(SUB *sip,vecstr *plp) noex {
	static cchar	*path = getenver(varname.path) ;
	int		rs = SR_OK ;
	int		c = 0 ;
	if (path) {
	    rs = plp->addpathclean(path,-1) ;
	    c += rs ;
	}
	if (rs >= 0) ylikely {
	    rs = subinfo_addprbins(sip,plp) ;
	    c += rs ;
	}
	return (rs >= 0) ? c : rs ;
} /* end subroutine (subinfo_addourpath) */

local int subinfo_addprbins(SUB *sip,vecstr *plp) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	cchar		*pr = sip->pr ;
	for (int i = 0 ; (rs >= 0) && prbins[i] ; i += 1) {
	    cchar	*prbin = prbins[i] ;
	    rs = subinfo_addprbin(sip,plp,pr,prbin) ;
	    c += rs ;
	} /* end for */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (subinfo_addprbins) */

local int subinfo_addprbin(SUB *sip,vecstr *plp,cc *pr,cc *prbin) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (char *tbuf ; (rs = lm_mp(&tbuf)) >= 0) ylikely {
	    if ((rs = mkpath2(tbuf,pr,prbin)) >= 0) ylikely {
	        cint	tl = rs ;
	        if (ustat sb ; (rs = u_stat(tbuf,&sb)) >= 0) ylikely {
		    if (S_ISDIR(sb.st_mode)) {
		        cint	am = (R_OK|X_OK) ;
		        if ((rs = permids(&sip->id,&sb,am)) >= 0) {
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
	    rs1 = lm_free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (subinfo_addprbin) */

local int subinfo_id(SUB *sip) noex {
	int		rs = SR_OK ;
	if (! sip->open.id) {
	    sip->open.id = true ;
	    rs = ids_load(&sip->id) ;
	}
	return rs ;
} /* end subroutine (subinfo_id) */

local int subinfo_dircheck(SUB *sip,cchar *dname) noex {
	const uid_t	euid = sip->euid ;
	cint		nrs = SR_NOENT ;
	int		rs ;
	cmode		dm = (0777 | S_ISGID) ;
	if (ustat sb ; (rs = u_stat(dname,&sb)) >= 0) {
	    if (sb.st_uid == euid) {
		rs = subinfo_dirminmode(sip,dname,dm) ;
	    }
	} else if (rs == nrs) {
	    if ((rs = mkdirs(dname,dm)) >= 0) {
		rs = subinfo_dirminmode(sip,dname,dm) ;
	    } /* end if (mkdirs) */
	} /* end if (stat) */
	return rs ;
} /* end subroutine (subinfo_dircheck) */

local int subinfo_dirminmode(SUB *sip,cchar *dname,mode_t dm) noex {
	const uid_t	euid = sip->euid ;
	int		rs ;
	if ((rs = uc_minmod(dname,dm)) >= 0) ylikely {
	    if (sip->uid_pr != euid) {
		u_chown(dname,sip->uid_pr,sip->gid_pr) ;
	    }
	} /* end if (uc_minmod) */
	return rs ;
} /* end subroutine (subinfo_dirminmode) */

#if	CF_DEBUG && CF_OPENDEF
/* ARGSUSED */
local int opendef(SUB *sip) noex {
	int		rs ;
	int		pipes[2] ;
	int		fd = -1 ;
	(void) sip ;
	if ((rs = uc_piper(pipes,0,3)) >= 0) ylikely {
	    int		wfd = pipes[0] ;
	    int		sl ;
	    cchar	*sp = "hello world!\n" ;
	    fd = pipes[1] ;
	    sl = lenstr(sp) ;
	    rs = uc_writen(wfd,sp,sl) ;
	    u_close(wfd) ;
	} /* end if */
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (opendef) */
#endif /* CF_DEBUG */

#if	CF_DEBUG
local int debugmode(cchar *ids,cchar *s,cchar *fname) noex {
	int		rs ;
	int		len = 0 ; /* return-value */
	if (char *mstr ; (rs = lm_ml(&mstr)) >= 0) ylikely {
	    cint	mlen = rs ;
	    if (ustat sb ; (rs = u_stat(fname,&sb)) >= 0) {
	        if ((rs = snfilemode(mstr,mlen,sb.st_mode)) >= 0) {
	            debugprintf("%s: %s %s\n",ids,s,mstr) ;
		}
	    } else {
	        debugprintf("%s: %s rs=%d\n",ids,s,rs) ;
	    }
	    rs1 = lm_free(mstr) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (debugmode) */
#endif /* CF_DEBUG */

#if	CF_DEBUG
local int debugfmode(cchar *id,cchar *s,int fd) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
	if (char *mstr ; (rs = lm_ml(&mstr)) >= 0) ylikely {
	    cint	mlen = rs ;
	    if (ustat sb ; (rs = u_fstat(fd,&sb)) >= 0) ylikely {
	        if ((rs = snfilemode(mstr,mlen,sb.st_mode)) >= 0) {
	            rs = debugprintf("%s: %s m=%s\n",id,s,mstr) ;
		    len = rs ;
		} /* end if (snfilemode) */
	    } /* end if (u_fstat) */
	    rs1 = lm_free(mstr) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (debugfmode) */
#endif /* CF_DEBUG */


