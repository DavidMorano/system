/* prqotd_config SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* open a channel (file-descriptor) to the quote-of-the-day (QOTD) */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* compile-time debugging */
#define	CF_OPENDEF	0		/* ? */
#define	CF_SOURCES	1		/* use sources */
#define	CF_CONFIGCHECK	0		/* |config_check()| */

/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	prqotd_config

	Description:
	This subroutine sort of forms the back-end maintenance end
	of the QOTD mechansim.

	Synopsis:
	int prqotd_config(cchar *pr,int mjd,int of,int to) noex

	Arguments:
	pr		program-root
	mjd		modified-julian-day
	of		open-flags
	to		time-out

	Returns:
	>=0		FD of QOTD
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<tzfile.h>		/* for TM_YEAR_BASE */
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<ucgetpid.h>
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
#include	<permx.h>
#include	<isnot.h>
#include	<ischarx.h>
#include	<localmisc.h>		/* |DIGBUFLEN| */
#include	<libdebug.h>		/* LIBDEBUG */

#include	"prqotd.h"
#include	"prqotd_util.hh"
#include	"prqotd_subinfo.hh"
#include	"prqotd_config.hh"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/uconstants.ccm"

import libutil ;			/* |lenstr(3u)| */
import uconstants ;			/* |varname(3u)| */

/* local defines */

#ifndef	VBUFLEN
#define	VBUFLEN		(2 * MAXPATHLEN)
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

#define	CF		prqotd::config

#ifndef	SI
#define	SI		subinfo
#endif

#define	PF		paramfile
#define	PF_CUR		paramfile_cur


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr cpcchar	csched[] = {
	"%p/%e/%n/%n.%f",
	"%p/%e/%n/%f",
	"%p/%e/%n.%f",
	"%p/%n.%f",
	nullptr
} ; /* end array (csched) */

enum cparams {
	cparam_spooldir,
	cparam_logsize,
	cparam_logfile,
	cparam_hostname,
	cparam_svcname,
	cparam_to,
	cparam_source,
	cparam_overlast
} ; /* end enum (cparams) */

constexpr cpcchar	cparams[] = {
	"spooldir",
	"logsize",
	"logfile",
	"hostname",
	"svcname",
	"to",
	"source",
	nullptr
} ; /* end array (cparams) */


/* exported variables */


/* exported subroutines */

