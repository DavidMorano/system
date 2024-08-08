/* proglog SUPPORT */
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

	Manage the general process logging.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<bfile.h>
#include	<logfile.h>
#include	<userinfo.h>
#include	<format.h>
#include	<nulstr.h>
#include	<sncpyx.h>
#include	<mkpathx.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"proglog.h"


/* local defines */


/* external subroutines */

extern "C" {
    int		proglog_begin(proginfo *,userinfo *) noex ;
    int		proglog_end(proginfo *) noex ;
    int		proglog_intro(proginfo *,userinfo *) noex ;
    int		proglog_checksize(proginfo *) noex ;
}


/* external variables */


/* local structures */


/* forward references */

static int	proglog_file(proginfo *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int proglog_begin(proginfo *pip,userinfo *uip) noex {
	int		rs = SR_OK ;
	int		f_opened = false ;
	if (pip->pf.logprog) {
	    if ((rs = proglog_file(pip)) >= 0) {
	        if (pip->lfname != nullptr) {
		    logfile	*lhp = pip->lhp ;
	            cchar	*lf = pip->lfname ;
	            cchar	*li = pip->logid ;
	            if ((rs = logfile_open(lhp,lf,0,0666,li)) >= 0) {
	                f_opened = true ;
	                pip->open.logprog = true ;
#ifdef	COMMENT
	                if (pip->debuglevel > 0) {
		    	    cchar	*pn = pip->progname ;
	                    bprintf(pip->efp,"%s: lf=%s\n",pn,lf) ;
			}
#endif /* COMMENT */
			if ((rs = proglog_intro(pip,uip)) >= 0) {
	                    rs = proglog_checksize(pip) ;
			}
	            } else if (isNotPresent(rs)) {
	                rs = SR_OK ;
		    }
	        } /* end if */
	    } /* end if (log-file) */
	} /* end if (enabled) */

	return (rs >= 0) ? f_opened : rs ;
}
/* end subroutine (proglog_begin) */

int proglog_end(proginfo *pip) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (pip) {
	    rs = SR_OK ;
	    if (pip->open.logprog) {
	        logfile	*lhp = pip->lhp ;
	        pip->open.logprog = false ;
	        rs1 = logfile_close(lhp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (proglog_end) */

int proglog_intro(proginfo *pip,userinfo *uip) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (pip) {
	    rs = SR_OK ;
	    if (pip->open.logprog && pip->pf.logprog) {
	        logfile	*lhp = pip->lhp ;
	        if (uip != nullptr) {
		    custime	dt = pip->daytime ;
		    cchar	*pn = pip->progname ;
		    cchar	*ver = pip->version ;
	            rs = logfile_userinfo(lhp,uip,dt,pn,ver) ;
		    wlen += rs ;
	        }
	        if (rs >= 0) {
	            rs = logfile_printf(lhp,"pr=%s\n",pip->pr) ;
		    wlen += rs ;
	        }
	    } /* end if (enabled and open) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (proglog_intro) */

int proglog_checksize(proginfo *pip) noex {
	int		rs = SR_FAULT ;
	if (pip) {
	    rs = SR_OK ;
	    if (pip->open.logprog && (pip->logsize > 0)) {
	        logfile		*lhp = pip->lhp ;
	        rs = logfile_checksize(lhp,pip->logsize) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (proglog_checksize) */

int proglog_check(proginfo *pip) noex {
	int		rs = SR_FAULT ;
	if (pip) {
	    rs = SR_OK ;
	    if (pip->open.logprog) {
	        logfile	*lhp = pip->lhp ;
	        rs = logfile_check(lhp,pip->daytime) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (proglog_check) */

int proglog_getid(proginfo *pip,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	if (pip && rbuf) {
	    rs = sncpy1(rbuf,rlen,pip->logid) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (proglog_getid) */

int proglog_setid(proginfo *pip,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (pip && sp) {
	    rs = SR_OK ;
	    if (pip->open.logprog) {
	        nulstr	ns ;
	        cchar	*logid ;
	        if ((rs = nulstr_start(&ns,sp,sl,&logid)) >= 0) {
		    {
	                logfile	*lhp = pip->lhp ;
	                rs = logfile_setid(lhp,logid) ;
		        c = rs ;
		    }
		    rs1 = nulstr_finish(&ns) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (nulstr) */
	    } /* end if (log-open) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (proglog_setid) */

int proglog_print(proginfo *pip,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (pip && sp) {
	    rs = SR_OK ;
	    if (pip->open.logprog) {
	        logfile	*lhp = pip->lhp ;
	        rs = logfile_print(lhp,sp,sl) ;
		wlen += rs ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (proglog_print) */

/* vprintf-like thing */
int proglog_vprintf(proginfo *pip,cchar *fmt,va_list ap) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		wlen = 0 ;
	if (pip && fmt) {
	    rs = SR_OK ;
	    if (pip->open.logprog) {
	        char	*fbuf{} ;
		if ((rs = malloc_ml(&fbuf)) >= 0) {
		    cint	flen = rs ;
	            if ((rs = format(fbuf,flen,0x01,fmt,ap)) >= 0) {
	                rs = proglog_print(pip,fbuf,rs) ;
	                wlen = rs ;
		    }
		    rs1 = uc_free(fbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (proglog_vprintf) */

/* PRINTFLIKE2 */
int proglog_printf(proginfo *pip,cchar *fmt,...) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (pip && fmt) {
	    rs = SR_OK ;
	    if (pip->open.logprog) {
	        va_list		ap ;
	        va_begin(ap,fmt) ;
	        rs = proglog_vprintf(pip,fmt,ap) ;
	        wlen += rs ;
	        va_end(ap) ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (proglog_printf) */

int proglog_printfold(proginfo *pip,cchar *pre,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (pip && pre && sp) {
	    rs = SR_OK ;
	    if (pip->open.logprog) {
	        logfile	*lhp = pip->lhp ;
	        rs = logfile_printfold(lhp,pre,sp,sl) ;
		wlen += rs ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (proglog_printfold) */

int proglog_ssprint(proginfo *pip,cchar *id,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (pip && sp) {
	    rs = SR_OK ;
	    if (pip->open.logprog) {
	        logfile		*lhp = pip->lhp ;
	        if (id != nullptr) {
	            if ((rs = logfile_setid(lhp,id)) >= 0) {
	                if ((rs = logfile_print(lhp,sp,sl)) >= 0) {
	    	            rs = logfile_setid(lhp,pip->logid) ;
			    wlen += rs ;
		        }
	            }
	        } else {
	            rs = logfile_print(lhp,sp,sl) ;
		    wlen += rs ;
	        }
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (proglog_ssprint) */

/* vprintf-like thing */
int proglog_ssvprintf(proginfo *pip,cchar *id,cchar *fmt,va_list ap) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		wlen = 0 ;
	if (pip && id && fmt) {
	    rs = SR_OK ;
	    if (pip->open.logprog) {
	        char	*fbuf{} ;
		if ((rs = malloc_ml(&fbuf)) >= 0) {
		    cint	flen = rs ;
	            if ((rs = format(fbuf,flen,0x01,fmt,ap)) >= 0) {
	                rs = proglog_ssprint(pip,id,fbuf,rs) ;
	                wlen = rs ;
	            }
		    rs1 = uc_free(fbuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (proglog_ssvprintf) */

/* PRINTFLIKE2 */
int proglog_ssprintf(proginfo *pip,cchar *id,cchar *fmt,...) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (pip && id && fmt) {
	    rs = SR_OK ;
	    if (pip->open.logprog) {
	        va_list	ap ;
	        va_begin(ap,fmt) ;
	        rs = proglog_ssvprintf(pip,id,fmt,ap) ;
		wlen += rs ;
	        va_end(ap) ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (proglog_ssprintf) */

int proglog_flush(proginfo *pip) noex {
	int		rs = SR_FAULT ;
	if (pip) {
	    rs = SR_OK ;
	    if (pip->open.logprog) {
	        logfile	*lhp = pip->lhp ;
	        rs = logfile_flush(lhp) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (proglog_flush) */


/* local subroutines */

static int proglog_file(proginfo *pip) noex {
	int		rs = SR_OK ;
	if (pip->pf.logprog) {
	    int		cl = -1 ;
	    cchar	*cp = pip->lfname ;
	    if ((cp == nullptr) || (cp[0] == '+')) {
	        cp = pip->searchname ;
	        cl = -1 ;
	    }
	    if (cp[0] != '-') {
	        char	tbuf[MAXPATHLEN + 1] ;
	        pip->have.logprog = true ;
	        if (cp[0] != '/') {
	            if (strchr(cp,'/') != nullptr) {
	                cl = mkpath2(tbuf,pip->pr,cp) ;
	            } else {
			cchar	*logcname = LOGCNAME ;
	                cl = mkpath3(tbuf,pip->pr,logcname,cp) ;
	            }
	            cp = tbuf ;
	        }
	        if (cp != nullptr) {
	            cchar	**vpp = &pip->lfname ;
	            rs = proginfo_setentry(pip,vpp,cp,cl) ;
	        }
	    } else {
	        pip->pf.logprog = false ;
	    }
	} /* end if (opened) */
	return rs ;
}
/* end subroutine (proglog_file) */


