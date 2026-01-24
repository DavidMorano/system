/* sd_ussinfo SUPPORT (Sys-Dialer) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* USSINFO helper object */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable debug print-outs */

/* revision history:

	= 2003-11-04, David A­D­ Morano
	This is a helper object for the various USS-type SYSDIALERs.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	ussinfo

	Description:
	This is a helper object for the various USS-type SYSDIALERs.
	The use of this object allows for common code among all of
	the USS-type SYSDIALERs.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<strings.h>		/* for |strcasecmp(3c)| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<keyopt.h>
#include	<field.h>
#include	<ids.h>
#include	<vecstr.h>
#include	<userinfo.h>
#include	<nulstr.h>
#include	<hasx.h>
#include	<isx.h>
#include	<localmisc.h>

#include	"sysdialer.h"
#include	"sd_ussinfo.h"
#include	"sd_uss.h"
#include	"envs.h"
#include	"inetaddrparse.h"


/* local defines */

#define	USSINFO_VARPR		"LOCAL"
#define	USSINFO_PR		"/usr/add-on/local"
#define	USSINFO_LOGDNAME	"log"
#define	USSINFO_LOGFNAME	SYSDIALER_LF

#ifndef	SVCNAMELEN
#define	SVCNAMELEN	32
#endif

#define	ARGBUFLEN	(MAXPATHLEN + 35)


/* external subroutines */


/* external variables */

extern char	**environ ;


/* local structures */


/* forward references */

local int	ussinfo_userinfo(USSINFO *) ;
local int	ussinfo_addrparseunix(USSINFO *,int) ;
local int	ussinfo_addrparseinet(USSINFO *) ;
local int	ussinfo_dirok(USSINFO *,cchar *,int) ;
local int	ussinfo_setentry(USSINFO *,cchar **,
			cchar *,int) ;


/* external variables (module information) */


/* local variables */

enum argopts {
	argopt_root,
	argopt_rn,
	argopt_sn,
	argopt_af,
	argopt_lf,
	argopt_pvf,
	argopt_pf,
	argopt_df,
	argopt_xf,
	argopt_ef,
	argopt_overlast
} ; /* end enum (argopts) */

constexpr cpcchar	argopts[] = {
	"ROOT",
	"RN",
	"sn",
	"af",
	"lf",
	"pvf",
	"pf",
	"df",
	"xf",
	"ef",
	nullptr
} ; /* end array */

enum procopts {
	procopt_log,
	procopt_overlast
} ; /* end enum (procopts) */

constexpr cpcchar	procopts[] = {
	"log",
	nullptr
} ; /* end array */


/* exported variables */


/* exported subroutines */