int config_start(CF *csp,SI *sip,cchar *cfname) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (csp && cfname) {
	    memclear(csp) ;
	    rs = SR_INVALID ;
	    if (cfname[0]) {
	        if (char *tbuf ; (rs = lm_mp(&tbuf)) >= 0) {
	            csp->sip = sip ;
	            if ((rs = config_findfile(csp,tbu,cfname)) >= 0) {
	                cchar	**envv = var.envv ;
	                if (rs > 0) cfname = tbuf ;
	                if ((rs = paramfile_open(&csp->p,envv,cfname)) >= 0) {
	                    if ((rs = config_cookbegin(csp)) >= 0) {
	                        csp->f_p = true ;
	                    }
	                    if (rs < 0) {
	                        paramfile_close(&csp->p) ;
		            }
	                } else if (isNotPresent(rs)) {
	                    rs = SR_OK ;
	                }
	            } else if (isNotPresent(rs)) {
	                rs = SR_OK ;
	            }
	            if (rs >= 0) {
	                csp->magval = MAINTQOTD_CONFMAGIC ;
	            }
		    rs1 = lm_free(tbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
		if (rs < 0) {
		    csp->magval = 0 ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (config_start) */

int config_finish(CF *csp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (csp) {
	    rs = SR_NOTOPEN ;
	    if (csp->magval == MAINTQOTD_CONFMAGIC) {
		rs = SR_OK ;
	        if (csp->f_p) {
	            if (csp->f_cooks) {
	                rs1 = config_cookend(csp) ;
	                if (rs >= 0) rs = rs1 ;
	            }
	            rs1 = paramfile_close(&csp->p) ;
	            if (rs >= 0) rs = rs1 ;
	            csp->f_p = false ;
	        } /* end if */
	    } /* end if (config_magic) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (config_finish) */

int config_findfile(CF *csp,char *tbuf,cchar *cfname) noex {
	MAINTQOTD	*sip = csp->sip ;
	int		rs ;
	int		rs1 ;
	int		pl = 0 ; /* return-value */
	tbuf[0] = '\0' ;
	if (vecstr sv ; (rs = vecstr_start(&sv,6,0)) >= 0) {
	    cint	tlen = var.maxpathlen ;
	    if (rs >= 0) rs = vecstr_envset(&sv,"p",sip->pr,-1) ;
	    if (rs >= 0) rs = vecstr_envset(&sv,"e","etc",-1) ;
	    if (rs >= 0) rs = vecstr_envset(&sv,"n",sip->sn,-1) ;
	    if (rs >= 0) {
	        rs = permsched(csched,&sv,tbuf,tlen,cfname,R_OK) ;
	        pl = rs ;
	    }
	    rs1 = vecstr_finish(&sv) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (finding file) */
	return (rs >= 0) ? pl : rs ;
}
/* end subroutine (config_findfile) */

int config_cookbegin(CF *csp) noex {
	MAINTQOTD	*sip = csp->sip ;
	int		rs ;
	int		rs1 ;
	if (char *hbuf ; (rs = lm_mp(&hbuf)) >= 0) {
	    int	hlen = rs ;
	    if ((rs = expcook_start(&csp->cooks)) >= 0) {
	        cchar	*ks = "PSNDHRU" ;
	        char	kbuf[2] ;
	        kbuf[1] = '\0' ;
	        for (int i = 0 ; (rs >= 0) && (ks[i] != '\0') ; i += 1) {
		    cchar *vp = nullptr ;
	            int    vl = -1 ;
	            switch (cint kch = MKCHAR(ks[i]) ; kch) {
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
	            if ((rs >= 0) && vp) {
	                kbuf[0] = kch ;
	                rs = expcook_add(&csp->cooks,kbuf,vp,vl) ;
	            }
	        } /* end for */
	        if (rs >= 0) {
	            if ((vl = sfbasename(sip->pr,-1,&vp)) > 0) {
	                rs = expcook_add(&csp->cooks,"RN",vp,vl) ;
	            }
	        } /* end if (ok) */
	        if (rs >= 0) {
	            if ((rs = ctdeci(hbuf,hlen,sip->mjd)) >= 0) {
	                rs = expcook_add(&csp->cooks,"MJD",hbuf,rs) ;
	            }
	        } /* end if (ok) */
	        if (rs >= 0) {
	            csp->f_cooks = true ;
	        } else {
	            expcook_finish(&csp->cooks) ;
	        }
	    } /* end if (expcook_start) */
	    rs1 = lm_free(hbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (config_cookbegin) */

int config_cookend(CF *csp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (csp->f_cooks) {
	    csp->f_cooks = false ;
	    rs1 = expcook_finish(&csp->cooks) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if */
	return rs ;
}
/* end subroutine (config_cookend) */

int config_check(CF *csp) noex {
	int		rs = SR_FAULT ;
	int		rv = 0 ; /* return-value */
	if (csp) {
	    MAINTQOTD	*sip = csp->sip ;
	    rs = SR_NOTOPEN ;
	    if (csp->magval == MAINTQOTD_CONFMAGIC) {
		rs = SR_OK ;
	        if (csp->f_p) {
	            custime dt = sip->dt ;
	            if ((rs = paramfile_check(&csp->p,dt)) > 0) {
	                rs = config_read(csp) ;
			rv = rs ;
		    }
	        }
	    } /* end if (config_magic) */
	} /* end if (non-null) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (config_check) */

int config_read(CF *csp) noex {
	MAINTQOTD	*sip = csp->sip ;
	int		rs = SR_FAULT ;
	int		rv = 0 ; /* return-value */
	if (csp) {
	    MAINTQOTD	*sip = csp->sip ;
	    rs = SR_NOTOPEN ;
	    if (csp->magval == MAINTQOTD_CONFMAGIC) {
		rs = SR_FAULT ;
		if ((sip = csp->sip)) {
		    rs = SR_OK ;
	            if (csp->f_p) {
	                cint	elen = var.ebuflen ;
	                if (char *ebuf ; (rs = lm_mall((elen+1),&ebuf)) >= 0) {
			    {
		                rs = config_reader(csp,ebuf,elen) ;
			        rv = rs ;
			    }
		            rs1 = lm_free(ebuf) ;
			    if (rs >= 0) rs = rs1 ;
	                } /* end if (memory-allocation) */
	            } /* end if (avtive) */
	        } /* end if (valid) */
	    } /* end if (config_magic) */
	} /* end if (non-null) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (config_read) */

int config_reader(CF *csp,char *ebuf,int elen) noex {
	MAINTQOTD	*sip = csp->sip ;
	PF	*pfp = &csp->p ;
	PF_CUR	cur ;
	cint	vlen = var.vbuflen ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		i ;
	int		vl, el ;
	int		v ;
	int		ml ;
	int		c = 0 ;
	char		vbuf[VBUFLEN + 1] ;

#if	CF_DEBUG
	debugprintf("maintqotd/config_reader: ent f_active=%u\n",
		csp->f_p) ;
#endif
	if (sip == nullptr) return SR_FAULT ;

	if (csp->f_p) {
	    for (i = 0 ; cparams[i] != nullptr ; i += 1) {
		cchar	*cparam = cparams[i] ;

#if	CF_DEBUG
	        debugprintf("mqintqotd/config_read: cparam=%s\n",cparam) ;
#endif

	        if ((rs = paramfile_curbegin(pfp,&cur)) >= 0) {

	            while (rs >= 0) {
	                vl = paramfile_fetch(pfp,cparam,&cur,vbuf,vlen) ;
#if	CF_DEBUG
	                debugprintf("mqintqotd/config_read: "
			"paramfile_fetch() rs=%d\n",vl) ;
#endif
	                if (vl == SR_NOTFOUND) break ;
	                rs = vl ;
	                if (rs < 0) break ;

#if	CF_DEBUG
	                    debugprintf("mqintqotd/config_read: "
				"vbuf=>%r<\n",vbuf,vl) ;
#endif

	                ebuf[0] = '\0' ;
	                el = 0 ;
	                if (vl > 0) {
	                    el = expcook_exp(&csp->cooks,0,ebuf,elen,vbuf,vl) ;
	                    if (el >= 0) ebuf[el] = '\0' ;
	                }

#if	CF_DEBUG
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
	                        if (! sip->finval.lfname) {
	                            cchar *lfn = sip->lfname ;
	                            cchar	*tfn = tbuf ;
	                            sip->finval.lfname = true ;
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
	            rs1 = paramfile_curend(pfp,&cur) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (cursor) */
	        if (rs < 0) break ;
	    } /* end for (parameters) */
	} /* end if (active) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (config_reader) */


