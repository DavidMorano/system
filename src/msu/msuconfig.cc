/* msu-config SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* handle MSU configuration functions */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable debug print-outs */
#define	CF_DEBUG	0		/* switchable at invocation */

/* revision history:

	= 2004-03-01, David A�D� Morano
	This code was separated out from the main code.

*/

/* Copyright � 2004,2005,2011 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

  	Description:
	These subroutines form part of the MSU program (yes, getting
	a little bit more complicated every day now).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<vecstr.h>
#include	<paramfile.h>
#include	<expcook.h>
#include	<prmkfname.h>
#include	<strwcpy.h>
#include	<strwcmp.h>
#include	<localmisc.h>

#include	"msumain.h"
#include	"msuconfig.h"
#include	"msulocinfo.h"
#include	"defs.h"


/* local typedefs */

#ifndef	TYPEDEF_CCHAR
#define	TYPEDEF_CCHAR	1
typedef const char	cchar ;
#endif


/* local defines */

#ifndef	PBUFLEN
#define	PBUFLEN		(4 * MAXPATHLEN)
#endif

#ifndef	VBUFLEN
#define	VBUFLEN		(4 * MAXPATHLEN)
#endif

#ifndef	EBUFLEN
#define	EBUFLEN		(3 * MAXPATHLEN)
#endif

#ifndef	DIGBUFLEN
#define	DIGBUFLEN	40		/* can hold int128_t in decimal */
#endif


/* external subroutines */

extern int	snsd(char *,int,const char *,uint) ;
extern int	snsds(char *,int,const char *,const char *) ;
extern int	sncpy1(char *,int,const char *) ;
extern int	sncpy2(char *,int,const char *,const char *) ;
extern int	sncpy3(char *,int,const char *,const char *,const char *) ;
extern int	mkfnamesuf1(char *,const char *,const char *) ;
extern int	mkpath1w(char *,const char *,int) ;
extern int	mkpath1(char *,const char *) ;
extern int	mkpath2(char *,const char *,const char *) ;
extern int	mkpath3(char *,const char *,const char *,const char *) ;
extern int	sfdirname(const char *,int,const char **) ;
extern int	sfshrink(const char *,int,const char **) ;
extern int	matstr(const char **,const char *,int) ;
extern int	matostr(const char **,int,const char *,int) ;
extern int	cfdeci(const char *,int,int *) ;
extern int	cfdecui(const char *,int,uint *) ;
extern int	cfdecti(const char *,int,int *) ;
extern int	cfdecmfi(const char *,int,int *) ;
extern int	ctdeci(char *,int,int) ;
extern int	optbool(const char *,int) ;

#if	CF_DEBUGS || CF_DEBUG
extern int	debugprintf(const char *,...) ;
extern int	strlinelen(const char *,int,int) ;
#endif


/* external variables */


/* local structures */


/* forward references */

static int	config_reader(CONFIG *cfp) ;
static int	config_addcooks(CONFIG *) ;


/* local variables */

static cchar	*params[] = {
	"cmd",
	"logsize",
	"msfile",
	"pidfile",
	"runtime",
	"runint",
	"mspoll",
	"pollint",
	"markint",
	"lockint",
	"speedint",
	"logfile",
	"reqfile",
	"speedname",
	NULL
} ;

enum params {
	param_cmd,
	param_logsize,
	param_msfile,
	param_pidfile,
	param_runtime,
	param_intrun,
	param_mspoll,
	param_intpoll,
	param_intmark,
	param_intlock,
	param_intspeed,
	param_logfile,
	param_reqfile,
	param_speedname,
	param_overlast
} ;


/* exported subroutines */


int config_start(CONFIG *cfp,PROGINFO *pip,cchar *cfname,int intcheck)
{
	EXPCOOK		*ckp ;
	int		rs ;

	if (cfp == NULL) return SR_FAULT ;
	if (pip == NULL) return SR_FAULT ;
	if (cfname == NULL) return SR_FAULT ;

	if (cfname[0] == '\0') return SR_INVALID ;

/* start in */

	memset(cfp,0,sizeof(CONFIG)) ;
	cfp->pip = pip ;
	cfp->intcheck = intcheck ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4)) {
	    debugprintf("config_start: cfname=%s\n",cfname) ;
	    debugprintf("config_start: nodename=%s\n",pip->nodename) ;
	    debugprintf("config_start: domainname=%s\n",pip->domainname) ;
	}
