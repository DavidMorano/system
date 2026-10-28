/* proglog_log1 MODULE (implementation[ ) */
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
#include	<ucmem.h>		/* LIBUC */
#include	<fmtstr.h>		/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<isch.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<logfile.h>		/* LIBDAM */
#include	<userinfo.h>		/* LIBDAM */
#include	<bfile.h>		/* LIBB */

#include	"proginfo.hh"

module proginfo.log ;

import proginfo.base ;

/* local defines */


/* imported namespaces */

using libuc::mem ;		/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int	proglog_file(proginfo *) noex ;


/* local variables */

statcexpr bool		f_comment = false ;


/* exported variables */


/* exported subroutines */

    int proglog_begin(proginfo *pip,userinfo *uip) noex {
	int		rs = SR_FAULT ;
	int		f_opened = false ; /* return-value */
	if (pip && uip) ylikely {
	    rs = SR_OK ;
	    if (pip->pf.logprog) {
	        if ((rs = proglog_file(pip)) >= 0) {
	            if (pip->lfname != nullptr) {
		        logfile	*lfp = pip->lfp ;
	                cchar	*lf = pip->lfname ;
	                cchar	*li = pip->logid ;
	                if ((rs = lfp->open(lf,0,0666,li)) >= 0) {
	                    f_opened = true ;
	                    pip->pfopen.logprog = true ;
			    if_constexpr (f_comment) {
	                        if (pip->debuglevel > 0) {
	                            pip->eprintf("lf=%s\n",lf) ;
			        }
			    } /* end if_constexpr (f_comment) */
			    if ((rs = proglog_intro(pip,uip)) >= 0) {
	                        rs = proglog_cksize(pip) ;
			    }
	                } else if (isNotPresent(rs)) {
	                    rs = SR_OK ;
		        } /* end */
	            } /* end if */
	        } /* end if (log-file) */
	    } /* end if (enabled) */
	} /* end if (non-null) */
	return (rs >= 0) ? f_opened : rs ;
    } /* end subroutine (proglog_begin) */

    int proglog_end(proginfo *pip) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (pip) {
	    rs = SR_OK ;
	    if (pip->pfopen.logprog) {
	        logfile	*lfp = pip->lfp ;
	        pip->pfopen.logprog = false ;
	        rs1 = lfp->close ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (log-open) */
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (proglog_end) */

    int proglog_intro(proginfo *pip,userinfo *uip) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ; /* return-value */
	if (pip) {
	    rs = SR_OK ;
	    if (pip->pfopen.logprog && pip->pf.logprog) {
	        logfile	*lfp = pip->lfp ;
	        if (uip != nullptr) {
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
	    } /* end if (enabled and open) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
    } /* end subroutine (proglog_intro) */

    int proglog_checksize(proginfo *pip) noex {
	int		rs = SR_FAULT ;
	if (pip) {
	    rs = SR_OK ;
	    if (pip->pfopen.logprog && (pip->logsize > 0)) {
	        logfile	*lfp = pip->lfp ;
	        rs = lfp->cksize(pip->logsize) ;
	    } /* end if (log-open) */
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (proglog_checksize) */

    int proglog_check(proginfo *pip) noex {
	int		rs = SR_FAULT ;
	if (pip) {
	    rs = SR_OK ;
	    if (pip->pfopen.logprog) {
	        logfile	*lfp = pip->lfp ;
	        rs = lfp->check(pip->daytime) ;
	    } /* end if (log-open) */
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (proglog_check) */

    int proglog_getid(proginfo *pip,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	if (pip && rbuf) {
	    rs = sncpy1(rbuf,rlen,pip->logid) ;
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (proglog_getid) */

    int proglog_setid(proginfo *pip,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (pip && sp) {
	    rs = SR_OK ;
	    if (pip->pfopen.logprog) {
	        cchar	*logid ;
	        if (nulstr ls ; (rs = ls.start(sp,sl,&logid)) >= 0) {
		    {
	                logfile	*lfp = pip->lfp ;
	                rs = lfp->setid(logid) ;
		        c = rs ;
		    }
		    rs1 = ls.finish ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (nulstr) */
	    } /* end if (log-open) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
    } /* end subroutine (proglog_setid) */

    int proglog_print(proginfo *pip,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (pip && sp) {
	    rs = SR_OK ;
	    if (pip->pfopen.logprog) {
	        logfile	*lfp = pip->lfp ;
	        rs = lfp->print(sp,sl) ;
		wlen += rs ;
	    } /* end if (log-open) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
    } /* end subroutine (proglog_print) */

    int proglog_vprintf(proginfo *pip,cchar *fmt,va_list ap) noex {
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
		    }
		    rs1 = mem.free(fbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (log-open) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
    } /* end subroutine (proglog_vprintf) */

    int proglog_printf(proginfo *pip,cchar *fmt,...) noex {
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

    int proglog_printfold(proginfo *pip,cchar *pre,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (pip && pre && sp) {
	    rs = SR_OK ;
	    if (pip->pfopen.logprog) {
	        logfile	*lfp = pip->lfp ;
	        rs = lfp->printfold(pre,sp,sl) ;
		wlen += rs ;
	    } /* end if (log-open) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
    } /* end subroutine (proglog_printfold) */

    int proglog_ssprint(proginfo *pip,cchar *id,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (pip && sp) {
	    rs = SR_OK ;
	    if (pip->pfopen.logprog) {
	        logfile		*lfp = pip->lfp ;
	        if (id != nullptr) {
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

    int proglog_ssvprintf(proginfo *pip,cchar *id,cchar *fmt,va_list ap) noex {
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

    int proglog_ssprintf(proginfo *pip,cchar *id,cchar *fmt,...) noex {
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

    int proglog_flush(proginfo *pip) noex {
	int		rs = SR_FAULT ;
	if (pip) {
	    rs = SR_OK ;
	    if (pip->pfopen.logprog) {
	        logfile	*lfp = pip->lfp ;
	        rs = lfp->flush ;
	    } /* end if (log-open) */
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (proglog_flush) */


/* local subroutines */

local int proglog_file(proginfo *pip) noex {
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
			    cchar	*logcname = LOGCNAME ;
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
} /* end subroutine (proglog_file) */


