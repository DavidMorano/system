/* pcspoll_progprocess SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* handle some service processing */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable print-outs */
#define	CF_DEBUG	0		/* switchable print-outs */
#define	CF_LOGID	1		/* use a special LOGID */
#define	CF_SETRUID	1		/* use 'setreuid(2)' */
#define	CF_SETEUID	0		/* already done in 'main()' */

/* revision history:

	= 2008-09-01, David A­D­ Morano
	This subroutine was borrowed and modified from previous
	generic front-end 'main' subroutines!

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Description:
	Prepare to do some servicing.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<baops.h>
#include	<vecstr.h>
#include	<bfile.h>
#include	<logfile.h>
#include	<varsub.h>
#include	<storebuf.h>
#include	<ids.h>
#include	<getax.h>
#include	<svcfile.h>
#include	<acctab.h>
#include	<strx.h>
#include	<vstrcmp.h>		/* |vstrkeycmp(3uc)| */
#include	<exitcodes.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"


/* local defines */

#ifndef	GETFNAME_TYPELOCAL
#define	GETFNAME_TYPELOCAL	0	/* search locally first */
#define	GETFNAME_TYPEROOT	1	/* search programroot area first */
#endif

#ifndef	VARPATHEXEC
#define	VARPATHEXEC	"PATH"
#endif

#ifndef	VARPATHLIB
#define	VARPATHLIB	"LD_LIBRARY_PATH"
#endif


/* external subroutines */

extern int	progsvcopen(PROGINFO *) ;
extern int	progsvcclose(PROGINFO *) ;
extern int	progaccopen(PROGINFO *) ;
extern int	progaccclose(PROGINFO *) ;

extern int	proglog_begin(PROGINFO *,USERINFO *) ;
extern int	proglog_end(PROGINFO *) ;
extern int	proglog_printf(PROGINFO *,cchar *,...) ;
extern int	proglog_flush(PROGINFO *) ;

extern int	progwatch(PROGINFO *,vecstr *) ;

extern int	securefile(cchar *,uid_t,gid_t) ;
extern int	mklogidpre(char *,int,cchar *,int) ;

#if	CF_DEBUGS || CF_DEBUG 
extern int	debugprintf(cchar *,...) ;
extern int	strlinelen(cchar *,int,int) ;
extern int	progexports(PROGINFO *,cchar *) ;
#endif /* CF_DEBUGS */


/* external variables */


/* local structures */


/* forward references */

local int	proglog_extra(PROGINFO *) ;
local int	procsecurity(PROGINFO *) ;
local int	procaa(PROGINFO *,ARGINFO *) ;

local int	loadserial(PROGINFO *) ;
local int	loadpath(PROGINFO *,vecstr *,cchar *,cchar **,cchar *) ;
local int	loadpathpr(PROGINFO *,vecstr *,cchar **) ;
local int	loadpathprdir(PROGINFO *,vecstr *,cchar *) ;
local int	loadpathcomp(PROGINFO *,vecstr *,cchar *) ;
local int	loadpather(PROGINFO *,vecstr *,cchar *,int) ;


/* local variables */

constexpr cpcchar	prbins[] = {
	"bin",
	"sbin",
	NULL
} ;

constexpr cpcchar	prlibs[] = {
	"lib",
	NULL
} ;


/* exported variables */


/* exported subroutines */