#endif

	ckp = &cfp->cooks ;
	if ((rs = paramfile_open(&cfp->p,pip->envv,cfname)) >= 0) {
#if	CF_DEBUG
	    if (DEBUGLEVEL(4))
	        debugprintf("config_start: paramfile_open rs=%d\n",rs) ;
#endif
	    if ((rs = expcook_start(ckp)) >= 0) {
#if	CF_DEBUG
	        if (DEBUGLEVEL(4))
	            debugprintf("config_start: expcook_start rs=%d\n",rs) ;
#endif
	        if ((rs = config_addcooks(cfp)) >= 0) {
#if	CF_DEBUG
	            if (DEBUGLEVEL(4))
	                debugprintf("config_start: config_addcooks rs=%d\n",
				rs) ;
#endif
	            cfp->fl.p = TRUE ;
	            rs = config_read(cfp) ;
#if	CF_DEBUG
	            if (DEBUGLEVEL(4))
	                debugprintf("config_start: config_read rs=%d\n",rs) ;
#endif
	        }
	    }
	    if (rs < 0)
	        expcook_finish(ckp) ;
	} /* end if (paramfile_open) */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("config_start: ret rs=%d \n",rs) ;
#endif

	return rs ;
}
/* end subroutine (config_start) */


int config_finish(CONFIG *cfp)
{
	PROGINFO	*pip ;
	int		rs = SR_OK ;
	int		rs1 ;

#if	CF_DEBUGS
	debugprintf("config_finish: ent\n") ;
#endif

	if (cfp == NULL) return SR_FAULT ;

	pip = cfp->pip ;
	if (pip == NULL) return SR_FAULT ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("config_finish: ent %c\n",
		((cfp->fl.p) ? '�' : '_')) ;
#endif

	if (cfp->fl.p) {

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("config_finish: fins\n") ;
#endif

	    rs1 = expcook_finish(&cfp->cooks) ;
	    if (rs >= 0) rs = rs1 ;

	    rs1 = paramfile_close(&cfp->p) ;
	    if (rs >= 0) rs = rs1 ;

	} else {
	    rs = SR_NOTOPEN ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("config_finish: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (config_finish) */


int config_check(CONFIG *cfp)
{
	PROGINFO	*pip ;
	int		rs = SR_OK ;
	int		f_changed = FALSE ;

	if (cfp == NULL) return SR_FAULT ;

	pip = cfp->pip ;
	if (cfp->fl.p) {
	    const time_t	dt = pip->daytime ;
	    const int		intcheck = cfp->intcheck ;
	    int			f_check = TRUE ;

	    f_check = f_check && (intcheck > 0) ;
	    f_check = f_check && ((dt - cfp->ti_lastcheck) >= intcheck) ;
	    if (f_check) {
	        cfp->ti_lastcheck = dt ;

#if	CF_DEBUG
	        if (DEBUGLEVEL(4))
	            debugprintf("msumain/config_check: paramfile_check()\n") ;
#endif

	        if ((rs = paramfile_check(&cfp->p,dt)) > 0) {
	            f_changed = TRUE ;
	            rs = config_read(cfp) ;

#if	CF_DEBUG
	            if (DEBUGLEVEL(4))
	                debugprintf("msumain/config_check: "
	                    "config_read() rs=%d\n",rs) ;
#endif

	        } /* end if (parameter file changed) */
	    } /* end if (needed a check) */

	} else {
	    rs = SR_NOTOPEN ;
	}

	return (rs >= 0) ? f_changed : rs ;
}
/* end subroutine (config_check) */


int config_read(CONFIG *cfp)
{
	int		rs = SR_OK ;
	if (cfp->fl.p) {
	    rs = config_reader(cfp) ;
	}
	return rs ;
}
/* end subroutine (config_read) */


/* private subroutines */


static int config_reader(CONFIG *cfp)
{
	PROGINFO	*pip = cfp->pip ;
	LOCINFO		*lip ;
	PARAMFILE	*pfp = &cfp->p ;
	PARAMFILE_CUR	cur ;
	PARAMFILE_ENT	pe ;
	int		rs ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("config_read: f_p=%u\n",cfp->fl.p) ;
#endif

	lip = pip->lip ;
	if ((rs = paramfile_curbegin(pfp,&cur)) >= 0) {
	    const int	plen = PBUFLEN ;
	    const int	elen = EBUFLEN ;
	    int		rs1 ;
	    int		pi ;
	    int		kl ;
	    int		ml, vl, el ;
	    int		v ;
	    cchar	*pr = pip->pr ;
	    const char	*ccp ;
	    const char	*kp, *vp ;
	    char	pbuf[PBUFLEN + 1] ;
	    char	ebuf[EBUFLEN + 1] ;
	    char	tbuf[MAXPATHLEN + 1] ;

	    while (rs >= 0) {
	        kl = paramfile_curenum(pfp,&cur,&pe,pbuf,plen) ;
	        if (kl == SR_NOTFOUND) break ;
	        rs = kl ;
	        if (rs < 0) break ;

	        kp = pe.key ;
	        vp = pe.value ;
	        vl = pe.vlen ;

	        pi = matostr(params,2,kp,kl) ;
	        if (pi < 0) continue ;

	        ebuf[0] = '\0' ;
	        el = 0 ;
	        if (vl > 0) {
	            el = expcook_exp(&cfp->cooks,0,ebuf,elen,vp,vl) ;
	            if (el >= 0)
	                ebuf[el] = '\0' ;
	        } /* end if */

#if	CF_DEBUG
	        if (DEBUGLEVEL(4)) {
	            debugprintf("config_read: ebuf=>%r<\n",ebuf,el) ;
	            debugprintf("config_read: param=%s(%u)\n",
	                params[pi],pi) ;
	        }
#endif

	        if (el < 0)
	            continue ;

	        switch (pi) {
	        case param_logsize:
	            if (el > 0) {
	                rs1 = cfdecmfi(ebuf,el,&v) ;
	                if (rs1 >= 0)
	                    pip->logsize = v ;
	            }
	            break ;
	        case param_intrun:
	        case param_intpoll:
	        case param_intmark:
	        case param_intlock:
	        case param_intspeed:
	            if ((rs = cfdecti(ebuf,el,&v)) >= 0) {
	                switch (pi) {
	                case param_intrun:
	                    if (! pip->final.intrun)
	                        pip->intrun = v ;
	                    break ;
	                case param_mspoll:
	                case param_intpoll:
	                    if (! pip->final.intpoll)
	                        pip->intpoll = v ;
	                    break ;
	                case param_intmark:
	                    if (! pip->final.intmark)
	                        pip->intmark = v ;
	                    break ;
	                case param_intlock:
	                    if (! pip->final.intlock)
	                        pip->intlock = v ;
	                    break ;
	                case param_intspeed:
			    if (! lip->final.intspeed)
	                        lip->intspeed = v ;
	                    break ;
	                } /* end switch */
	            } /* end if (cfdecti) */
	            break ;
	        case param_pidfile:
	            if (! pip->final.pidfname) {
	                pip->have.pidfname = TRUE ;
	                rs1 = prsetfname(pr,tbuf,ebuf,el,TRUE,
	                    RUNDNAME,pip->nodename,PIDFNAME) ;
	                ccp = pip->pidfname ;
	                if ((ccp == NULL) ||
	                    (strcmp(ccp,tbuf) != 0)) {
			    const char	**vpp = &pip->pidfname ;
	                    pip->changed.pidfname = TRUE ;
	                    rs = proginfo_setentry(pip,vpp,tbuf,rs1) ;
	                }
	            }
	            break ;
	        case param_msfile:
	            if (! lip->final.msfname) {
	                lip->have.msfname = TRUE ;
	                rs1 = prsetfname(pr,tbuf,ebuf,el,TRUE,
	                    MSDNAME,MSFNAME,"") ;
	                ccp = lip->msfname ;
	                if ((ccp == NULL) ||
	                    (strcmp(ccp,tbuf) != 0)) {
			    const char	**vpp = &lip->msfname ;
	                    lip->changed.msfname = TRUE ;
	                    rs = locinfo_setentry(lip,vpp,tbuf,rs1) ;
	                }
	            }
	            break ;
	        case param_logfile:
	            if (! pip->final.logprog) {
	                pip->have.logprog = TRUE ;
	                rs1 = prsetfname(pr,tbuf,ebuf,el,TRUE,
	                    LOGDNAME,pip->searchname,"") ;
	                ccp = pip->lfname ;
	                if ((ccp == NULL) ||
	                    (strcmp(ccp,tbuf) != 0)) {
			    const char	**vpp = &pip->lfname ;
	                    pip->changed.logprog = TRUE ;
	                    rs = proginfo_setentry(pip,vpp,tbuf,rs1) ;
	                }
	            } /* end if */
	            break ;
	        case param_reqfile:
	            if (! lip->final.reqfname) {
	                lip->have.reqfname = TRUE ;
#ifdef	COMMENT
	                rs1 = prsetfname(pr,tbuf,ebuf,el,TRUE,
	                    LOGDNAME,pip->searchname,"") ;
#else			    
	                rs1 = mkpath1w(tbuf,ebuf,el) ;
#endif /* COMMENT */
	                ccp = lip->reqfname ;
	                if ((ccp == NULL) ||
	                    (strcmp(ccp,tbuf) != 0)) {
			    const char	**vpp = &lip->reqfname ;
	                    lip->changed.reqfname = TRUE ;
	                    rs = locinfo_setentry(lip,vpp,tbuf,rs1) ;
	                }
	            } /* end if */
	            break ;
	        case param_cmd:
	            ml = MIN(LOGIDLEN,el) ;
	            if (ml && (lip->cmd[0] == '\0')) {
	                strwcpy(lip->cmd,ebuf,ml) ;
		    }
	            break ;
	        case param_speedname:
	            if (! lip->final.speedname) {
	                lip->have.speedname = TRUE ;
	                ccp = lip->speedname ;
	                if ((ccp == NULL) ||
	                    (strwcmp(ccp,ebuf,el) != 0)) {
			    const char	**vpp = &lip->speedname ;
	                    lip->changed.speedname = TRUE ;
	                    rs = locinfo_setentry(lip,vpp,ebuf,el) ;
	                }
	            } /* end if */
	            break ;
	        } /* end switch */
	    } /* end while (fetching) */

	    rs1 = paramfile_curend(&cfp->p,&cur) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (parameters) */

	return rs ;
}
/* end subroutine (config_reader) */


static int config_addcooks(CONFIG *cfp)
{
	PROGINFO	*pip = cfp->pip ;
	EXPCOOK		*ckp = &cfp->cooks ;
	int		rs = SR_OK ;
	int		i ;
	cchar		*keys = "PSNDHRU" ;

	if (pip == NULL) return SR_FAULT ;
	for (i = 0 ; keys[i] ; i += 1) {
	    const int	kch = MKCHAR(keys[i]) ;
	    int		vl = -1 ;
  	    cchar	*vp = NULL ;
	    switch (kch) {
	    case 'P':
		vp = pip->progname ;
		break ;
	    case 'S':
		vp = pip->searchname ;
		break ;
	    case 'N':
		vp = pip->nodename ;
		break ;
	    case 'D':
		vp = pip->domainname ;
		break ;
	    case 'H':
		{
		    const int	hlen = MAXHOSTNAMELEN ;
		    cchar	*nn = pip->domainname ;
		    cchar	*dn = pip->nodename ;
		    char	hbuf[MAXHOSTNAMELEN+1] ;
	            if ((rs = snsds(hbuf,hlen,nn,dn)) >= 0) {
			vl = rs ;
	    		rs = expcook_add(ckp,"H",hbuf,rs) ;
		    }
		}
		break ;
	    case 'R':
		vp = pip->pr ;
		break ;
	    case 'U':
		vp = pip->username ;
		break ;
	    } /* end switch */
	    if ((rs >= 0) && (vp != NULL)) {
		char	kbuf[2] = { 0, 0 } ;
		kbuf[0] = kch ;
		rs = expcook_add(ckp,kbuf,vp,vl) ;
	    }
	    if (rs < 0) break ;
	} /* end for */

	return rs ;
}
/* end subroutine (config_addcooks) */


