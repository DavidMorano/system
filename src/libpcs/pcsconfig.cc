/* pcsconfig SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* handle PCS configuration functions */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-03-01, David A­D­ Morano
	This code was originally written.  

*/

/* Copyright © 2004,2005,2011 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	pcsconfig

	Description:
	These subroutines form part of the PCS program (yes, getting
	a little bit more complicated every day now).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<climits>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<vecstr.h>
#include	<paramfile.h>
#include	<expcook.h>
#include	<logfile.h>
#include	<prmkfname.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"pcsmain.h"
#include	"pcsconfig.h"
#include	"pcslocinfo.h"
#include	"defs.h"


/* local defines */

#define	PC		pcsconfig

#ifndef	PBUFLEN
#define	PBUFLEN		(4 * MAXPATHLEN)
#endif

#ifndef	VBUFLEN
#define	VBUFLEN		(4 * MAXPATHLEN)
#endif

#ifndef	EBUFLEN
#define	EBUFLEN		(3 * MAXPATHLEN)
#endif

#define	PCS_REQCNAME	"req"


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	config_addcooks(PC *) noex ;
static int	config_reader(PC *,char *,char *,char *) noex ;


/* local variables */

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

constexpr cpcchar	params[] = {
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
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int config_start(PC *cfp,proginfo *pip,cchar *cfname,int intcheck) noex {
	expcook		*ckp ;
	int		rs = SR_OK ;

	if (cfp == nullptr) return SR_FAULT ;
	if (pip == nullptr) return SR_FAULT ;
	if (cfname == nullptr) return SR_FAULT ;

	if (cfname[0] == '\0') return SR_INVALID ;

/* start in */

	memset(cfp,0,sizeof(PC)) ;
	cfp->pip = pip ;
	cfp->intcheck = intcheck ;

	ckp = &cfp->cooks ;
	if ((rs = paramfile_open(&cfp->p,pip->envv,cfname)) >= 0) {
	    if ((rs = expcook_start(ckp)) >= 0) {
	        if ((rs = config_addcooks(cfp)) >= 0) {
	            cfp->fl.p = true ;
	            rs = config_read(cfp) ;
	        }
	    }
	    if (rs < 0)
	        expcook_finish(ckp) ;
	}
	if (rs < 0)
	    expcook_finish(ckp) ;

	return rs ;
}
/* end subroutine (config_start) */


int config_finish(PC *cfp)
{
	proginfo	*pip ;
	int		rs = SR_OK ;
	int		rs1 ;

	if (cfp == nullptr) return SR_FAULT ;

	pip = cfp->pip ;
	if (pip == nullptr) return SR_FAULT ;

	if (cfp->fl.p) {

	    rs1 = expcook_finish(&cfp->cooks) ;
	    if (rs >= 0) rs = rs1 ;

	    rs1 = paramfile_close(&cfp->p) ;
	    if (rs >= 0) rs = rs1 ;

	} else
	    rs = SR_NOTOPEN ;

	return rs ;
}
/* end subroutine (config_finish) */


int config_check(PC *cfp)
{
	proginfo	*pip ;
	int		rs = SR_OK ;
	int		f_changed = false ;

	if (cfp == nullptr)
	    return SR_FAULT ;

	pip = cfp->pip ;
	if (cfp->fl.p) {
	    const time_t	dt = pip->daytime ;
	    cint		intcheck = cfp->intcheck ;
	    int			f_check = false ;

	    f_check = f_check && (intcheck > 0) ;
	    f_check = f_check && ((dt - cfp->ti_lastcheck) >= intcheck) ;
	    if (f_check) {
	        cfp->ti_lastcheck = dt ;

	        if ((rs = paramfile_check(&cfp->p,dt)) > 0) {
	            f_changed = true ;
	            rs = config_read(cfp) ;
	        } /* end if (parameter file changed) */
	    } /* end if (needed a check) */

	} else {
	    rs = SR_NOTOPEN ;
	}

	return (rs >= 0) ? f_changed : rs ;
}
/* end subroutine (config_check) */


int config_read(PC *cfp)
{
	int		rs = SR_OK ;
	int		rs1 ;
	if (cfp->fl.p) {
	        int	sz = 0 ;
	        char	*abuf ;
	        sz += (PBUFLEN+1) ;
	        sz += (EBUFLEN+1) ;
	        sz += (MAXPATHLEN+1) ;
	        if ((rs = uc_malloc(sz,&abuf)) >= 0) {
		    {
		        char	*pbuf = abuf ;
		        char	*ebuf = (abuf+(PBUFLEN+1)) ;
		        char	*tbuf = (ebuf+(EBUFLEN+1)) ;
		        rs = config_reader(cfp,pbuf,ebuf,tbuf) ;
	            }
		    rs1 = uc_free(abuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	} /* end if (active) */
	return rs ;
}
/* end subroutine (config_read) */


/* private subroutines */


int config_reader(PC *cfp,char *pbuf,char *ebuf,char *tbuf)
{
	proginfo	*pip = cfp->pip ;
	PARAMFILE	*pfp = &cfp->p ;
	PARAMFILE_CUR	cur ;
	PARAMFILE_ENT	pe ;
	int		rs ;
	int		rs1 ;
	int		pi ;
	int		kl ;
	int		ml, vl, el ;

	if ((rs = paramfile_curbegin(pfp,&cur)) >= 0) {
	    LOCINFO	*lip = pip->lip ;
	    cint	elen = EBUFLEN ;
	    int		v ;
	    cchar	*pr = pip->pr ;
	    cchar	*ccp ;
	    cchar	*kp, *vp ;

	    while (rs >= 0) {
	        kl = paramfile_curenum(pfp,&cur,&pe,pbuf,PBUFLEN) ;
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

	        if (el < 0) continue ;

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
	                    if (! pip->fin.intrun)
	                        pip->intrun = v ;
	                    break ;
	                case param_mspoll:
	                case param_intpoll:
	                    if (! pip->fin.intpoll)
	                        pip->intpoll = v ;
	                    break ;
	                case param_intmark:
	                    if (! pip->fin.intmark)
	                        pip->intmark = v ;
	                    break ;
	                case param_intlock:
	                    if (! pip->fin.intlock)
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
	            if (! pip->fin.pidfname) {
	                pip->have.pidfname = true ;
	                rs1 = prsetfname(pr,tbuf,ebuf,el,true,
	                    RUNDNAME,pip->nodename,PIDFNAME) ;
	                ccp = pip->pidfname ;
	                if ((ccp == nullptr) ||
	                    (strcmp(ccp,tbuf) != 0)) {
			    cchar	**vpp = &pip->pidfname ;
	                    pip->changed.pidfname = true ;
	                    rs = proginfo_setentry(pip,vpp,tbuf,rs1) ;
	                }
	            }
	            break ;
	        case param_msfile:
	            if (! lip->final.msfname) {
	                lip->have.msfname = true ;
	                rs1 = prsetfname(pr,tbuf,ebuf,el,true,
	                    MSDNAME,MSFNAME,"") ;
	                ccp = lip->msfname ;
	                if ((ccp == nullptr) ||
	                    (strcmp(ccp,tbuf) != 0)) {
			    cchar	**vpp = &lip->msfname ;
	                    lip->changed.msfname = true ;
	                    rs = locinfo_setentry(lip,vpp,tbuf,rs1) ;
	                }
	            }
	            break ;
	        case param_logfile:
	            if (! pip->fin.logprog) {
	                pip->have.logprog = true ;
	                rs1 = prsetfname(pr,tbuf,ebuf,el,true,
	                    LOGDNAME,pip->searchname,"") ;
	                ccp = pip->lfname ;
	                if ((ccp == nullptr) ||
	                    (strcmp(ccp,tbuf) != 0)) {
			    cchar	**vpp = &pip->lfname ;
	                    pip->changed.logprog = true ;
	                    rs = proginfo_setentry(pip,vpp,tbuf,rs1) ;
	                }
	            } /* end if */
	            break ;
	        case param_reqfile:
	            if (! lip->final.reqfname) {
	                lip->have.reqfname = true ;
#ifdef	COMMENT
	                rs1 = prsetfname(pr,tbuf,ebuf,el,true,
	                    LOGDNAME,pip->searchname,"") ;
#else			    
	                rs1 = mkpath1w(tbuf,ebuf,el) ;
#endif /* COMMENT */
	                ccp = lip->reqfname ;
	                if ((ccp == nullptr) ||
	                    (strcmp(ccp,tbuf) != 0)) {
			    cchar	**vpp = &lip->reqfname ;
	                    lip->changed.reqfname = true ;
	                    rs = locinfo_setentry(lip,vpp,tbuf,rs1) ;
	                }
	            } /* end if */
	            break ;
	        case param_cmd:
	            ml = MIN(LOGIDLEN,el) ;
	            if (ml && (lip->cmd[0] == '\0'))
	                strwcpy(lip->cmd,ebuf,ml) ;
	            break ;
	        case param_speedname:
	            if (! lip->final.speedname) {
	                lip->have.speedname = true ;
	                ccp = lip->speedname ;
	                if ((ccp == nullptr) ||
	                    (strwcmp(ccp,ebuf,el) != 0)) {
			    cchar	**vpp = &lip->speedname ;
	                    lip->changed.speedname = true ;
	                    rs = locinfo_setentry(lip,vpp,ebuf,el) ;
	                }

	            } /* end if */
	            break ;
	        } /* end switch */
	    } /* end while (fetching) */

	    rs1 = paramfile_curend(&cfp->p,&cur) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (paramfile-cur) */

	return rs ;
}
/* end subroutine (config_readrer) */

static int config_addcooks(PC *cfp) noex {
	proginfo	*pip = cfp->pip ;
	expcook		*ckp = &cfp->cooks ;
	int		rs = SR_OK ;
	int		i ;
	cchar		*keys = "PSNDHRU" ;

	if (pip == nullptr) return SR_FAULT ;
	for (i = 0 ; keys[i] ; i += 1) {
	    cint	kch = mkchar(keys[i]) ;
	    int		vl = -1 ;
  	    cchar	*vp = nullptr ;
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
		    cint	hlen = MAXHOSTNAMELEN ;
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
	    if ((rs >= 0) && (vp != nullptr)) {
		char	kbuf[2] = { 0, 0 } ;
		kbuf[0] = kch ;
		rs = expcook_add(ckp,kbuf,vp,vl) ;
	    }
	    if (rs < 0) break ;
	} /* end for */

	return rs ;
}
/* end subroutine (config_addcooks) */


