/* prqotd_config SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* open a channel (file-descriptor) to the quote-of-the-day (QOTD) */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* compile-time debugging */

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
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<ucgetpid.h>
#include	<estrings.h>
#include	<mkfnamesuf.h>
#include	<vecstr.h>
#include	<vecpstr.h>
#include	<ascii.h>
#include	<paramfile.h>
#include	<expcook.h>
#include	<permx.h>		/* |permsched(3uc)| */
#include	<cfdecmf.h>
#include	<cfdect.h>
#include	<ctdec.h>
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

#ifndef	LOGCNAME
#define	LOGCNAME	"log"
#endif

#define	CF		prqotd::config

#ifndef	SUB
#define	SUB		subinfo
#endif

#define	PF		paramfile
#define	PF_CUR		paramfile_cur

#define	EC		expcook
#define	EC_CUR		expcook_cur

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* compile-time debugging */
#endif

/* imported namespaces */

using prqotd::config ;			/* type */
using prqotd::subinfo ;			/* type */
using prqotd::subinfo_setfname ;        /* subroutine */
using prqotd::subinfo_setentry ;        /* subroutine */
using prqotd::subinfo_spooldir ;        /* subroutine */
using prqotd::subinfo_hostname ;        /* subroutine */
using prqotd::subinfo_source ;          /* subroutine */
using prqotd::var ;			/* variable */
using prqotd::config_magicval ;		/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local int config_ctor(config *op,Args ... args) noex {
    	config		*hop = op ;
	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    memclear(hop) ; /* <- dangerous */
	    rs = SR_NOMEM ;
	    if ((op->pfp = new(nt) paramfile) != np) ylikely {
	        if ((op->ecp = new(nt) expcook) != np) ylikely {
		    rs = SR_OK ;
		} /* end if (new-expcook) */
		if (rs < 0) {
		    delete op->pfp ;
		    op->pfp = nullptr ;
		} /* end if (error) */
	    } /* end if (new-paramfile) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (config_ctor) */

local int config_dtor(config *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->ecp) ylikely {
		delete op->ecp ;
		op->ecp = nullptr ;
	    }
	    if (op->pfp) ylikely {
		delete op->pfp ;
		op->pfp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (config_dtor) */

template<typename ... Args>
local inline int config_magic(config *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == config_magicval) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (config_magic) */

local int	config_findfile		(config *,char *,cchar *) noex ;
local int	config_cookbegin	(config *) noex ;
local int	config_cookend		(config *) noex ;
local int	config_reader		(config *,char *,int) noex ;


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

cbool			f_debug = CF_DEBUG ;


/* exported variables */


/* exported subroutines */

namespace prqotd {
    int config_start(CF *csp,SUB *sip,cchar *cfname) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = config_ctor(csp,sip,cfname)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (cfname[0]) ylikely {
	        if (char *tbuf ; (rs = lm_mp(&tbuf)) >= 0) ylikely {
		    PF	*pfp = csp->pfp ;
	            csp->sip = sip ;
	            if ((rs = config_findfile(csp,tbuf,cfname)) >= 0) ylikely {
	                con mainv envv = var.envv ;
	                if (rs > 0) cfname = tbuf ;
	                if ((rs = pfp->open(envv,cfname)) >= 0) ylikely {
	                    if ((rs = config_cookbegin(csp)) >= 0) {
	                        csp->f_p = true ;
	                    }
	                    if (rs < 0) {
	                        pfp->close() ;
		            } /* end if (error) */
	                } else if (isNotPresent(rs)) {
	                    rs = SR_OK ;
	                }
	            } else if (isNotPresent(rs)) {
	                rs = SR_OK ;
	            }
	            if (rs >= 0) {
	                csp->magval = config_magicval ;
	            }
		    rs1 = lm_free(tbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
		if (rs < 0) nlikely {
		    csp->magval = 0 ;
		} /* end if (error) */
	    } /* end if (valid) */
	    if (rs < 0) nlikely {
		config_dtor(csp) ;
	    } /* end if (error) */
	} /* end if (config_ctor) */
	return rs ;
    } /* end subroutine (config_start) */
    int config_finish(CF *csp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = config_magic(csp)) >= 0) ylikely {
	    if (csp->f_p) {
	        if (csp->f_cooks) {
	            rs1 = config_cookend(csp) ;
	            if (rs >= 0) rs = rs1 ;
	        }
		{
	            rs1 = paramfile_close(csp->pfp) ;
	            if (rs >= 0) rs = rs1 ;
	            csp->f_p = false ;
		}
	    } /* end if */
	    {
		rs1 = config_dtor(csp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (config_magic) */
	return rs ;
    } /* end subroutine (config_finish) */
    int config_read(CF *csp) noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	if ((rs = config_magic(csp)) >= 0) ylikely {
	    SUB		*sip = csp->sip ;
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
	return (rs >= 0) ? rv : rs ;
    } /* end subroutine (config_read) */
    int config_check(CF *csp) noex {
	int		rs ;
	int		rv = 0 ; /* return-value */
	if ((rs = config_magic(csp)) >= 0) ylikely {
	    SUB		*sip = csp->sip ;
	    if (csp->f_p) {
	        custime dt = sip->dt ;
	        if ((rs = paramfile_check(csp->pfp,dt)) > 0) {
	            rs = config_read(csp) ;
		    rv = rs ;
		}
	    }
	} /* end if (config_magic) */
	return (rs >= 0) ? rv : rs ;
    } /* end subroutine (config_check) */
} /* end namespace (prqotd) */


/* private subroutines */

local int config_findfile(CF *csp,char *tbuf,cchar *cfname) noex {
	int		rs ;
	int		rs1 ;
	int		pl = 0 ; /* return-value */
	if ((rs = config_magic(csp)) >= 0) ylikely {
	    SUB		*sip = csp->sip ;
	    tbuf[0] = '\0' ;
	    if (vecstr sv ; (rs = vecstr_start(&sv,6,0)) >= 0) ylikely {
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
	} /* end if (config_magic) */
	return (rs >= 0) ? pl : rs ;
} /* end subroutine (config_findfile) */

local int config_cookbegin(CF *csp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = config_magic(csp)) >= 0) ylikely {
	    SUB		*sip = csp->sip ;
	    if (char *hbuf ; (rs = lm_mp(&hbuf)) >= 0) ylikely {
		EC	*ecp = csp->ecp ;
	        int	hlen = rs ;
	        if ((rs = ecp->start) >= 0) ylikely {
	            cchar	*ks = "PSNDHRU" ;
		    cchar	*vp = nullptr ;
	            char	kbuf[2] = {} ;
	            int		vl = -1 ;
	            for (int i = 0 ; (rs >= 0) && ks[i] ; i += 1) {
	                cint kch = MKCHAR(ks[i]) ;
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
	                    } /* end block */
	                    break ;
	                case 'R':
	                    vp = sip->pr ;
	                    break ;
	                case 'U':
	                    vp = sip->un ;
	                    break ;
	                } /* end switch */
	                if ((rs >= 0) && vp) {
	                    kbuf[0] = char(kch) ;
	                    rs = ecp->add(kbuf,vp,vl) ;
	                }
	            } /* end for */
	            if (rs >= 0) ylikely {
	                if ((vl = sfbasename(sip->pr,-1,&vp)) > 0) {
	                    rs = ecp->add("RN",vp,vl) ;
	                }
	            } /* end if (ok) */
	            if (rs >= 0) ylikely {
	                if ((rs = ctdeci(hbuf,hlen,sip->mjd)) >= 0) {
	                    rs = ecp->add("MJD",hbuf,rs) ;
	                }
	            } /* end if (ok) */
	            if (rs >= 0) ylikely {
	                csp->f_cooks = true ;
	            } else {
	                ecp->finish() ;
	            } /* end if (error) */
	        } /* end if (expcook_start) */
	        rs1 = lm_free(hbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (config_magic) */
	return rs ;
} /* end subroutine (config_cookbegin) */

local int config_cookend(CF *csp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = config_magic(csp)) >= 0) ylikely {
	    if (csp->f_cooks) {
	        csp->f_cooks = false ;
	        rs1 = expcook_finish(csp->ecp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if */
	} /* end if (config_magic) */
	return rs ;
} /* end subroutine (config_cookend) */

local int config_readers(CF *,char *,int,int) noex ;

local int config_reader(CF *csp,char *ebuf,int elen) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	DEBUGPRINTF("ent f_active=%u\n",csp->f_p) ;
	if (csp->f_p) {
	    EC		*ecp = csp->ecp ;
	    PF		*pfp = csp->pfp ;
	    cint	sz = (var.vbuflen + 1) ;
	    if (char *vbuf ; (rs = lm_mall(sz,&vbuf)) >= 0) ylikely {
	        cint	vlen = var.vbuflen ;
	        for (int i = 0 ; cparams[i] ; i += 1) {
		    cchar	*cparam = cparams[i] ;
	            if (PF_CUR cur ; (rs = pfp->curbegin(&cur)) >= 0) {
	                while (rs >= 0) {
	                    cint vl = pfp->fetch(cparam,&cur,vbuf,vlen) ;
	    		    int	el = 0 ;
	                    if (vl == SR_NOTFOUND) break ;
	                    rs = vl ;
	                    if (rs < 0) break ;
	                    ebuf[0] = '\0' ;
	                    if (vl > 0) {
	                        rs = ecp->exp(0,ebuf,elen,vbuf,vl) ;
			        el = rs ;
	                        if (el >= 0) ebuf[el] = '\0' ;
	                    }
	                    if ((rs >= 0) && (el > 0)) {
			        rs = config_readers(csp,ebuf,el,i) ;
			        c += 1 ;
			    }
	                } /* end while (fetching) */
	                rs1 = paramfile_curend(pfp,&cur) ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (cursor) */
	            if (rs < 0) break ;
	        } /* end for (parameters) */
	        rs1 = lm_free(vbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (active) */
	DEBUGPRINTF("ret rs=%d c=%d\n",rs,c) ;
	return (rs >= 0) ? c : rs ;
} /* end subroutine (config_reader) */

local int config_readers(CF *csp,char *ebuf,int el,int i) noex {
	SUB		*sip = csp->sip ;
	cnullptr	np{} ;
    	int		rs ;
	int		rs1 ;
	DEBUGPRINTF("ent i=%d el=%d\n",i,el) ;
	if (char *tbuf ; (rs = lm_mp(&tbuf)) >= 0) ylikely {
	    cchar	*sn = sip->sn ;
            switch (int v ; i) {
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
		    int ml ;
                    cchar *lfn = sip->lfname ;
                    cchar *tfn = tbuf ;
                    sip->finval.lfname = true ;
                    sip->have.lfname = true ;
                    ml = subinfo_setfname(sip,tbuf,ebuf,el,true,
                        LOGCNAME,sn,"") ;
                    if ((lfn == np) || (strcmp(lfn,tfn) != 0)) {
                        cchar   **vpp = &sip->lfname ;
                        sip->changed.lfname = true ;
                        rs = subinfo_setentry(sip,vpp,tbuf,ml) ;
                    }
                } /* end if (needed) */
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
	    rs1 = lm_free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (config_readers) */


