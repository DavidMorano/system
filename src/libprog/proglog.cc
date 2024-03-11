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
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<bfile.h>
#include	<userinfo.h>
#include	<format.h>
#include	<nulstr.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"


/* local defines */


/* external subroutines */

extern "C" {
    int		proglog_begin(PROGINFO *,USERINFO *) noex ;
    int		proglog_end(PROGINFO *) noex ;
    int		proglog_intro(PROGINFO *,USERINFO *) noex ;
    int		proglog_checksize(PROGINFO *) noex ;
}


/* external variables */


/* local structures */


/* forward references */

static int	proglog_file(PROGINFO *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int proglog_begin(PROGINFO *pip,USERINFO *uip) noex {
	int		rs = SR_OK ;
	int		f_opened = false ;

	if (pip->f.logprog) {
	    if ((rs = proglog_file(pip)) >= 0) {
	        if (pip->lfname != nullptr) {
		    LOGFILE	*lhp = &pip->lh ;
		    const char	*pn = pip->progname ;
	            const char	*lf = pip->lfname ;
	            const char	*li = pip->logid ;
	            if ((rs = logfile_open(lhp,lf,0,0666,li)) >= 0) {
	                f_opened = true ;
	                pip->open.logprog = true ;
	                if (pip->debuglevel > 0) {
	                    bprintf(pip->efp,"%s: lf=%s\n",pn,lf) ;
			}
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


int proglog_end(PROGINFO *pip)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (pip->open.logprog) {
	    LOGFILE	*lhp = &pip->lh ;
	    pip->open.logprog = false ;
	    rs1 = logfile_close(lhp) ;
	    if (rs >= 0) rs = rs1 ;
	}

	return rs ;
}
/* end subroutine (proglog_end) */


int proglog_intro(PROGINFO *pip,USERINFO *uip)
{
	int		rs = SR_OK ;

	if (pip->open.logprog && pip->f.logprog) {
	    LOGFILE	*lhp = &pip->lh ;

	    if (uip != nullptr) {
		const time_t	dt = pip->daytime ;
		cchar		*pn = pip->progname ;
		cchar		*ver = pip->version ;
	        rs = logfile_userinfo(lhp,uip,dt,pn,ver) ;
	    }

	    if (rs >= 0) {
	        rs = logfile_printf(lhp,"pr=%s\n",pip->pr) ;
	    }

	} /* end if (enabled and open) */

	return rs ;
}
/* end subroutine (proglog_intro) */


int proglog_checksize(PROGINFO *pip)
{
	int		rs = SR_OK ;
	if (pip->open.logprog && (pip->logsize > 0)) {
	    LOGFILE	*lhp = &pip->lh ;
	    rs = logfile_checksize(lhp,pip->logsize) ;
	}
	return rs ;
}
/* end subroutine (proglog_checksize) */


int proglog_check(PROGINFO *pip)
{
	int		rs = SR_OK ;
	if (pip->open.logprog) {
	    LOGFILE	*lhp = &pip->lh ;
	    rs = logfile_check(lhp,pip->daytime) ;
	}
	return rs ;
}
/* end subroutine (proglog_check) */


int proglog_getid(PROGINFO *pip,char *rbuf,int rlen)
{
	if (pip == nullptr) return SR_FAULT ;
	if (rbuf == nullptr) return SR_FAULT ;
	return sncpy1(rbuf,rlen,pip->logid) ;
}
/* end subroutine (proglog_getid) */


int proglog_setid(PROGINFO *pip,cchar *sp,int sl)
{
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	if (pip == nullptr) return SR_FAULT ;
	if (sp == nullptr) return SR_FAULT ;
	if (pip->open.logprog) {
	    NULSTR	ns ;
	    cchar	*logid ;
	    if ((rs = nulstr_start(&ns,sp,sl,&logid)) >= 0) {
		{
	            LOGFILE	*lhp = &pip->lh ;
	            rs = logfile_setid(lhp,logid) ;
		    c = rs ;
		}
		rs1 = nulstr_finish(&ns) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (nulstr) */
	} /* end if (log-open) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (proglog_setid) */


int proglog_print(PROGINFO *pip,cchar *sp,int sl)
{
	int		rs = SR_OK ;
	if (pip == nullptr) return SR_FAULT ;
	if (sp == nullptr) return SR_FAULT ;
	if (pip->open.logprog) {
	    LOGFILE	*lhp = &pip->lh ;
	    rs = logfile_print(lhp,sp,sl) ;
	}
	return rs ;
}
/* end subroutine (proglog_print) */


/* vprintf-like thing */
int proglog_vprintf(PROGINFO *pip,cchar *fmt,va_list ap)
{
	int		rs = SR_OK ;
	int		wlen = 0 ;

	if (pip == nullptr) return SR_FAULT ;
	if (fmt == nullptr) return SR_FAULT ;

	if (pip->open.logprog) {
	    const int	flen = LINEBUFLEN ;
	    char	fbuf[LINEBUFLEN+1] ;
	    if ((rs = format(fbuf,flen,0x01,fmt,ap)) >= 0) {
	        rs = proglog_print(pip,fbuf,rs) ;
	        wlen = rs ;
	    }
	}

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (proglog_vprintf) */


/* PRINTFLIKE2 */
int proglog_printf(PROGINFO *pip,cchar *fmt,...)
{
	int		rs = SR_OK ;
	if (pip->open.logprog) {
	    va_list	ap ;
	    va_begin(ap,fmt) ;
	    rs = proglog_vprintf(pip,fmt,ap) ;
	    va_end(ap) ;
	}
	return rs ;
}
/* end subroutine (proglog_printf) */


int proglog_printfold(PROGINFO *pip,cchar *pre,cchar *sp,int sl)
{
	int		rs = SR_OK ;
	if (pip == nullptr) return SR_FAULT ;
	if (pre == nullptr) return SR_FAULT ;
	if (sp == nullptr) return SR_FAULT ;
	if (pip->open.logprog) {
	    LOGFILE	*lhp = &pip->lh ;
	    rs = logfile_printfold(lhp,pre,sp,sl) ;
	}
	return rs ;
}
/* end subroutine (proglog_printfold) */


int proglog_ssprint(PROGINFO *pip,cchar *id,cchar *sp,int sl)
{
	int		rs = SR_OK ;
	if (pip == nullptr) return SR_FAULT ;
	if (sp == nullptr) return SR_FAULT ;
	if (pip->open.logprog) {
	    LOGFILE	*lhp = &pip->lh ;
	    if (id != nullptr) {
	        if ((rs = logfile_setid(lhp,id)) >= 0) {
	            if ((rs = logfile_print(lhp,sp,sl)) >= 0) {
	    	        rs = logfile_setid(lhp,pip->logid) ;
		    }
	        }
	    } else {
	        rs = logfile_print(lhp,sp,sl) ;
	    }
	}
	return rs ;
}
/* end subroutine (proglog_ssprint) */


/* vprintf-like thing */
int proglog_ssvprintf(PROGINFO *pip,cchar *id,cchar *fmt,va_list ap)
{
	int		rs = SR_OK ;
	int		wlen = 0 ;

	if (pip == nullptr) return SR_FAULT ;
	if (fmt == nullptr) return SR_FAULT ;

	if (pip->open.logprog) {
	    const int	flen = LINEBUFLEN ;
	    char	fbuf[LINEBUFLEN+1] ;
	    if ((rs = format(fbuf,flen,0x01,fmt,ap)) >= 0) {
	        rs = proglog_ssprint(pip,id,fbuf,rs) ;
	        wlen = rs ;
	    }
	}

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (proglog_ssvprintf) */


/* PRINTFLIKE2 */
int proglog_ssprintf(PROGINFO *pip,cchar *id,cchar *fmt,...)
{
	int		rs = SR_OK ;
	if (pip->open.logprog) {
	    va_list	ap ;
	    va_begin(ap,fmt) ;
	    rs = proglog_ssvprintf(pip,id,fmt,ap) ;
	    va_end(ap) ;
	}
	return rs ;
}
/* end subroutine (proglog_ssprintf) */


int proglog_flush(PROGINFO *pip)
{
	int		rs = SR_OK ;
	if (pip == nullptr) return SR_FAULT ;
	if (pip->open.logprog) {
	    LOGFILE	*lhp = &pip->lh ;
	    rs = logfile_flush(lhp) ;
	}
	return rs ;
}
/* end subroutine (proglog_flush) */


/* local subroutines */


static int proglog_file(PROGINFO *pip)
{
	int		rs = SR_OK ;

	if (pip->f.logprog) {
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
	        pip->f.logprog = false ;
	    }
	} /* end if (opened) */

	return rs ;
}
/* end subroutine (proglog_file) */


