/* proginfo_log1 MODULE (implementation[ ) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* handle program logging */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-09-01, David A­D­ Morano
	This subroutine was borrowed and modified from previous
	generic front-end 'main' subroutines!

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Module:
	proginfo_log

	Description:
	Manage the general process logging.

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<sys/param.h>		/* POSIX */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<nulstr.h>		/* LIBU */
#include	<new>			/* C++STD placement-new */
#include	<memory>		/* C++STD |destroy_at(3c++)| */
#include	<ucmem.h>		/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<fmtstr.h>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<isch.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<logfile.h>		/* LIBDAM */
#include	<userinfo.h>		/* LIBDAM */

#include	"proginfo.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

module proginfo.log ;

import libutil ;			/* |lenstr(3u)| */
import proginfo.base ;

/* local defines */

#ifndef	UI
#define	UI	userinfo
#endif


/* imported namespaces */

using std::destroy_at ;			/* subroutine */
using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int proglog_lfname	(PI *) noex ;
local int proglog_lfopen	(PI *,UI *) noex ;
local int proglog_lfobjbeg	(PI *) noex ;
local int proglog_lfobjend	(PI *) noex ;


/* local variables */

statcexpr bool		f_comment = false ;


/* exported variables */


/* exported subroutines */

int proglog_begin(PI *pip,userinfo *uip) noex {
	int		rs = SR_FAULT ;
	int		fopened = false ; /* return-value */
	if (pip && uip) ylikely {
	    rs = SR_OK ;
	    if (pip->pf.logprog) {
	        if ((rs = proglog_lfname(pip)) >= 0) ylikely {
	            if (pip->lfname) ylikely {
			if ((rs = proglog_lfobjbeg(pip)) >= 0) {
			    {
			        rs = proglog_lfopen(pip,uip) ;
			        fopened = (rs > 0) ;
			    }
			    if (rs < 0) {
				proglog_lfobjend(pip) ;
			    } /* end if (error) */
			} /* end if (proglog_lfobjbeg) */
	            } /* end if */
	        } /* end if (log-file) */
	    } /* end if (enabled) */
	} /* end if (non-null) */
	return (rs >= 0) ? fopened : rs ;
} /* end subroutine (proglog_begin) */