int ussinfo_start(sip,op,dip,ap,hostname,svcname)
USSINFO		*sip ;
void		*op ;
SYSDIALER_INFO	*dip ;
SYSDIALER_ARGS	*ap ;
cchar	hostname[] ;
cchar	svcname[] ;
{
	int	rs = SR_OK ;


	memset(sip,0,sizeof(USSINFO)) ;

	sip->envv = (cchar **) environ ;
	sip->op = op ;
	sip->searchname = dip->name ;
	sip->version = dip->version ;
	sip->ap = ap ;
	sip->pr = ap->pr ;
	sip->prn = ap->prn ;
	sip->to = ap->timeout ;
	sip->hostname = hostname ;
	sip->svcname = svcname ;
	sip->af = -1 ;

	if ((rs = vecstr_start(&sip->stores,3,0)) >= 0) {
	    if ((rs = vecstr_start(&sip->aenvs,3,0)) >= 0) {
		if (ap != nullptr) {
	    	    rs = ussinfo_procargs(sip) ;
		}
	        if (rs < 0)
		    vecstr_finish(&sip->aenvs) ;
	    } /* end if (aenvs) */
	    if (rs < 0)
		vecstr_finish(&sip->stores) ;
	} /* end if (stores) */

#if	CF_DEBUGS
	debugprintf("ussinfo/ussinfo_start: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (ussinfo_start) */


int ussinfo_finish(sip)
USSINFO		*sip ;
{
	int	rs = SR_OK ;
	int	rs1 ;

	if (sip->open.userinfo) {
	    sip->open.userinfo = false ;
	    rs1 = userinfo_finish(&sip->u) ;
	    if (rs >= 0) rs = rs1 ;
	}

	if (sip->open.ids) {
	    sip->open.ids = false ;
	    rs1 = ids_load(&sip->id) ;
	    if (rs >= 0) rs = rs1 ;
	}

	if (sip->a.node != nullptr) {
	    uc_free(sip->a.node) ;
	    if (rs >= 0) rs = rs1 ;
	    sip->a.node = nullptr ;
	}

	if (sip->a.svc != nullptr) {
	    uc_free(sip->a.svc) ;
	    if (rs >= 0) rs = rs1 ;
	    sip->a.svc = nullptr ;
	}

	if (sip->a.pr != nullptr) {
	    uc_free(sip->a.pr) ;
	    if (rs >= 0) rs = rs1 ;
	    sip->a.pr = nullptr ;
	}

	if (sip->a.portspec != nullptr) {
	    uc_free(sip->a.portspec) ;
	    if (rs >= 0) rs = rs1 ;
	    sip->a.portspec = nullptr ;
	}

	rs1 = vecstr_finish(&sip->aenvs) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = vecstr_finish(&sip->stores) ;
	if (rs >= 0) rs = rs1 ;

	return rs ;
}
/* end subroutine (ussinfo_finish) */


int ussinfo_procargs(sip)
USSINFO		*sip ;
{
	KEYOPT		akopts ;

	SYSDIALER_ARGS	*ap = sip->ap ;

	int	rs = SR_OK ;
	int	rs1 ;
	int	argc ;
	int	argr, argl, aol, akl, avl, kwi ;
	int	ai, ai_max, ai_pos ;
	int	pan = 0 ;
	int	f_optminus, f_optplus, f_optequal ;
	int	f_doubledash = false ;

	cchar	**argv ;
	cchar	*argval = nullptr ;
	cchar	*argp, *aop, *akp, *avp ;


#if	CF_DEBUGS
	debugprintf("ussinfo/ussinfo_procargs: arguments\n") ;
#endif

	argv = (cchar **) ap->argv ;

	for (argc = 0 ; argv[argc] != nullptr ; argc += 1) ;

#if	CF_DEBUGS
	{
	    debugprintf("ussinfo/ussinfo_procargs: argc=%u\n",argc) ;
	    for (ai = 0 ; argv[ai] != nullptr ; ai += 1)
	        debugprintf("ussinfo/ussinfo_procargs: argv[%u]=%s\n",
		ai,argv[ai]) ;
	}
#endif /* CF_DEBUGS */

	sip->argc = argc ;
	sip->argv = argv ;
	rs = keyopt_start(&akopts) ;
	if (rs < 0) goto badkopts ;

/* process program arguments */

	ai_max = 0 ;
	ai_pos = 0 ;
	argr = argc ;
	for (ai = 0 ; (ai < argc) && (argv[ai] != nullptr) ; ai += 1) {
	    if (rs < 0) break ;
	    argr -= 1 ;
	    if (ai == 0) continue ;

	    argp = argv[ai] ;
	    argl = strlen(argp) ;

	    f_optminus = (*argp == '-') ;
	    f_optplus = (*argp == '+') ;
	    if ((argl > 1) && (f_optminus || f_optplus)) {
		const int	ach = MKCHAR(argp[1]) ;

	        if (isdigitlatin(ach)) {

	            argval = (argp + 1) ;

	        } else if ((argl == 2) && (ach == '-')) {

	            f_doubledash = true ;
	            ai += 1 ;
	            argr -= 1 ;

	        } else {
		    int	v ;

#if	CF_DEBUGS
	            debugprintf("ussinfo/ussinfo_procargs: option? ao=>%r<\n",
			argp,argl) ;
#endif

	            aop = argp + 1 ;
	            akp = aop ;
	            aol = argl - 1 ;
	            f_optequal = false ;
	            if ((avp = strchr(aop,'=')) != nullptr) {
	                f_optequal = true ;
	                akl = avp - aop ;
	                avp += 1 ;
	                avl = aop + argl - 1 - avp ;
	                aol = akl ;
	            } else {
	                avp = nullptr ;
	                avl = 0 ;
	                akl = aol ;
	            }

#if	CF_DEBUGS
	            debugprintf("ussinfo/ussinfo_procargs: k=>%r<\n",akp,akl) ;
#endif

	            if ((kwi = matostr(argopts,2,akp,akl)) >= 0) {

	                switch (kwi) {

/* program-root */
	                case argopt_root:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            sip->pr = avp ;
	                    } else {
	                        if (argr <= 0) {
	                            rs = SR_INVALID ;
	                            break ;
	                        }
	                        argp = argv[++ai] ;
	                        argr -= 1 ;
	                        argl = strlen(argp) ;
	                        if (argl)
	                            sip->pr = argp ;
	                    }
	                    break ;

/* program-root-name */
	                case argopt_rn:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            sip->prn = avp ;
	                    } else {
	                        if (argr <= 0) {
	                            rs = SR_INVALID ;
	                            break ;
	                        }
	                        argp = argv[++ai] ;
	                        argr -= 1 ;
	                        argl = strlen(argp) ;
	                        if (argl)
	                            sip->prn = argp ;
	                    }
	                    break ;

/* search-name root */
	                case argopt_sn:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            sip->searchname = avp ;
	                    } else {
	                        if (argr <= 0) {
	                            rs = SR_INVALID ;
	                            break ;
	                        }
	                        argp = argv[++ai] ;
	                        argr -= 1 ;
	                        argl = strlen(argp) ;
	                        if (argl)
	                            sip->searchname = argp ;
	                    }
	                    break ;

/* logfile */
	                case argopt_lf:
			    sip->fl.log = true ;
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            sip->lfname = avp ;
	                    } else {
	                        if (argr <= 0) {
	                            rs = SR_INVALID ;
	                            break ;
	                        }
	                        argp = argv[++ai] ;
	                        argr -= 1 ;
	                        argl = strlen(argp) ;
	                        if (argl)
	                            sip->lfname = argp ;
	                    }
	                    break ;

/* path-vars file */
	                case argopt_pvf:
	                case argopt_pf:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            sip->pvfname = avp ;
	                    } else {
	                        if (argr <= 0) {
	                            rs = SR_INVALID ;
	                            break ;
	                        }
	                        argp = argv[++ai] ;
	                        argr -= 1 ;
	                        argl = strlen(argp) ;
	                        if (argl)
	                            sip->pvfname = argp ;
	                    }
	                    break ;

	                case argopt_df:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            sip->dfname = avp ;
	                    } else {
	                        if (argr <= 0) {
	                            rs = SR_INVALID ;
	                            break ;
	                        }
	                        argp = argv[++ai] ;
	                        argr -= 1 ;
	                        argl = strlen(argp) ;
	                        if (argl)
	                            sip->dfname = avp ;
	                    }
	                    break ;

	                case argopt_xf:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            sip->xfname = avp ;
	                    } else {
	                        if (argr <= 0) {
	                            rs = SR_INVALID ;
	                            break ;
	                        }
	                        argp = argv[++ai] ;
	                        argr -= 1 ;
	                        argl = strlen(argp) ;
	                        if (argl)
	                            sip->xfname = argp ;
	                    }
			    break ;

	                case argopt_ef:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            sip->efname = avp ;
	                    } else {
	                        if (argr <= 0) {
	                            rs = SR_INVALID ;
	                            break ;
	                        }
	                        argp = argv[++ai] ;
	                        argr -= 1 ;
	                        argl = strlen(argp) ;
	                        if (argl)
	                            sip->efname = argp ;
	                    }
	                    break ;

	                } /* end switch (key words) */

	            } else {

	                while (akl--) {

	                    switch ((int) *akp) {

/* address-family */
	                    case 'f':
	                        if (argr <= 0) {
	                            rs = SR_INVALID ;
	                            break ;
	                        }
	                        argp = argv[++ai] ;
	                        argr -= 1 ;
	                        argl = strlen(argp) ;
	                        if (argl)
	                            sip->afspec = argp ;
	                        break ;

	                    case 'i':
	                        sip->fl.ignore = true ;
	                        break ;

/* options */
	                    case 'o':
	                        if (argr <= 0) {
	                            rs = SR_INVALID ;
	                            break ;
	                        }
	                        argp = argv[++ai] ;
	                        argr -= 1 ;
	                        argl = strlen(argp) ;
	                        if (argl)
	                            rs = keyopt_loads(&akopts,argp,argl) ;
	                        break ;

/* service */
	                    case 's':
	                        if (argr <= 0) {
	                            rs = SR_INVALID ;
	                            break ;
	                        }
	                        argp = argv[++ai] ;
	                        argr -= 1 ;
	                        argl = strlen(argp) ;
	                        if (argl)
	                            sip->svcname = argp ;
	                        break ;
/* timeout */
	                    case 't':
	                        if (argr <= 0) {
	                            rs = SR_INVALID ;
	                            break ;
	                        }
	                        argp = argv[++ai] ;
	                        argr -= 1 ;
	                        argl = strlen(argp) ;
	                        if (argl) {
	                            rs = cfdecti(argp,argl,&v) ;
				    sip->to = v ;
				}
	                        break ;

/* eXported environment */
	                    case 'x':
	                        if (argr <= 0) {
	                            rs = SR_INVALID ;
	                            break ;
	                        }
	                        argp = argv[++ai] ;
	                        argr -= 1 ;
	                        argl = strlen(argp) ;
	                        if (argl)
	                            rs = vecstr_envadds(&sip->aenvs,argp,argl) ;
	                        break ;

	                    default:
	                        rs = SR_INVALID ;
				break ;

	                    } /* end switch */
	                    akp += 1 ;

	                    if (rs < 0) break ;
	                } /* end while */

	            } /* end if (individual option key letters) */

	        } /* end if (digits as argument or not) */

	    } else {

		switch (pan) {
		case 0:
	            sip->portspec = (cchar *) argp ;
		    break ;
		case 1:
	            sip->svcname = (cchar *) argp ;
		    break ;
		} /* end switch */
		pan += 1 ;

	    } /* end if (key letter-word or positional) */

	    ai_pos = ai ;

	} /* end while (all command line argument processing) */

	sip->argi = (argc > 0) ? (ai + 1) : 0 ;

