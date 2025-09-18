/* tmpx_getuserterms SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get all of the terminals where the given user is logged in */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	This code was originally written. It was prompted by
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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<vecstr.h>
#include	<vecobj.h>
#include	<tmpx.h>
#include	<pathadd.h>
#include	<mkpath.h>
#include	<localmisc.h>


/* local defines */

#define	DEVDNAME	"/dev/"


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct terment {
	cchar		*devpath ;
	time_t		atime ;
} ;

typedef terment *	termentp ;

namespace {
    struct subinfo {
	tmpx		*op ;
	vecstr		*rlp ;		/* caller argument */
	cchar		*un ;		/* username */
	char		*tbuf = nullptr ;
	int		tlen ;
	int		dnl ;		/* directoy-name-length */
	subinfo(tmpx *o,vecstr *l,cchar *u) noex : op(o), rlp(l), un(u) { } ;
	int start() noex ;
	int finish() noex ;
	int entget() noex ;
	int entproc(vecobj *,int) noex ;
	int entstore(vecobj *) noex ;
	int entfins(vecobj *) noex ;
	operator int () noex ;
    } ; /* end struct (subinfo) */
}


/* forward references */

static int	terment_start(terment *,cchar *,int,time_t) noex ;
static int	terment_finish(terment *) noex ;

static int	mktermfname(char *,int,cchar *,int) noex ;
static int	getatime(cchar *,time_t *) noex ;
static int	revsortfunc(cvoid **,cvoid **) noex ;


/* local variables */

constexpr cchar		devdname[] = DEVDNAME ;


/* exported variables */


/* exported subroutines */

int tmpx_getuserterms(tmpx *op,vecstr *lp,cchar *username) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op && lp && username) {
	    rs = SR_INVALID ;
	    if (username[0]) {
		subinfo		go(op,lp,username) ;
		rs = go ;
		c = rs ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (tmpx_getuserterms) */


/* local subroutines */

int subinfo::start() noex {
	int		rs ;
	if ((rs = malloc_mp(&tbuf)) >= 0) {
	    tlen = rs ;
	    if ((rs = mkpath(tbuf,devdname)) >= 0) {
		dnl = rs ;
	    } /* end if (mkpath) */
	    if (rs < 0) {
		malloc_free(tbuf) ;
		tbuf = nullptr ;
		tlen = 0 ;
	    }
	} /* end if (memory-allocation) */
	return rs ;
}
/* end method (subinfo::start) */

int subinfo::finish() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (tbuf) {
	    rs1 = malloc_free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	    tbuf = nullptr ;
	    tlen = 0 ;
	}
	return rs ;
}
/* end method (subinfo::finish) */

subinfo::operator int () noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = start()) >= 0) {
	    {
		rs = entget() ;
		c = rs ;
	    }
	    rs1 = finish() ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (start-finish) */
	return (rs >= 0) ? c : rs ;
}
/* end method (subinfo::operator) */

int subinfo::entget() noex {
	vecobj		elist, *elp = &elist ;
	cint		esz = szof(terment) ;
	cint		vne = 10 ;
	cint		vo = VECOBJ_OSORTED ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = vecobj_start(elp,esz,vne,vo)) >= 0) {
	    tmpx_ent	ue ;
	    tmpx_cur	cur ;
	    bool	f ;
	    if ((rs = tmpx_curbegin(op,&cur)) >= 0) {
	    	cint	utl_line = TMPX_LLINE ;
	        while (rs >= 0) {
	            rs1 = tmpx_fetchuser(op,&cur,&ue,un) ;
	            if (rs1 == SR_NOTFOUND) break ;
	            rs = rs1 ;
	            if (rs < 0) break ;
	            f = false ;
	            f = f || (ue.ut_type != TMPX_TPROCUSER) ;
	            f = f || (ue.ut_line[0] == '\0') ;
	            if (f) {
			cint	sl = utl_line ;
			cchar	*sp = ue.ut_line ;
			if ((rs = mktermfname(tbuf,dnl,sp,sl)) >= 0) {
			    rs = entproc(elp,rs) ;
			    c += rs ;
			}
		    }
		} /* end while */
	        rs1 = tmpx_curend(op,&cur) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (cursor) */
	    if ((rs >= 0) && (c > 0)) {
		rs = entstore(elp) ;
	    } /* end if (positive) */
	    {
		rs1 = entfins(elp) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    rs1 = vecobj_finish(elp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vecobj) */
	return (rs >= 0) ? c : rs ;
}
/* end method (subinfo::operator) */

int subinfo::entproc(vecobj *elp,int tl) noex {
	time_t		ti_access{} ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs1 = getatime(tbuf,&ti_access)) >= 0) {
	    terment	te ;
	    if ((rs = terment_start(&te,tbuf,tl,ti_access)) >= 0) {
		c += 1 ;
		rs = vecobj_add(elp,&te) ;
		if (rs < 0)  {
		    terment_finish(&te) ;
		}
	    } /* end if (terment_start) */
	} /* end if (we had a better one) */
	return (rs >= 0) ? c : rs ;
}
/* end method (subinfo::entproc) */

int subinfo::entstore(vecobj *elp) noex {
	int		rs ;
	if ((rs = vecobj_sort(elp,revsortfunc)) >= 0) {
	    void	*vp{} ;
	    for (int i = 0 ; vecobj_get(elp,i,&vp) >= 0 ; i += 1) {
		if (vp) {
		    terment	*ep = termentp(vp) ;
		    rs = vecstr_add(rlp,ep->devpath,-1) ;
		}
		if (rs < 0) break ;
	    } /* end for */
	} /* end if (vecobj_sort) */
	return rs ;
}
/* end method (subinfo::entstore) */

int subinfo::entfins(vecobj *elp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	void		*vp{} ;
	for (int i = 0 ; vecobj_get(elp,i,&vp) >= 0 ; i += 1) {
	    if (vp) {
		terment	*ep = termentp(vp) ;
		{
	            rs1 = terment_finish(ep) ;
		    if (rs >= 0) rs = rs1 ;
		}
	    }
	} /* end for */
	return rs ;
}
/* end method (subinfo::entfins) */

static int terment_start(terment *ep,cchar *fp,int fl,time_t t) noex {
	int		rs = SR_FAULT ;
	if (ep && fp) {
	    ep->atime = t ;
	    if (cchar *cp ; (rs = libmem.strw(fp,fl,&cp)) >= 0) {
	        ep->devpath = cp ;
	    } /* end if (memory-allocation) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (terment_start) */

static int terment_finish(terment *ep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (ep) {
	    rs = SR_OK ;
	    if (ep->devpath) {
		void *vp = voidp(ep->devpath) ;
	        rs1 = libmem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->devpath = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (terment_finish) */

static int mktermfname(char *rbuf,int ddnl,cchar *sp,int sl) noex {
	return pathaddw(rbuf,ddnl,sp,sl) ; /* <- nice refactor here */
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
	                rc = intconv(f2p->atime - f1p->atime) ; /* reversed */
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


