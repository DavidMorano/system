/* progconfig SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* program configuration */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable debug print-outs */
#define	CF_DEBUG	0		/* switchable at invocation */

/* revision history:

	= 2008-10-10, David A­D­ Morano
	This was adapted from the BACKGROUND program.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Description:
	This module contains the subroutines that manage program
	configuration.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<vecstr.h>
#include	<paramfile.h>
#include	<ascii.h>
#include	<prmkfname.h>		/* LIBPR */
#include	<localmisc.h>

#include	"vmail_config.h"
#include	"defs.h"


/* local defines */

#define	PI		proginfo

#define	PCONF		struct pconf
#define	PCONF_FL	struct pconf_flags

#ifndef	KBUFLEN
#define	KBUFLEN		(4 * MAXPATHLEN)
#endif

#ifndef	VBUFLEN
#define	VBUFLEN		(4 * MAXPATHLEN)
#endif

#ifndef	PBUFLEN
#define	PBUFLEN		(KBUFLEN + VBUFLEN + MAXPATHLEN)
#endif

#ifndef	EBUFLEN
#define	EBUFLEN		(4 * MAXPATHLEN)
#endif


/* external subroutines */

#if	CF_DEBUGS || CF_DEBUG
extern int	debugprintf(cchar *,...) noex ;
extern int	strlinelen(cchar *,int,int) noex ;
#endif


/* local structures */

struct pconf_flags {
	uint		opened:1 ;
} ;

struct pconf {
	proginfo	*pip ;
	cchar	**envv ;
	PCONF_FL	f ;
	paramfile	params ;
	int		nf ;		/* n-files */
} ;


/* forward references */

local int procfile(PI *,cchar **,cchar *) ;
local int procfile_begin(PI *) ;
local int procfile_end(PI *) ;
local int procfile_load(PI *,cchar *) ;
local int procauxprog(PI *,cchar *,int) ;

local int pconf_start(PCONF *,PI *,cchar **) ;
local int pconf_fileadd(PCONF *,cchar *) ;
local int pconf_check(PCONF *,time_t) ;
local int pconf_load(PCONF *) ;
local int pconf_loader(PCONF *,char *,int) ;
local int pconf_finish(PCONF *) ;


/* local variables */

static cchar	*sysconfs[] = {
	"%p/etc/%n/%n.%f",
	"%p/etc/%n/%f",
	"%p/etc/%n.%f",
	"%p/%n.%f",
	nullptr
} ;

static cchar	*usrconfs[] = {
	"%h/etc/%n/%n.%f",
	"%h/etc/%n/%f",
	"%h/etc/%n.%f",
	nullptr
} ;

static cchar	*params[] = {
	"cmdkey",
	"logfile",
	"loglen",
	"logsize",
	"mailcheck",
	"testmsg",
	"auxprog",
	"mbdefault",
	"mbinput",
	"mbspam",
	"mbtrash",
	nullptr
} ;

enum params {
	param_cmdkey,
	param_logfile,
	param_loglen,
	param_logsize,
	param_mailcheck,
	param_testmsg,
	param_auxprog,
	param_mbdefault,
	param_mbinput,
	param_mbspam,
	param_mbtrash,
	param_overlast
} ;

static cchar	*prognames[] = {
	"shell",
	"getmail",
	"mailer",
	"editor",
	"metamail",
	"pager",
	"postspam",
	nullptr
} ;

enum prognames {
	progname_shell,
	progname_getmail,
	progname_mailer,
	progname_editor,
	progname_metamail,
	progname_pager,
	progname_postspam,
	progname_overlast
} ;


/* exported subroutines */