#if	CF_DEBUGS
	debugprintf("ussinfo/ussinfo_procargs: portspec=%s\n",sip->portspec) ;
	debugprintf("ussinfo/ussinfo_procargs: argi=%u\n",sip->argi) ;
#endif

	if (rs >= 0)
	    rs = ussinfo_procopts(sip,&akopts) ;

/* process any address-family specification */

#if	CF_DEBUGS
	debugprintf("ussinfo/ussinfo_procargs: af=%d afspec=%s\n",
		sip->af, sip->afspec) ;
#endif

	if ((rs >= 0) && (sip->af < 0) && 
		(sip->afspec != nullptr) && (sip->afspec[0] != '\0')) {

#if	CF_DEBUGS
	debugprintf("ussinfo/ussinfo_procargs: afspec=%s\n",sip->afspec) ;
#endif

	    rs1 = getaf(sip->afspec,-1) ;
	    if (rs1 >= 0)
		sip->af = rs1 ;

#if	CF_DEBUGS
	debugprintf("ussinfo/ussinfo_procargs: getaf() rs=%d\n",rs1) ;
#endif

	} /* end if */

/* done and out */
done:
	keyopt_finish(&akopts) ;

badkopts:

#if	CF_DEBUGS
	debugprintf("ussinfo/ussinfo_procargs: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (ussinfo_procargs) */


int ussinfo_procopts(sip,kop)
USSINFO		*sip ;
KEYOPT		*kop ;
{
	KEYOPT_CUR	kcur ;
	int		rs ;
	int		c = 0 ;

	if ((rs = keyopt_curbegin(kop,&kcur)) >= 0) {
	    int		oi ;
	    int		kl, vl ;
	    cchar	*kp, *vp ;

	while ((kl = keyopt_enumkeys(kop,&kcur,&kp)) >= 0) {

/* get the first value for this key */

	    vl = keyopt_fetch(kop,kp,nullptr,&vp) ;

/* do we support this option? */

	    if ((oi = matostr(procopts,2,kp,kl)) >= 0) {

	        switch (oi) {

	        case procopt_log:
	                sip->fl.log = true ;
	                if ((vl > 0) && ((rs = optbool(vp,vl)) >= 0)) {
	                    sip->fl.log = (rs > 0) ;
			}
	            break ;

	        } /* end switch */

	        c += 1 ;
	    } /* end if (valid option) */

	        if (rs < 0) break ;
	    } /* end while (looping through key options) */

	    keyopt_curend(kop,&kcur) ;
	} /* end if (keyopt-cur) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (ussinfo_procopts) */


int ussinfo_defaults(sip)
USSINFO		*sip ;
{
	SYSDIALER_ARGS	*ap ;

	int	rs = SR_OK ;
	int	rs1 ;

	cchar	*vp ;


	ap = sip->ap ;

#if	CF_DEBUGS
	debugprintf("ussinfo/ussinfo_defaults: ent af=%d\n",sip->af) ;
#endif

/* program-root */

	if ((sip->pr == nullptr) && (sip->prn != nullptr) && (sip->prn[0] != '\0')) {
	    char	domainname[MAXHOSTNAMELEN + 1] ;
	    char	pr[MAXPATHLEN + 1] ;

	    rs1 = getnodedomain(nullptr,domainname) ;

	    if (rs1 >= 0)
	        rs1 = mkpr(pr,MAXPATHLEN,sip->prn,domainname) ;

	    if (rs1 >= 0) {

	        rs1 = ussinfo_dirok(sip,pr,rs1) ;

	        if (rs1 > 0) {
	            rs = uc_mallocstrw(pr,rs1,&sip->a.pr) ;
	            if (rs > 0)
	                sip->pr = sip->a.pr ;
		}

	    } /* end if */

	} /* end if */

	if ((rs >= 0) && (sip->pr == nullptr)) {

	    if ((vp = getenv(USSINFO_VARPR)) != nullptr) {

	        rs1 = ussinfo_dirok(sip,vp,-1) ;
	        if (rs1 > 0)
	            sip->pr = vp ;

	    }

	} /* end if */

	if ((rs >= 0) && (sip->pr == nullptr)) {

	    vp = USSINFO_PR ;
	    rs1 = ussinfo_dirok(sip,vp,-1) ;
	    if (rs1 > 0)
	        sip->pr = vp ;

	} /* end if */

	if (sip->pr == nullptr)
	    sip->pr = ap->pr ;

/* log-file */

	if ((rs >= 0) && (sip->lfname == nullptr))
	    sip->lfname = USSINFO_LOGFNAME ;

#if	CF_DEBUGS
	debugprintf("ussinfo/ussinfo_defaults: pr=%s\n",sip->pr) ;
	debugprintf("ussinfo/ussinfo_defaults: logfname=%s\n",sip->lfname) ;
#endif

/* address family */

#if	CF_DEBUGS
	debugprintf("ussinfo/ussinfo_defaults: af=%d\n",sip->af) ;
#endif

#ifdef	COMMENT
	if (sip->af < 0)
	    sip->af = AF_UNIX ;
#endif

/* out of here */

	return rs ;
}
/* end subroutine (ussinfo_defaults) */

int ussinfo_addrparse(USSINFO *sip) noex {
	int		rs = SR_OK ;

	if ((sip->portspec != nullptr) && (sip->portspec[0] != '\0')) {

	    if (hasleadcolon(sip->portspec,-1)) {
		if (strncasecmp("unix:",sip->portspec,5) == 0) {
		    rs = ussinfo_addrparseunix(sip,1) ;
		} else {
		    rs = ussinfo_addrparseinet(sip) ;
		}
	    } else if (strchr(sip->portspec,'/') != nullptr) {
		rs = ussinfo_addrparseunix(sip,0) ;
	    } else {
		const int	af = sip->af ;
		if ((af == 0) || (af != AF_UNIX)) {
		    rs = ussinfo_addrparseinet(sip) ;
		} else {
		    rs = ussinfo_addrparseunix(sip,0) ;
		}
	    }

	} /* end if (needed) */

	return rs ;
}
/* end subroutine (ussinfo_addrparse) */


int ussinfo_logfile(sip)
USSINFO		*sip ;
{
	int	rs = SR_OK ;
	int	f = sip->init.log ;

	if (sip->fl.log && (! sip->init.log)) {
	    cchar	*lfname = sip->lfname ;
	    sip->init.log = true ;
	    f = true ;

	    if (lfname[0] != '/') {
		cchar	*logdname = USSINFO_LOGDNAME ;
	        char		tbuf[MAXPATHLEN + 1] ;
	        if ((rs = mkpath3(tbuf,sip->pr,logdname,lfname)) >= 0) {
		    cchar	**vpp = &sip->lfname ;
		    rs = ussinfo_setentry(sip,vpp,tbuf,rs) ;
		}
	    }

#if	CF_DEBUGS
	debugprintf("ussinfo/ussinfo_logfile: lnp=%s\n",lnp) ;
#endif

	    if (rs >= 0) {
	        rs = ussinfo_userinfo(sip) ;
	    } /* end if (ok) */

	} /* end if (needed initialziation) */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (ussinfo_logfile) */


/* private subroutines */


local int ussinfo_setentry(sip,epp,sp,sl)
USSINFO		*sip ;
cchar	**epp ;
cchar	sp[] ;
int		sl ;
{
	int	rs = SR_OK ;
	int	len = 0 ;


	if (sip == nullptr) return SR_FAULT ;
	if (epp == nullptr) return SR_FAULT ;

	if (! sip->open.stores) {
	    rs = vecstr_start(&sip->stores,4,0) ;
	    sip->open.stores = (rs >= 0) ;
	}

	if (rs >= 0) {
	    int	oi = -1 ;

	    if (*epp != nullptr) oi = vecstr_findaddr(&sip->stores,*epp) ;

	    if (sp != nullptr) { 
		len = strnlen(sp,sl) ;
	        if ((rs = vecstr_add(&sip->stores,sp,len)) >= 0) {
	            rs = vecstr_get(&sip->stores,rs,epp) ;
	        } /* end if (added new entry) */
	    } else
		*epp = nullptr ;

	    if ((rs >= 0) && (oi >= 0))
	        vecstr_del(&sip->stores,oi) ;

	} /* end if */

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (ussinfo_setentry) */


local int ussinfo_userinfo(sip)
USSINFO		*sip ;
{
	int	rs = SR_OK ;

	if (! sip->open.userinfo) {
	    USERINFO	*uip = &sip->u ;
	    if ((rs = userinfo_start(uip,nullptr)) >= 0) {
	        sip->open.userinfo = true ;
	        sip->umachine = uip->machine ;
	        sip->usysname = uip->sysname ;
	        sip->urelease = uip->release ;
	        sip->uversion = uip->version ;
	        sip->nodename = uip->nodename ;
	        sip->username = uip->username ;
	        sip->homedname = uip->homedname ;
	        sip->shell = uip->shell ;
	        sip->organization = uip->organization ;
	        sip->gecosname = uip->gecosname ;
	        sip->realname = uip->realname ;
	        sip->name = uip->name ;
	        sip->domainname = uip->domainname ;
	        sip->tz = uip->tz ;
	        sip->logid = uip->logid ;
	        sip->uid = uip->uid ;
	        sip->gid = uip->gid ;
	    } /* end if (userinfo_start) */
	} /* end if (needed initialization) */

	return rs ;
}
/* end subroutine (ussinfo_userinfo) */


local int ussinfo_dirok(sip,d,dlen)
USSINFO		*sip ;
cchar	d[] ;
int		dlen ;
{
	ustat	sb ;

	NULSTR	ss ;

	int	rs = SR_OK ;
	int	rs1 ;
	int	f = false ;

	cchar	*dnp ;


	if (! sip->open.ids) {
	    sip->open.ids = true ;
	    rs = ids_load(&sip->id) ;
	}

	if (rs >= 0) {
	    if ((rs = nulstr_start(&ss,d,dlen,&dnp)) >= 0) {

	rs1 = u_stat(dnp,&sb) ;

	if ((rs1 >= 0) && S_ISDIR(sb.st_mode)) {

	    rs1 = permid(&sip->id,&sb,(R_OK | X_OK)) ;
	    f = (rs1 >= 0) ;

	} /* end if */

	nulstr_finish(&ss) ;
	} /* end if */
	} /* end if (ok) */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (ussinfo_dirok) */


local int ussinfo_addrparseunix(sip,f)
USSINFO		*sip ;
int		f ;
{
	int	rs = SR_OK ;
	int	pslen = -1 ;

	cchar	*ps = sip->portspec ;

	char	tmpfname[MAXPATHLEN + 1] ;


	if (f)
		ps += 5 ;

	if (ps[0] != '/') {
		f = true ;
		pslen = mkpath2(tmpfname,sip->pr,ps) ;
		ps = tmpfname ;
	}

	if (f) {
	    rs = uc_mallocstrw(ps,pslen,&sip->a.portspec) ;
	    if (rs >= 0)
	        sip->portspec = sip->a.portspec ;
	}


	if (rs >= 0) {
	    sip->af = AF_UNIX ;
	}

	return rs ;
}
/* end subroutine (ussinfo_addrparseunix) */

local int ussinfo_addrparseinet(USSINFO *sip) noex {
	INETADDRPARSE	a ;
	int	rs = SR_OK ;
	int	rs1 ;

	if ((rs = inetaddrparse_load(&a,sip->portspec,-1)) >= 0) {

	if ((rs >= 0) && (a.af.sp != nullptr) && a.af.sl) {
	    cchar	**vpp = &sip->afspec ;

	    if ((rs = ussinfo_setentry(sip,vpp,a.af.sp,a.af.sl)) >= 0) {
		if (strcasecmp(sip->afspec,"inet") == 0) {
		    sip->af = AF_UNSPEC ;
		} else {
		    rs1 = getaf(sip->afspec,-1) ;
		    if (rs1 >= 0)
		        sip->af = rs1 ;
		}
	    }

	} /* end if */

	if ((rs >= 0) && (a.host.sp != nullptr) && a.host.sl) {
	    cchar	**vpp = &sip->hostname ;
	    rs = ussinfo_setentry(sip,vpp,a.host.sp,a.host.sl) ;
	}

	if ((rs >= 0) && (a.port.sp != nullptr) && a.port.sl) {
	    cchar	**vpp = &sip->portspec ;
	    rs = ussinfo_setentry(sip,vpp,a.port.sp,a.port.sl) ;
	}

	if (sip->af < 0)
	    sip->af = AF_INET4 ;

	} /* end if (inetaddrparse_load) */

	return rs ;
}
/* end subroutine (ussinfo_addrparseinet) */


