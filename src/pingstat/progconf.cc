/* progconf */

/* program configuration */
/* version %I% last-modified %G% */


#define	CF_DEBUGS	0		/* non-switchable debug print-outs */
#define	CF_DEBUG	0		/* switchable at invocation */


/* revision history:

	= 2008-10-10, David A�D� Morano
	This was adapted from the FINGERS program.

*/

/* Copyright � 2008 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	This module contains the subroutines that manage program configuration.


*******************************************************************************/


#include	<envstandards.h>	/* MUST be first to configure */

#include	<sys/types.h>
#include	<sys/param.h>
#include	<climits>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstdlib>
#include	<cstring>
#include	<ctype.h>
#include	<time.h>
#include	<netdb.h>

#include	<usystem.h>
#include	<paramfile.h>
#include	<expcook.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"


/* local defines */

#ifndef	PBUFLEN
#define	PBUFLEN		(4 * MAXPATHLEN)
#endif

#ifndef	VBUFLEN
#define	VBUFLEN		(4 * MAXPATHLEN)
#endif

#ifndef	EBUFLEN
#define	EBUFLEN		(4 * MAXPATHLEN)
#endif

#define	SUMFEXT		"sum"


/* external subroutines */

extern int	snsds(char *,int,const char *,const char *) ;
extern int	snwcpy(char *,int,const char *,int) ;
extern int	mkpath1(char *,const char *) ;
extern int	mkpath2(char *,const char *,const char *) ;
extern int	mkpath3(char *,const char *,const char *,const char *) ;
extern int	mkfnamesuf1(char *,const char *,const char *) ;
extern int	matstr(const char **,const char *,int) ;
extern int	matostr(const char **,int,const char *,int) ;
extern int	matpstr(const char **,int,const char *,int) ;
extern int	cfdeci(const char *,int,int *) ;
extern int	cfdecmfi(const char *,int,int *) ;
extern int	cfdecmfu(const char *,int,uint *) ;
extern int	cfdecti(const char *,int,int *) ;
extern int	permsched(cchar **,vecstr *,char *,int,cchar *,int) ;
extern int	prsetfname(cchar *,char *,cchar *,int,int,
			cchar *,cchar *,cchar *) ;
extern int	isNotPresent(int) ;

extern int	securefile(const char *,uid_t,gid_t) ;

#if	CF_DEBUGS || CF_DEBUG
extern int	debugprintf(const char *,...) ;
extern int	strlinelen(const char *,int,int) ;
#endif

extern char	*strwcpy(char *,const char *,int) ;


/* forward references */

int		progconf_read(PROGINFO *) ;


/* local variables */

static const char	*schedpconf[] = {
	"%p/%e/%n/%n.%f",
	"%p/%e/%n/%f",
	"%p/%e/%n.%f",
	"%p/%n.%f",
	NULL
} ;

static const char	*configopts[] = {
	"pidfile",
	"logfile",
	"sumfile",
	"logsize",
	"markint",
	"minpingint",
	"minupdateint",
	"pingto",
	NULL
} ;

enum configopts {
	configopt_pidfile,
	configopt_logfile,
	configopt_sumfile,
	configopt_logsize,
	configopt_markint,
	configopt_minpingint,
	configopt_minupdateint,
	configopt_pingto,
	configopt_overlast
} ;


/* exported subroutines */