int progconf_begin(PI *pip)
{
	int		rs = SR_OK ;

#if	CF_DEBUG 
	if (DEBUGLEVEL(3))
	    debugprintf("progconf_begin: ent\n") ;
#endif

/* look for system configuration file */

	if ((rs >= 0) && (! pip->fl.nosysconf)) {
	    cchar	*cfn = CONFIGFNAME ;
	    rs = procfile(pip,sysconfs,cfn) ;
	}

#if	CF_DEBUG 
	if (DEBUGLEVEL(3))
	    debugprintf("progconf_begin: 1 rs=%d\n",rs) ;
#endif

	if (rs >= 0) {
	    cchar	*cfn = pip->cfname ;
	    if (cfn == nullptr) cfn = CONFIGFNAME ;
	    rs = procfile(pip,usrconfs,cfn) ;
	}

#if	CF_DEBUG 
	if (DEBUGLEVEL(3))
	    debugprintf("progconf_begin: 2 rs=%d\n",rs) ;
#endif

	if ((rs >= 0) && (pip->config != nullptr)) {
	    PCONF	*csp = pip->config ;
	    rs = pconf_load(csp) ;
	}

	if (rs < 0) {
	    procfile_end(pip) ;
	}

#if	CF_DEBUG 
	if (DEBUGLEVEL(3))
	    debugprintf("progconf_begin: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (progconf_begin) */


int progconf_end(PI *pip)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (pip->config != nullptr) {
	    rs1 = procfile_end(pip) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if */

	return rs ;
}
/* end subroutine (progconf_end) */


int progconf_check(PI *pip)
{
	int		rs = SR_OK ;
	int		f = FALSE ;

	if (pip->config != nullptr) {
	    PCONF	*csp = pip->config ;

	    rs = pconf_check(csp,pip->daytime) ;
	    f = rs ;

	} /* end if (conf) */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (progconf_check) */


/* local subroutines */


local int procfile(PI *pip,cchar **confs,cchar *cfn)
{
	vecstr		*svp = &pip->svars ;
	const int	tlen = MAXPATHLEN ;
	const int	m = R_OK ;
	int		rs = SR_OK ;
	int		rs1 ;
	char		tfname[MAXPATHLEN+1] ;

	if ((rs1 = permsched(confs,svp,tfname,tlen,cfn,m)) >= 0) {
	    rs = procfile_load(pip,tfname) ;
	} else if (! isNotPresent(rs1))
	    rs = rs1 ;

	return rs ;
}
/* end subroutine (procfile) */


local int procfile_begin(PI *pip)
{
	int		rs = SR_OK ;

	if (pip->config == nullptr) {
	    const int	size = sizeof(PCONF) ;
	    void	*p ;
	    if ((rs = uc_malloc(size,&p)) >= 0) {
	        PCONF		*csp = p ;
	        cchar	**envv = pip->envv ;
	        pip->config = p ;
	        rs = pconf_start(csp,pip,envv) ;
	        if (rs < 0) {
	            uc_free(pip->config) ;
	            pip->config = nullptr ;
	        }
	    } /* end if (memory-allocation) */
	} /* end if (instatiation) */

	return rs ;
}
/* end subroutine (procfile_begin) */


local int procfile_end(PI *pip)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (pip->config != nullptr) {
	    PCONF	*csp = pip->config ;
	    rs1 = pconf_finish(csp) ;
	    if (rs >= 0) rs = rs1 ;
	    rs1 = uc_free(pip->config) ;
	    if (rs >= 0) rs = rs1 ;
	    pip->config = nullptr ;
	} /* end if */

	return rs ;
}
/* end subroutine (procfile_end) */


local int procfile_load(PI *pip,cchar *fname)
{
	int		rs = SR_OK ;

	if (pip->config == nullptr) {
	    rs = procfile_begin(pip) ;
#if	CF_DEBUG
	    if (DEBUGLEVEL(4))
	        debugprintf("progconf_/procfile_load: procfile_begin() rs=%d\n",
		    rs) ;
#endif
	} /* end if (instatiation) */

	if ((rs >= 0) && (pip->config != nullptr)) {
	    PCONF	*csp = pip->config ;
	    rs = pconf_fileadd(csp,fname) ;
#if	CF_DEBUG
	    if (DEBUGLEVEL(4))
	        debugprintf("progconf_/procfile_load: pconf_fileadd() "
		    "rs=%d\n",rs) ;
#endif
	}

	return rs ;
}
/* end subroutine (procfile_load) */


local int pconf_start(PCONF *csp,PI *pip,cchar **envv)
{

	memset(csp,0,sizeof(PCONF)) ;
	csp->pip = pip ;
	csp->envv = envv ;

	return SR_OK ;
}
/* end subroutine (pconf_start) */


local int pconf_finish(PCONF *csp)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (csp->nf > 0) {
	    csp->nf = 0 ;
	    rs1 = paramfile_close(&csp->params) ;
	    if (rs >= 0) rs = rs1 ;
	}

	return rs ;
}
/* end subroutine (pconf_finish) */


local int pconf_fileadd(PCONF *csp,cchar *fname)
{
	paramfile	*pfp = &csp->params ;
	int		rs ;
	if (csp->nf == 0) {
	    cchar	**envv = csp->envv ;
	    rs = paramfile_open(pfp,envv,fname) ;
	} else {
	    rs = paramfile_fileadd(pfp,fname) ;
	}
	if (rs >= 0) csp->nf += 1 ;
	return rs ;
}
/* end subroutine (pconf_fileadd) */


local int pconf_check(PCONF *csp,time_t dt)
{
	int		rs ;
	int		f = FALSE ;

	if ((rs = paramfile_check(&csp->params,dt)) > 0) {
	    if (csp->nf > 0) {
	        f = TRUE ;
	        rs = pconf_load(csp) ;
	    }
	} /* end if */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (pconf_check) */


local int pconf_load(PCONF *csp)
{
	PROGINFO	*pip = csp->pip ;
	paramfile	*pfp = &csp->params ;
	int		rs = SR_OK ;

	if (pip == nullptr) return SR_FAULT ; /* ¥ GCC false complaint */
	if (pfp == nullptr) return SR_FAULT ; /* ¥ GCC false complaint */

	if (csp->nf > 0) {
	    const int	plen = PBUFLEN ;
	    int		size ;
	    char	*pbuf ;
	    size = (plen+1) ;
	    if ((rs = uc_malloc(size,&pbuf)) >= 0) {
	        rs = pconf_loader(csp,pbuf,plen) ;
	        uc_free(pbuf) ;
	    } /* end if (memory-allocation) */
	} /* end if */

	return rs ;
}
/* end subroutine (pconf_load) */


local int pconf_loader(PCONF *csp,char *pbuf,int plen)
{
	PROGINFO	*pip = csp->pip ;
	paramfile	*pfp = &csp->params ;
	paramfile_cur	cur ;
	paramfile_ent	pe ;
	int		rs ;
	int		rs1 ;

	if ((rs = paramfile_curbegin(pfp,&cur)) >= 0) {
	    const int	elen = EBUFLEN ;
	    cchar	*kp, *vp ;
	    int		pi ;
	    int		kl, vl ;
	    int		el, tl ;
	    int		v ;
	    int		f ;
	    cchar	*pr = pip->pr ;
	    char	tfname[MAXPATHLEN + 1] ;
	    char	ebuf[EBUFLEN + 1] ;

	    while (rs >= 0) {
	        kl = paramfile_curenum(pfp,&cur,&pe,pbuf,plen) ;
	        if (kl == SR_NOTFOUND) break ;
	        rs = kl ;
	        if (rs < 0) break ;

	        kp = pe.key ;
	        vp = pe.value ;
	        vl = pe.vlen ;

#if	CF_DEBUG
	        if (DEBUGLEVEL(4))
	            debugprintf("progconf_read: enum k=%r\n",kp,kl) ;
#endif

	        pi = matpstr(params,2,kp,kl) ;
	        if (pi < 0) continue ;

#if	CF_DEBUG
	        if (DEBUGLEVEL(4))
	            debugprintf("progconf_read: param=%s (%u)\n",
	                params[pi],pi) ;
#endif

	        ebuf[0] = '\0' ;
	        el = 0 ;
	        if (vl > 0) {
	            el = expcook_exp(&pip->cooks,0,ebuf,elen,vp,vl) ;
	            if (el >= 0) ebuf[el] = '\0' ;
	        } /* end if */

#if	CF_DEBUG
	        if (DEBUGLEVEL(4)) {
	            debugprintf("progconf_read: ebuf=>%r<\n",
	                ebuf,strlinelen(ebuf,el,50)) ;
	            debugprintf("progconf_read: switch=%s(%u)\n",
	                params[pi],pi) ;
	        }
#endif

	        if (el < 0)
	            continue ;

	        switch (pi) {

	        case param_mailcheck:
	            if ((el > 0) && (! pip->finval.mailcheck)) {
	                if (hasalldig(ebuf,el)) {
	                    if ((rs = cfdecti(ebuf,el,&v)) >= 0) {
	                        if (v >= 0) {
	                            pip->have.mailcheck = TRUE ;
	                            pip->changed.mailcheck = TRUE ;
	                            pip->finval.mailcheck = TRUE ;
	                            pip->mailcheck = v ;
	                        }
	                    }
	                }
	            }
	            break ;

	        case param_loglen:
	        case param_logsize:
	            if ((el > 0) && (! pip->finval.logsize)) {
	                if (hasalldig(ebuf,el)) {
	                    rs1 = cfdecmfi(ebuf,el,&v) ;
	                    if ((rs1 >= 0) && (v >= 0)) {
	                        pip->have.logsize = TRUE ;
	                        pip->changed.logsize = TRUE ;
	                        pip->finval.logsize = TRUE ;
	                        pip->logsize = v ;
	                    }
	                }
	            }
	            break ;

	        case param_cmdkey:
	            if (! pip->finval.cmdfname) {
	                pip->have.cmdfname = TRUE ;
	                tl = prmkfname(pr,tfname,ebuf,el,TRUE,
	                    CMDMAPFNAME,pip->searchname,"") ;
	                f = (pip->cmdfname == nullptr) ;
	                f = f || (strcmp(pip->cmdfname,tfname) != 0) ;
	                if (f) {
	                    cchar	**vpp = &pip->cmdfname ;
	                    pip->finval.cmdfname = TRUE ;
	                    pip->changed.cmdfname = TRUE ;
	                    rs = proginfo_setentry(pip,vpp,tfname,tl) ;
	                }
	            }
	            break ;

	        case param_logfile:
	            if (! pip->finval.lfname) {
	                pip->have.lfname = TRUE ;
	                tl = prmkfname(pr,tfname,ebuf,el,TRUE,
	                    LOGCNAME,pip->searchname,"") ;
	                f = (pip->lfname == nullptr) ;
	                f = f || (strcmp(pip->lfname,tfname) != 0) ;
	                if (f) {
	                    cchar	**vpp = &pip->lfname ;
	                    pip->finval.lfname = TRUE ;
	                    pip->changed.lfname = TRUE ;
	                    rs = proginfo_setentry(pip,vpp,tfname,tl) ;
	                }
	            }
	            break ;

	        case param_testmsg:
	            if (el > 0) {
	                cchar	**vpp = &pip->testmsg ;
	                if (el > 76) el = 76 ;
	                rs = proginfo_setentry(pip,vpp,ebuf,el) ;
	            }
	            break ;

	        case param_auxprog:
	            if (el > 0) {
	                rs = procauxprog(pip,ebuf,el) ;
	            }
	            break ;

	        case param_mbdefault:
	        case param_mbinput:
	        case param_mbspam:
	        case param_mbtrash:
	            if (el > 0) {
	                cchar	**vpp = nullptr ;
	                switch (pi) {
	                case param_mbdefault:
	                    vpp = &pip->mbname_def ;
	                    break ;
	                case param_mbinput:
	                    vpp = &pip->mbname_in ;
	                    break ;
	                case param_mbspam:
	                    vpp = &pip->mbname_spam ;
	                    break ;
	                case param_mbtrash:
	                    vpp = &pip->mbname_trash ;
	                    break ;
	                } /* end switch */
	                rs = proginfo_setentry(pip,vpp,ebuf,el) ;
	            }
	            break ;

	        } /* end switch */

	        if (rs < 0) break ;
	    } /* end while (enumerating) */

	    paramfile_curend(pfp,&cur) ;
	} /* end if (paramfile-cur) */

	return rs ;
}
/* end subroutine (pconf_loader) */

local int procauxprog(PI *pip,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		vl = 0 ;
	int		kl ;
	int		pi ;
	cchar	*tp ;
	cchar	*kp = sp ;
	cchar	*vp ;

	if (sl < 0) sl = strlen(sp) ;

	if ((tp = strnchr(sp,sl,CH_FS)) != nullptr) {
	    kl = (tp-sp) ;
	    vp = (tp+1) ;
	    vl = ((sp+sl)-vp) ;
	    if (vl > 0) {
	        if ((pi = matstr(prognames,kp,kl)) >= 0) {
	            cchar	**vpp = nullptr ;
	            switch (pi) {
	            case progname_shell:
	                vpp = &pip->prog_shell ;
	                break ;
	            case progname_getmail:
	                vpp = &pip->prog_getmail ;
	                break ;
	            case progname_mailer:
	                vpp = &pip->prog_mailer ;
	                break ;
	            case progname_editor:
	                vpp = &pip->prog_editor ;
	                break ;
	            case progname_metamail:
	                vpp = &pip->prog_metamail ;
	                break ;
	            case progname_pager:
	                vpp = &pip->prog_pager ;
	                break ;
	            case progname_postspam:
	                vpp = &pip->prog_postspam ;
	                break ;
	            } /* end switch */
	            rs = proginfo_setentry(pip,vpp,vp,vl) ;
	        } /* end if */
	    } /* end if (non-zero value) */
	} /* end if (key-val pair) */

	return rs ;
}
/* end subroutine (procaxprog) */


