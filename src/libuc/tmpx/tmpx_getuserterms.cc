/* tmpx_getuserterms SUPPORT */
/* lang=C++20 */

/* get all of the terminals where the given user is logged in */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	This subroutine was originally written. It was prompted by
	the failure of other terminal message programs from finding
	the proper controlling terminal.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	tmpx_getuserterms

	Descrption:
	This subroutine will find and return the names of all of
	the controlling terminals for the specified username, if
	there are any.

	Synopsis:
	int tmpx_getuserterms(tmpx *op,vecstr *lp,cchar *username) noex

	Arguments:
	op		pointer to TMPX object
	lp		pointer to VECSTR to receive terminals
	username	user to find controlling terminals for

	Returns:
	>=0		number of entries returned
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<ctime>
#include	<cstring>
#include	<usystem.h>
#include	<vecstr.h>
#include	<vecitem.h>
#include	<tmpx.h>
#include	<storebuf.h>
#include	<mkpath.h>
#include	<localmisc.h>


/* local defines */

#define	DEVDNAME	"/dev/"


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct terment {
	cchar		*devpath ;
	time_t		atime ;
} ;

typedef terment *	termentp ;


/* forward references */

static int	entry_start(terment *,char *,int,time_t) noex ;
static int	entry_finish(terment *) noex ;

static int	mktermfname(char *,int,cchar *,int) noex ;
static int	getatime(cchar *,time_t *) noex ;
static int	revsortfunc(cvoid **,cvoid **) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int tmpx_getuserterms(tmpx *op,vecstr *lp,cchar *username) noex {
	vecitem		el ;
	int		rs ;
	int		rs1 ;
	int		ddnl ;
	int		n = 0 ;
	cchar		*devdname = DEVDNAME ;
	char		termfname[MAXPATHLEN + 1] ;

	if (op == NULL) return SR_FAULT ;
	if (lp == NULL) return SR_FAULT ;
	if (username == NULL) return SR_FAULT ;

	if (username[0] == '\0') return SR_INVALID ;

	ddnl = mkpath1(termfname,devdname) ;

	if ((rs = vecitem_start(&el,10,VECITEM_OSORTED)) >= 0) {
	    terment	*ep ;
	    tmpx_ent		ue ;
	    tmpx_cur		cur ;
	    cint		llen = TMPX_LLINE ;
	    int			i ;
	    int			f ;

	    if ((rs = tmpx_curbegin(op,&cur)) >= 0) {
	        time_t	ti_access ;
		int	tlen ;

	        while (rs >= 0) {
	            rs1 = tmpx_fetchuser(op,&cur,&ue,username) ;
	            if (rs1 == SR_NOTFOUND) break ;
	            rs = rs1 ;
	            if (rs < 0) break ;

	            f = false ;
	            f = f || (ue.ut_type != TMPX_TPROCUSER) ;
	            f = f || (ue.ut_line[0] == '\0') ;
#ifdef	COMMENT
	            f = f || (strncmp(username,ue.ut_user,llen) != 0) ;
#endif
	            if (f)
	                continue ;

	            tlen = mktermfname(termfname,ddnl,ue.ut_line,llen) ;

/* check the access time of this terminal and permissions */

	            if ((rs1 = getatime(termfname,&ti_access)) >= 0) {
	                terment		te ;
			cint		ti = ti_access ;
	                if ((rs = entry_start(&te,termfname,tlen,ti)) >= 0) {
	                    cint	esize = sizeof(terment) ;
	                    n += 1 ;
	                    rs = vecitem_add(&el,&te,esize) ;
	                    if (rs < 0)  {
				entry_finish(&te) ;
			    }
			}
	            } /* end if (we had a better one) */

	        } /* end while (looping through entries) */

	        rs1 = tmpx_curend(op,&cur) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (cursor) */

	    if ((rs >= 0) && (n > 0)) {
	        if ((rs = vecitem_sort(&el,revsortfunc)) >= 0) {
	            for (i = 0 ; vecitem_get(&el,i,&ep) >= 0 ; i += 1) {
	                if (ep) {
	                    rs = vecstr_add(lp,ep->devpath,-1) ;
		        }
	                if (rs < 0) break ;
	            } /* end for */
		} /* end if (vecitem_sort) */
	    } /* end if (positive) */

/* free up */

	    for (i = 0 ; vecitem_get(&el,i,&ep) >= 0 ; i += 1) {
	        if (ep) {
	            entry_finish(ep) ;
		}
	    } /* end for */

	    rs1 = vecitem_finish(&el) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vecitem) */

	return (rs >= 0) ? n : rs ;
}
/* end subroutine (tmpx_getuserterms) */


/* local subroutines */

static int entry_start(terment *ep,char *fp,int fl,time_t t) noex {
	int		rs = SR_FAULT ;
	if (ep && fp) {
	    cchar	*cp{} ;
	    ep->atime = t ;
	    if ((rs = uc_mallocstrw(fp,fl,&cp)) >= 0) {
	        ep->devpath = cp ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entry_start) */

static int entry_finish(terment *ep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (ep) {
	    rs = SR_OK ;
	    if (ep->devpath) {
	        rs1 = uc_free(ep->devpath) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->devpath = NULL ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entry_finish) */

static int mktermfname(char *rbuf,int ddnl,cchar *sp,int sl) noex {
	cint		rlen = MAXPATHLEN ;
	int		rs ;
	int		len = ddnl ;

	rs = storebuf_strw(rbuf,rlen,ddnl,sp,sl) ;
	len += rs ;

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mktermfname) */

static int getatime(cchar *termdev,time_t *rp) noex {
	USTAT		sb ;
	int		rs ;
	int		f = true ;
	*rp = 0 ;
	if ((rs = u_stat(termdev,&sb)) >= 0) {
	    *rp = sb.st_atime ;
	    if ((sb.st_mode & S_IWGRP) != S_IWGRP) {
	        f = false ;
	    }
	} /* end if */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (getatime) */

static int revsortfunc(cvoid **v1pp,cvoid **v2pp) noex {
	cvoid		*v1p = *v1pp ;
	cvoid		*v2p = *v2pp ;
	int		rc = 0 ;
	{
	    const terment *f1p = termentp(v1p) ;
	    const terment *f2p = termentp(v2p) ;
	    if (f1p || f2p) {
	        if (f1p) {
	            if (f2p) {
	                rc = (f2p->atime - f1p->atime) ; /* reversed */
	            } else {
	                rc = -1 ;
		    }
	        } else {
	            rc = 1 ;
		}
	    } 
	} /* end block */
	return rc ;
}
/* end subroutine (revsortfunc) */