int progconf_begin(PROGINFO *pip)
{
	const int	tlen = MAXPATHLEN ;
	int		rs = SR_OK ;
	cchar		*cfn = pip->cfname ;
	cchar		**schedp = schedpconf ;
	char		tbuf[MAXPATHLEN + 1] ;

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("proconfigbegin: ent cfn=%s\n",pip->cfname) ;
#endif

#if	CF_DEBUG
	if (DEBUGLEVEL(3)) {
	    int	i ;
	    debugprintf("progconfbegin: search-schedule:\n") ;
	    for (i = 0 ; schedp[i] != NULL ; i += 1)
	        debugprintf("progconfbegin: sched%u=%s\n",i,schedp[i]) ;
	}
#endif /* CF_DEBUG */

	tbuf[0] = '\0' ;
	if (cfn == NULL) {
	    const int	am = R_OK ;
	    cchar	**vpp = &pip->cfname ;
	    cchar	*def = CONFIGFNAME ;

	    if ((rs = permsched(schedp,&pip->svars,tbuf,tlen,def,am)) >= 0) {
	        rs = proginfo_setentry(pip,vpp,tbuf,rs) ;
		cfn = pip->cfname ;
	    } else if (isNotPresent(rs)) {
		rs = SR_OK ;
	    }

	} /* end if (null) */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("progconfbegin: mid1 rs=%d\n",rs) ;
#endif

	if ((rs >= 0) && (cfn != NULL)) {

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("progconfbegin: cfn=%s\n",cfn) ;
#endif

	    if ((rs = paramfile_open(&pip->params,pip->envv,cfn)) >= 0) {
	        pip->f.pc = TRUE ;
	        pip->open.params = TRUE ;
	        rs = progconf_read(pip) ;
		if (rs < 0) {
	    	    pip->open.params = FALSE ;
	    	    paramfile_close(&pip->params) ;
		}
	    }
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("progconfbegin: ret rs=%d f_pc=%u\n",rs,pip->f.pc) ;
#endif

	return rs ;
}
/* end subroutine (progconf_begin) */


int progconf_end(PROGINFO *pip)
{
	int		rs = SR_NOTOPEN ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("progconf_end: ent f_pc=%u\n",pip->f.pc) ;
#endif

	if (pip->f.pc) {
	    pip->f.pc = FALSE ;
	    rs = SR_OK ;
	    if (pip->open.params) {
	        pip->open.params = FALSE ;
	        rs = paramfile_close(&pip->params) ;
	    }
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("progconf_end: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (progconf_end) */


int progconf_check(PROGINFO *pip)
{
	int		rs = SR_OK ;
	int		f = FALSE ;

	if (pip->f.pc && pip->open.params) {
	    PARAMFILE	*pfp = &pip->params ;
	    if ((rs = paramfile_check(pfp,pip->daytime)) > 0) {
	        f = TRUE ;
	        rs = progconf_read(pip) ;
	    }
	} /* end if */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (progconf_check) */


int progconf_read(PROGINFO *pip)
{
	int		rs = SR_OK ;
	int		rs1 = 0 ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("progconf_read: f_pc=%u open_params=%u\n",
	        pip->f.pc,pip->open.params) ;
#endif

	if (pip->open.params) {
	PARAMFILE_CUR	cur ;
	PARAMFILE_ENT	pe ;
	int		oi ;
	int		kl ;
	int		vl ;
	int		elen ;
	int		tl ;
	int		v ;
	int		f ;
	const char	*kp, *vp ;
	cchar		*pr = pip->pr ;
	char		tbuf[MAXPATHLEN + 1] ;
	char		pbuf[PBUFLEN + 1] ;
	char		ebuf[EBUFLEN + 1] ;
	if ((rs = paramfile_curbegin(&pip->params,&cur)) >= 0) {

	while (rs >= 0) {

	    kl = paramfile_curenum(&pip->params,&cur,&pe,pbuf,PBUFLEN) ;

#if	CF_DEBUG
	    if (DEBUGLEVEL(4))
	        debugprintf("progconf_read: paramfile_curenum() rs=%d\n",
	            kl) ;
#endif

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

	    oi = matpstr(configopts,2,kp,kl) ;
	    if (oi < 0) continue ;

	    ebuf[0] = '\0' ;
	    elen = 0 ;
	    if (vl > 0) {
	        elen = expcook_exp(&pip->cooks,0,ebuf,EBUFLEN,vp,vl) ;
	        if (elen >= 0) ebuf[elen] = '\0' ;
	    } /* end if */

#if	CF_DEBUG
	    if (DEBUGLEVEL(4))
	        debugprintf("progconf_read: ebuf=>%r<\n",ebuf,elen) ;
#endif

	    if (elen < 0) continue ;

	    switch (oi) {
	    case configopt_logsize:
	        if ((elen > 0) && (! pip->final.logsize)) {
	            rs1 = cfdecmfi(ebuf,elen,&v) ;
	            if ((rs1 >= 0) && (v >= 0)) {
	                pip->have.logsize = TRUE ;
	                pip->changed.logsize = TRUE ;
	                pip->logsize = v ;
	            }
	        }
	        break ;
	    case configopt_markint:
	case configopt_minpingint:
	case configopt_minupdateint:
	case configopt_pingto:
	        v = -1 ;
	        if (elen > 0) {
	            rs1 = cfdecti(ebuf,elen,&v) ;
		}
	        if ((rs1 >= 0) && (v >= 0)) {
	            switch (oi) {
	            case configopt_markint:
	                if (! pip->final.intmark) {
	                    pip->have.intmark = TRUE ;
	                    pip->changed.intmark = TRUE ;
	                    pip->intmark = v ;
	                }
	                break ;
	            case configopt_minpingint:
	                if (! pip->final.intminping) {
	                    pip->have.intminping = TRUE ;
	                    pip->changed.intminping = TRUE ;
	                    pip->intminping = v ;
	                }
	                break ;
	            case configopt_minupdateint:
	                if (! pip->final.intminupdate) {
	                    pip->have.intminupdate = TRUE ;
	                    pip->changed.intminupdate = TRUE ;
	                    pip->intminupdate = v ;
	                }
	                break ;
	            case configopt_pingto:
	                if (! pip->final.toping) {
	                    pip->have.toping = TRUE ;
	                    pip->changed.toping = TRUE ;
	                    pip->toping = v ;
	                }
	                break ;
	            } /* end switch */
	        } /* end if (valid number) */
	        break ;
	    case configopt_sumfile:
	        if (! pip->final.sumfile) {
	            char	dname[MAXPATHLEN + 1] ;
	            pip->have.sumfile = TRUE ;
	            mkpath2(dname,VDNAME,pip->searchname) ;
	            tl = prsetfname(pr,tbuf,ebuf,elen,TRUE,
	                dname,pip->nodename,SUMFEXT) ;
	            f = (pip->sumfname == NULL) ;
	            f = f || (strcmp(pip->sumfname,tbuf) != 0) ;
	            if (f) {
	                pip->changed.sumfile = TRUE ;
	                rs = proginfo_setentry(pip,&pip->sumfname,
	                    tbuf,tl) ;
	            }
	        }
	        break ;
	    case configopt_pidfile:
	        if (! pip->final.pfname) {
	            pip->have.pfname = TRUE ;
	            tl = prsetfname(pr,tbuf,ebuf,elen,TRUE,
	                RUNDNAME,pip->nodename,pip->searchname) ;
	            f = (pip->pfname == NULL) ;
	            f = f || (strcmp(pip->pfname,tbuf) != 0) ;
	            if (f) {
			cchar	**vpp = &pip->pfname ;
	                pip->changed.pfname = TRUE ;
	                rs = proginfo_setentry(pip,vpp,tbuf,tl) ;
	            }
	        }
	        break ;
	    case configopt_logfile:
	        if (! pip->final.logfile) {
	            pip->have.logfile = TRUE ;
	            tl = prsetfname(pr,tbuf,ebuf,elen,TRUE,
	                LOGDNAME,pip->searchname,"") ;
	            f = (pip->lfname == NULL) ;
	            f = f || (strcmp(pip->lfname,tbuf) != 0) ;
	            if (f) {
			cchar	**vpp = &pip->lfname ;
	                pip->changed.logfile = TRUE ;
	                rs = proginfo_setentry(pip,vpp,tbuf,tl) ;
	            }
	        }
	        break ;
	    } /* end switch */

	    if (rs < 0) break ;
	} /* end while (enumerating) */

	        paramfile_curend(&pip->params,&cur) ;
	    } /* end if (paramfile_curbegin) */
	} /* end if (pip->open.params) */

	return rs ;
}
/* end subroutine (progconf_read) */


/* local subroutines */


