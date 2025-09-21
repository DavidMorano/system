/* getuserterms SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get a list (returned as vector of c-strings) of the logged in user terms */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-01-10, David A­D­ Morano
	This code was originally written.  It was prompted by
	the failure of other terminal message programs from finding
	the proper controlling terminal.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getuserterms

	Description:
	This subroutine will find and return a list (in a VECSTR
	object) of the logged-in terminals of the given username.

	Synopsis:
	int getuserterms(vecstr *lp,cchar *username) noex

	Arguments:
	- listp		pointer to VECSTR to receive terminals
	- username	session ID to find controlling terminal for

	Returns:
	>=	number of entries returned
	<0	error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<vecstr.h>
#include	<vecobj.h>
#include	<tmpx.h>
#include	<strwcpy.h>
#include	<intsat.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"getuserterms.h"


/* local defines */

#ifndef	DEVDNAME
#define	DEVDNAME	"/dev/"
#endif

#define	TE		terment


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

namespace {
    struct userterms {
	vecobj		el ;
	cchar		*un ;
	char		*tbuf = nullptr ;
	int		tlen ;
	int		tl = 0 ;
	userterms(cchar *n) noex : un(n) { } ;
	int operator () (vecstr *) noex ;
	int start() noex ;
	int finish () noex ;
	int proc() noex ;
	int load(vecstr *) noex ;
	int entfins() noex ;
    } ; /* end struct (userterms) */
} /* end namespace */


/* forward references */

static int	terment_start(TE *,cc *,int,time_t) noex ;
static int	terment_finish(TE *) noex ;

static int	getatime(cchar *,time_t *) noex ;
static int	revsortfunc(cvoid **,cvoid **) noex ;


/* local variables */

constexpr int	lline = TMPX_LLINE ;


/* exported variables */


/* exported subroutines */

int getuserterms(vecstr *lp,cchar *un) noex {
	int		rs = SR_FAULT ;
	int		n = 0 ;
	if (lp && un) {
	    rs = SR_INVALID ;
	    if (un[0]) {
		userterms	uo(un) ;
		rs = uo(lp) ;
		n = rs ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (getuserterms) */


/* local subroutines */

int userterms::operator () (vecstr *tlp) noex {
	int		rs ;
	int		rs1 ;
	int		n = 0 ;
	if ((rs = start()) >= 0) {
	    if ((rs = proc()) >= 0) {
		n = rs ;
		rs = load(tlp) ;
	    } /* end if (vecobj_proc) */
	    rs1 = finish() ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (start-finish) */
	return (rs >= 0) ? n : rs ;
}

int userterms::start() noex {
	cint		osz = szof(terment) ;
	cint		vn = 10 ;
	cint		vo = VECOBJ_OSORTED ;
	int		rs ;
	if ((rs = vecobj_start(&el,osz,vn,vo)) >= 0) {
	    if ((rs = malloc_mp(&tbuf)) >= 0) {
		tlen = rs ;
		tl = intconv(strwcpy(tbuf,DEVDNAME) - tbuf) ;
	    } /* end if (memory-allocation) */
	    if (rs < 0) {
		vecobj_finish(&el) ;
	    }
	}
	return rs ;
}

int userterms::finish() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (tbuf) {
	    rs1 = malloc_free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	}
	{
	    rs1 = entfins() ;
	    if (rs >= 0) rs = rs1 ;
	}
	{
	    rs1 = vecobj_finish(&el) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}

int userterms::load(vecstr *tlp) noex {
	int		rs ;
	if ((rs = vecobj_sort(&el,revsortfunc)) >= 0) {
	    void	*vp{} ;
	    for (int i = 0 ; vecobj_get(&el,i,&vp) >= 0 ; i += 1) {
		TE	*ep = (TE *) vp ;
	        if (vp) {
	            rs = tlp->add(ep->devpath) ;
	        }
		if (rs < 0) break ;
	    } /* end for */
	} /* end if */
	return rs ;
}

int userterms::entfins() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	void		*vp{} ;
	for (int i = 0 ; vecobj_get(&el,i,&vp) >= 0 ; i += 1) {
	    TE	*ep = (TE *) vp ;
	    if (vp) {
	        rs1 = terment_finish(ep) ;
		if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */
	return rs ;
}

int userterms::proc() noex {
	cint		of = O_RDONLY ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (tmpx tx ; (rs = tmpx_open(&tx,nullptr,of)) >= 0) {
	    if (tmpx_cur cur ; (rs = tmpx_curbegin(&tx,&cur)) >= 0) {
	        tmpx_ent	ue ;
		char		*bp = (tbuf + tl) ;
	        while ((rs1 = tmpx_fetchuser(&tx,&cur,&ue,un)) > 0) {
		    int		rl ;
		    bool	f = false ;
	            f = f || (ue.ut_type != TMPX_TPROCUSER) ;
	            f = f || (ue.ut_line[0] == '\0') ;
		    if (f) continue ;
	            rl = intconv(strwcpy(bp,ue.ut_line,lline) - tbuf) ;
		    if (time_t tia{} ; (rs = getatime(tbuf,&tia)) >= 0) {
	                TE	te ;
	                if ((rs = terment_start(&te,tbuf,rl,tia)) >= 0) {
	                    rs = vecobj_add(&el,&te) ;
			    c += 1 ;
	                }
			if (rs < 0) {
	                    terment_finish(&te) ;
			}
	            } /* end if (we had a better one) */
		    if (rs < 0) break ;
	        } /* end while (looping through entries) */
		if (rs >= 0) rs = rs1 ;
	        rs1 = tmpx_curend(&tx,&cur) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if */
	    rs1 = tmpx_close(&tx) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (UTMPX open) */
	return (rs >= 0) ? c : rs ;
}
/* end method (userterms::proc) */

static int terment_start(TE *ep,cc *sp,int sl,time_t t) noex {
	int		rs ;
	ep->atime = t ;
	if (cchar *cp ; (rs = libmem.strw(sp,sl,&cp)) >= 0) {
	    ep->devpath = cp ;
	} /* end if (memory-allocation) */
	return rs ;
}
/* end subroutine (terment_start) */

static int terment_finish(TE *ep) noex {
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

static int getatime(cc *termdev,time_t *tp) noex {
	int		rs ;
	*tp = 0 ;
	if (USTAT sb ; (rs = u_stat(termdev,&sb)) >= 0) {
	    *tp = sb.st_atime ;
	    if ((sb.st_mode & S_IWGRP) != S_IWGRP) {
	        rs = SR_RDONLY ;
	    }
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	} /* end if */
	return rs ;
}
/* end subroutine (getatime) */

static int revsortfunc(cvoid **v1pp,cvoid **v2pp) noex {
	TE		**f1pp = (TE **) v1pp ;
	TE		**f2pp = (TE **) v2pp ;
	int		rc = 0 ;
	{
	    TE	*f1p = *f1pp ;
	    TE	*f2p = *f2pp ;
	    if (f1p || f2p) {
		rc = +1 ;
	        if (f1p) {
		    rc = -1 ;
		    if (f2p) {
			rc = intsat(f2p->atime - f1p->atime) ;
		    }
		}
	    }
	} /* end block */
	return rc ;
}
/* end subroutine (revsortfunc) */