int progprocess(PROGINFO *pip,ARGINFO *aip,USERINFO *uip) noex {
	int		rs ;
	int		rs1 ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("progprocess: ent\n") ;
#endif

	if ((rs = loadserial(pip)) >= 0) {
	    if ((rs = proglog_begin(pip,uip)) >= 0) {
		if ((rs = proglog_extra(pip)) >= 0) {
	                if ((rs = progsvcopen(pip)) >= 0) {
	                    if ((rs = procsecurity(pip)) >= 0) {
	                        if ((rs = progaccopen(pip)) >= 0) {
				    {
	                                rs = procaa(pip,aip) ;
#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("progprocess: procaa() rs=%d\n",rs) ;
#endif
				    }
	                            rs1 = progaccclose(pip) ;
	                            if (rs >= 0) rs = rs1 ;
	                        } /* end if (progacc) */
	                    } /* end if (procsecutiry) */
	                    rs1 = progsvcclose(pip) ;
	                    if (rs >= 0) rs = rs1 ;
	                } else {
	                    if (pip->debuglevel > 0) {
				cchar	*pn = pip->progname ;
				cchar	*fmt ;
				fmt = "%s: no service table\n" ;
	        		bprintf(pip->efp,fmt,pn) ;
			    }
		        } /* end if (progsvc) */
		} /* end if (proglog_extra) */
		rs1 = proglog_end(pip) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (proglog) */
	} /* end if (loadserial) */

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("progprocess: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (progprocess) */


/* local subroutines */


local int proglog_extra(PROGINFO *pip)
{
	const int	f = (pip->fl.named || pip->fl.passfd) ;
	int		rs = SR_OK ;
	int		rs1 = SR_OK ;
	    if ((pip->cfname != NULL) && (rs1 >= 0)) {
	        rs1 = proglog_printf(pip,"cf=%s\n",pip->cfname) ;
	    }
	    if ((! f) && (pip->pidfname != NULL) && (rs1 >= 0)) {
	        rs1 = proglog_printf(pip,"pid=%s\n",pip->pidfname) ;
	    }
	return rs ;
}
/* end subroutine (proglog_extra) */


local int procsecurity(PROGINFO *pip)
{
	int		rs = SR_OK ;
	int		f = TRUE ;

	f = f && pip->fl.secure_root ;
	f = f && pip->fl.secure_conf ;
	f = f && pip->fl.secure_svcfile ;
	f = f && pip->fl.secure_path ;
	pip->fl.secure = f ;

#if	CF_SETRUID
	if (pip->fl.secure) {
	    if (pip->uid != pip->euid) {
	        u_setreuid(pip->euid,-1) ;
		if (pip->debuglevel > 0)
	        bprintf(pip->efp,"%s: set RUID to uid=%d\n",
	            pip->progname,pip->euid) ;
	    }
	    if (pip->gid != pip->egid)
	        u_setreuid(pip->egid,-1) ;
	}
#endif /* CF_SETRUID */

#if	CF_SETEUID
	if (! pip->fl.secure) {
	    if (pip->uid != pip->euid)
	        u_seteuid(pip->uid) ;
	    if (pip->gid != pip->egid)
	        u_setegid(pip->gid) ;
	}
#endif /* CF_SETEUID */

	return rs ;
}
/* end subroutine (procsecurity) */


local int procaa(PROGINFO *pip,ARGINFO *aip)
{
	vecstr		snames ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		ai ;
	int		opts ;
	int		count = 0 ;
	int		f ;
	cchar	*varpathexec = VARPATHEXEC ;
	cchar	*varpathlib = VARPATHLIB ;
	cchar	*defpath ;
	cchar	*cp ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("progprocess/procaa: ent\n") ;
#endif

	opts = VECSTR_OCOMPACT ;
	rs = vecstr_start(&pip->pathexec,40,opts) ;
	if (rs < 0)
	    goto ret0 ;

	defpath = pip->defpath ;
	rs = loadpath(pip,&pip->pathexec,varpathexec,prbins,defpath) ;
	if (rs < 0)
	    goto ret1 ;

	rs = vecstr_start(&pip->pathlib,40,opts) ;
	if (rs < 0)
	    goto ret1 ;

	defpath = "/usr/preroot/lib:/usr/extra/lib" ;
	rs = loadpath(pip,&pip->pathlib,varpathlib,prlibs,defpath) ;
	if (rs < 0)
	    goto ret2 ;

	rs = vecstr_start(&snames,0,0) ;
	if (rs < 0)
	    goto ret2 ;

/* load up all of the service names that we have so far */

	if (pip->fl.named) {

	    for (ai = 1 ; ai < aip->argc ; ai += 1) {

	        f = (ai <= aip->ai_max) && (bits_test(&aip->pargs,ai) > 0) ;
	        f = f || ((ai > aip->ai_pos) && (aip->argv[ai] != NULL)) ;
	        if (! f) continue ;

	        cp = aip->argv[ai] ;

#if	CF_DEBUG
	        if (DEBUGLEVEL(4))
	            debugprintf("progprocess/procaa: svc=%s\n",cp) ;
#endif

	        rs1 = svcfile_fetch(&pip->stab,cp,NULL,NULL,NULL,0) ;

#if	CF_DEBUG
	        if (DEBUGLEVEL(4))
	            debugprintf("progprocess/procaa: svcfile_fetch() rs=%d\n",
	                rs1) ;
#endif

	        if (rs1 >= 0) {

	            rs = vecstr_adduniq(&snames,cp,-1) ;
	            if (rs != INT_MAX) count += 1 ;

	        } else if (! pip->fl.quiet) {

	            rs = SR_NOTFOUND ;
	            bprintf(pip->efp,
	                "%s: unavailable service=%s\n",
	                pip->progname,cp) ;

	        } /* end if */

#if	CF_DEBUG
	        if (DEBUGLEVEL(4))
	            debugprintf("progprocess/procaa: had_svc rs=%d\n",rs1) ;
#endif

	        if (rs < 0) break ;
	    } /* end for */

#if	CF_DEBUG
	    if (DEBUGLEVEL(4))
	        debugprintf("progprocess/procaa: specified svcs=%u\n",
	            count) ;
#endif

	} /* end if (named services) */

/* do some things if we are running in daemon mode */

	if ((rs >= 0) && (pip->fl.daemon || pip->fl.background)) {

	    proglog_flush(pip) ;

	    bflush(pip->efp) ;

	    if (pip->debuglevel == 0) {

	        rs = uc_fork() ;
	 	if (rs == 0) {

		    if (pip->fl.caf) {
		        int	i ;
			bclose(pip->efp) ;
			pip->efp = NULL ;
			for (i = 0 ; i < 3 ; i += 1)
			    u_close(i) ;
		    }

	            u_setsid() ;

	            pip->pid = getpid() ;

		    pip->spid = pip->pid ;
	            proglog_printf(pip,"backgrounded pid=%d\n",
				pip->pid) ;
	
		} else if (rs > 0)
		    uc_exit(EX_OK) ;

	    } /* end if (backgrounding) */

	} /* end if (daemon mode) */

	if ((rs >= 0) && ((! pip->fl.named) || (count > 0))) {

	    rs = progwatch(pip,&snames) ;

#if	CF_DEBUG
	    if (DEBUGLEVEL(4))
	        debugprintf("progprocess/procaa: progwatch() rs=%d\n",rs) ;
#endif

	    if (rs < 0) {
	        if (pip->debuglevel > 0) {
	            bprintf(pip->efp,
	                "%s: exceptional (%d)\n",
	                pip->progname,rs) ;
		}
	        proglog_printf(pip,"exceptional (%d)\n",rs) ;
	    } /* end if */

	} /* end if */

	vecstr_finish(&snames) ;

ret2:
	vecstr_finish(&pip->pathlib) ;

ret1:
	vecstr_finish(&pip->pathexec) ;

ret0:
	return (rs >= 0) ? count : rs ;
}
/* end subroutine (procaa) */


local int loadserial(pip)
PROGINFO	*pip ;
{
	int		rs = SR_OK ;
	int		rs1 ;
	int		serial = -1 ;
	cchar	*sfn = SERIALFNAME ;
	char		tmpfname[MAXPATHLEN + 1] ;

	if (sfn[0] != '/') {

	    if (serial < 0) {
	        rs1 = mkpath3(tmpfname,pip->pr,VARDNAME,sfn) ;
	        if (rs1 > 0)
	            serial = getserial(tmpfname) ;
	    }

	    if (serial < 0) {
	        rs1 = mkpath3(tmpfname,pip->tmpdname,pip->rootname,sfn) ;
	        if (rs1 > 0)
	            serial = getserial(tmpfname) ;
	    } /* end if */

	    if (serial < 0) {
	        rs1 = mkpath2(tmpfname,pip->tmpdname,sfn) ;
	        if (rs1 > 0)
	            serial = getserial(tmpfname) ;
	    } /* end if */

	} else
	    serial = getserial(sfn) ;

	if (serial >= 0) {
	    pip->serial = (serial % (PID_MAX+1)) ;
	} else
	    pip->serial = pip->pid ;

#if	CF_LOGID
	{
	    const int	llen = LOGIDLEN ;
	    cchar	*nn = pip->nodename ;
	    int		serial = pip->serial ;
	    char	lbuf[LOGIDLEN + 1] ;
	    if ((rs = mklogidpre(lbuf,llen,nn,serial)) >= 0) {
		cchar	**vpp = &pip->logid ;
	        rs = proginfo_setentry(pip,vpp,lbuf,rs) ;
	    }
	}
#endif /* CF_LOGID */

	return rs ;
}
/* end subroutine (loadserial) */


local int loadpath(pip,plp,varname,prdirs,defpath)
PROGINFO	*pip ;
vecstr		*plp ;
cchar	*varname ;
cchar	**prdirs ;
cchar	*defpath ;
{
	VECSTR		*elp = &pip->exports ;
	int		rs ;
	int		c = 0 ;
	cchar	*pp ;

/* system-administrative environment */

#if	CF_DEBUG
	if (DEBUGLEVEL(4)) {
	    debugprintf("progprocess/loadpath: exports> \n") ;
	    progexports(pip,"loadpath0") ;
	}
#endif

	if ((rs = vecstr_search(elp,varname,vstrkeycmp,&pp)) >= 0) {
	    cchar	*tp ;

	    if ((tp = strchr(pp,'=')) != NULL) {
	        rs = loadpathcomp(pip,plp,(tp + 1)) ;
	        c += rs ;
	    }

/* our program root */

	    if (rs >= 0) {
	        rs = loadpathpr(pip,plp,prdirs) ;
	        c += rs ;
	    }

/* system-default path */

	    if ((rs >= 0) && (defpath != NULL)) {
	        rs = loadpathcomp(pip,plp,defpath) ;
	        c += rs ;
	    }

/* process environment */

	    if ((rs >= 0) && ((tp = getenv(varname)) != NULL)) {
	        rs = loadpathcomp(pip,plp,tp) ;
	        c += rs ;
	    }

	} /* end if (search-found) */

#if	CF_DEBUG
	if (DEBUGLEVEL(4)) {
	    debugprintf("progprocess/loadpath: ret exports> \n") ;
	    progexports(pip,"loadpath1") ;
	}
#endif

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("progprocess/loadpath: ret rs=%d c=%u\n",rs,c) ;
#endif

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (loadpath) */


local int loadpathpr(pip,plp,prdirs)
PROGINFO	*pip ;
vecstr		*plp ;
cchar	**prdirs ;
{
	int		rs = SR_OK ;
	int		i ;
	int		c = 0 ;

	for (i = 0 ; prdirs[i] != NULL ; i += 1) {
	    rs = loadpathprdir(pip,plp,prdirs[i]) ;
	    c += rs ;
	    if (rs < 0) break ;
	} /* end for */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (loadpathpr) */


local int loadpathprdir(pip,plp,bname)
PROGINFO	*pip ;
vecstr		*plp ;
cchar	bname[] ;
{
	int		rs ;
	int		c = 0 ;
	char		tbuf[MAXPATHLEN + 1] ;

	if ((rs = mkpath2(tbuf,pip->pr,bname)) >=0) {
	    rs = vecstr_adduniq(plp,tbuf,rs) ;
	    if (rs < INT_MAX) c += 1 ;
	}

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (loadpathprdir) */


local int loadpathcomp(pip,plp,pp)
PROGINFO	*pip ;
vecstr		*plp ;
cchar	*pp ;
{
	int		rs = SR_OK ;
	int		c = 0 ;
	cchar	*tp ;

	while ((tp = strbrk(pp,":;")) != NULL) {
	    rs = loadpather(pip,plp,pp,(tp - pp)) ;
	    pp = (tp + 1) ;
	    if (rs < 0) break ;
	} /* end while */

	if ((rs >= 0) && (pp[0] != '\0')) {
	    rs = loadpather(pip,plp,pp,-1) ;
	} /* end if (trailing one) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (loadpathcomp) */


local int loadpather(pip,plp,pbuf,plen)
PROGINFO	*pip ;
vecstr		*plp ;
cchar	pbuf[] ;
int		plen ;
{
	int		rs ;
	int		c = 0 ;
	char		tbuf[MAXPATHLEN + 1] ;

	if (pip == NULL) return SR_FAULT ;

	if ((rs = pathclean(tbuf,pbuf,plen)) > 0) {
	    int	pl = rs ;
	    if ((rs = vecstr_findn(plp,tbuf,pl)) == SR_NOTFOUND) {
	        c += 1 ;
	        rs = vecstr_add(plp,tbuf,pl) ;
	    }
	} /* end if (pathclean) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (loadpather) */