int proglog_end(PI *pip) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (pip) {
	    rs = SR_OK ;
	    if (pip->pfopen.logprog) {
	        if (logfile *lfp = resumelife<logfile>(pip->lfp)) {
		    {
	                pip->pfopen.logprog = false ;
	                rs1 = lfp->close ;
	                if (rs >= 0) rs = rs1 ;
		    } /* end */
		    {
			rs1 = proglog_lfobjend(pip) ;
	                if (rs >= 0) rs = rs1 ;
		    } /* end */
		} /* end if (non-null) */
	    } /* end if (log-open) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (proglog_end) */

int proglog_intro(PI *pip,userinfo *uip) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ; /* return-value */
	if (pip) {
	    rs = SR_OK ;
	    if (pip->pfopen.logprog && pip->pf.logprog) {
	        if (logfile *lfp = resumelife<logfile>(pip->lfp)) {
	            if (uip) {
		        custime	dt = pip->daytime ;
		        cchar	*pn = pip->progname ;
		        cchar	*ver = pip->version ;
	                rs = logfile_userinfo(lfp,uip,dt,pn,ver) ;
		        wlen += rs ;
	            } /* end if (non-null) */
	            if (rs >= 0) {
	                rs = lfp->printf("pr=%s\n",pip->pr) ;
		        wlen += rs ;
	            } /* end if (ok) */
		} /* end if (non-null) */
	    } /* end if (enabled and open) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (proglog_intro) */

int proglog_cksize(PI *pip) noex {
	int		rs = SR_FAULT ;
	if (pip) {
	    rs = SR_OK ;
	    if (pip->pfopen.logprog && (pip->logsize > 0)) {
	        if (logfile *lfp = resumelife<logfile>(pip->lfp)) {
	            rs = lfp->cksize(pip->logsize) ;
		}
	    } /* end if (log-open) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (proglog_chsize) */

int proglog_check(PI *pip) noex {
	int		rs = SR_FAULT ;
	if (pip) {
	    rs = SR_OK ;
	    if (pip->pfopen.logprog) {
	        if (logfile *lfp = resumelife<logfile>(pip->lfp)) {
	            rs = lfp->check(pip->daytime) ;
		}
	    } /* end if (log-open) */
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (proglog_check) */

int proglog_getid(PI *pip,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	if (pip && rbuf) {
	    rs = sncpy1(rbuf,rlen,pip->logid) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (proglog_getid) */

int proglog_setid(PI *pip,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (pip && sp) {
	    rs = SR_OK ;
	    if (pip->pfopen.logprog) {
	        if (logfile *lfp = resumelife<logfile>(pip->lfp)) {
	           ccharp logid ;
	           if (nulstr ls ; (rs = ls.start(sp,sl,&logid)) >= 0) {
		       {
	                   rs = lfp->setid(logid) ;
		           c = rs ;
		       }
		       rs1 = ls.finish ;
		       if (rs >= 0) rs = rs1 ;
	           } /* end if (nulstr) */
		} /* end if (non-null) */
	    } /* end if (log-open) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (proglog_setid) */

int proglog_print(PI *pip,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (pip && sp) {
	    rs = SR_OK ;
	    if (pip->pfopen.logprog) {
	        if (logfile *lfp = resumelife<logfile>(pip->lfp)) {
	            rs = lfp->print(sp,sl) ;
		    wlen += rs ;
		}
	    } /* end if (log-open) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (proglog_print) */

int proglog_vprintf(PI *pip,cchar *fmt,va_list ap) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		wlen = 0 ;
	if (pip && fmt) {
	    rs = SR_OK ;
	    if (pip->pfopen.logprog) {
	        if (char *fbuf ; (rs = mem.ml(&fbuf)) >= 0) {
		    cint	flen = rs ;
	            if ((rs = fmtstr(fbuf,flen,0x01,fmt,ap)) >= 0) {
	                rs = proglog_print(pip,fbuf,rs) ;
	                wlen = rs ;
		    } /* end */
		    rs1 = mem.free(fbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (log-open) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (proglog_vprintf) */

int proglog_printf(PI *pip,cchar *fmt,...) noex {
	va_list		ap ;
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (pip && fmt) {
	    rs = SR_OK ;
	    if (pip->pfopen.logprog) {
	        va_begin(ap,fmt) ;
	        rs = proglog_vprintf(pip,fmt,ap) ;
	        wlen += rs ;
	        va_end(ap) ;
	    } /* end if (log-open) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (proglog_printf) */

int proglog_printfold(PI *pip,cchar *pre,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (pip && pre && sp) {
	    rs = SR_OK ;
	    if (pip->pfopen.logprog) {
	        if (logfile *lfp = resumelife<logfile>(pip->lfp)) {
	            rs = lfp->printfold(pre,sp,sl) ;
		    wlen += rs ;
		}
	    } /* end if (log-open) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (proglog_printfold) */

int proglog_ssprint(PI *pip,cchar *id,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (pip && sp) {
	    rs = SR_OK ;
	    if (pip->pfopen.logprog) {
	        if (logfile *lfp = resumelife<logfile>(pip->lfp)) {
	            if ((rs = lfp->setid(id)) >= 0) {
	                if ((rs = lfp->print(sp,sl)) >= 0) {
	    	            rs = lfp->setid(pip->logid) ;
			    wlen += rs ;
		        }
	            }
	        } else {
	            rs = lfp->print(sp,sl) ;
		    wlen += rs ;
	        } /* end if */
	    } /* end if (log-open) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (proglog_ssprint) */

int proglog_ssvprintf(PI *pip,cchar *id,cchar *fmt,va_list ap) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		wlen = 0 ;
	if (pip && id && fmt) {
	    rs = SR_OK ;
	    if (pip->pfopen.logprog) {
	        if (char *fbuf ; (rs = mem.ml(&fbuf)) >= 0) {
		    cint	flen = rs ;
	            if ((rs = fmtstr(fbuf,flen,0x01,fmt,ap)) >= 0) {
	                rs = proglog_ssprint(pip,id,fbuf,rs) ;
	                wlen = rs ;
	            }
		    rs1 = mem.free(fbuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (log-open) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (proglog_ssvprintf) */

int proglog_ssprintf(PI *pip,cchar *id,cchar *fmt,...) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (pip && id && fmt) {
	    rs = SR_OK ;
	    if (pip->pfopen.logprog) {
	        va_list	ap ;
	        va_begin(ap,fmt) ;
	        rs = proglog_ssvprintf(pip,id,fmt,ap) ;
		wlen += rs ;
	        va_end(ap) ;
	    } /* end if (log-open) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (proglog_ssprintf) */

int proglog_flush(PI *pip) noex {
	int		rs = SR_FAULT ;
	if (pip) {
	    rs = SR_OK ;
	    if (pip->pfopen.logprog) {
	        if (logfile *lfp = resumelife<logfile>(pip->lfp)) {
	            rs = lfp->flush ;
		}
	    } /* end if (log-open) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (proglog_flush) */


/* local subroutines */

local int proglog_lfname(PI *pip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (pip->pf.logprog) {
	    int		cl = -1 ;
	    cchar	*cp = pip->lfname ;
	    if ((cp == nullptr) || ischpl(cp[0])) {
	        cp = pip->searchname ;
	        cl = -1 ;
	    } /* end if */
	    if (cp && ischmi(cp[0])) {
		if (char *tbuf ; (rs = mem.mp(&tbuf)) >= 0) {
	            if (cp[0] != '/') {
	                if (strochr(cp,'/') != nullptr) {
	                    rs = mkpath(tbuf,pip->pr,cp) ;
			    cl = rs ;
	                } else {
			    ccharp logcname = LOGCNAME ;
	                    rs = mkpath(tbuf,pip->pr,logcname,cp) ;
			    cl = rs ;
	                } /* end */
	                cp = tbuf ;
	            } /* end if */
	            if ((rs >= 0) && cp) {
	                ccharpp vpp = &pip->lfname ;
	                if ((rs = pip->setent(vpp,cp,cl)) >= 0) {
	            	    pip->pfhave.logprog = true ;
			}
	            } /* end if */
		    rs1 = mem.free(tbuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } else {
	        pip->pf.logprog = false ;
	    } /* end if */
	} /* end if (opened) */
	return rs ;
} /* end subroutine (proglog_lfname) */

local int proglog_lfopen(PI *pip,UI *uip) noex {
	int		rs = SR_BUGCHECK ;
	int		fopened = false ;
	if (logfile *lfp = resumelife<logfile>(pip->lfp)) {
	   ccharp lfn = pip->lfname ;
	   ccharp lid = pip->logid ;
           if ((rs = lfp->open(lfn,0,0666,lid)) >= 0) {
               fopened = true ;
               pip->pfopen.logprog = true ;
               if_constexpr (f_comment) {
                   if (pip->debuglevel > 0) {
                       pip->eprintf("lf=%s\n",lfn) ;
                   }
               } /* end if_constexpr (f_comment) */
               if ((rs = proglog_intro(pip,uip)) >= 0) {
                   rs = proglog_cksize(pip) ;
               }
           } else if (isNotPresent(rs)) {
               rs = SR_OK ;
           } /* end */
	} /* end if (non-null) */
	return (rs >= 0) ? fopened : rs ;
} /* end subroutine (proglog_lfopen) */

local int proglog_lfobjbeg(PI *pip) noex {
    	cint		sz = szof(logfile) ;
    	int		rs ;
	if (voidp p ; (rs = mem.mall(sz,&p)) >= 0) {
    	    rs = SR_BUGCHECK ;
	    if (logfile *lfp = new(p) logfile) {
		rs = SR_OK ;
		pip->lfp = lfp ;
	    } /* end if (new-logfile) */
	    if (rs < 0) {
		mem.free(p) ;
	    } /* end if (error) */
	} /* end if (memory-acquire) */
	return rs ;
} /* end subroutine (proglog_lfobjbeg) */

local int proglog_lfobjend(PI *pip) noex {
    	int		rs = SR_OK ;
	int		rs1 ;
	if (logfile *lfp = resumelife<logfile>(pip->lzp)) {
	    {
		destroy_at(lfp) ;
	    } /* end */
	    {
		rs1 = mem.free(lfp) ;
		if (rs >= 0) rs = rs1 ;
		pip->lfp = nullptr ;
	    } /* end */
	} /* end if (non-nul) */
	return rs ;
} /* end subroutine (proglog_lfobjbeg) */


