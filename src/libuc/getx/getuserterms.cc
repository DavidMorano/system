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

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

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
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<intsat.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<vecobj.h>		/* LIBUC */
#include	<tmpx.h>		/* LIBUC */
#include	<strwcpy.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

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

local int	terment_start(TE *,cc *,int,time_t) noex ;
local int	terment_finish(TE *) noex ;

local int	getatime(cchar *,time_t *) noex ;
local int	revsortfunc(cvoid **,cvoid **) noex ;


/* local variables */

constexpr int	lline = TMPX_LLINE ;


/* exported variables */


/* exported subroutines */

int getuserterms(vecstr *lp,cchar *un) noex {
	int		rs = SR_FAULT ;
	int		n = 0 ;
	if (lp && un) ylikely {
	    rs = SR_INVALID ;
	    if (un[0]) ylikely {
		userterms	uo(un) ;
		rs = uo(lp) ;
		n = rs ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
} /* end subroutine (getuserterms) */


/* local subroutines */

int userterms::operator () (vecstr *tlp) noex {
	int		rs ;
	int		rs1 ;
	int		n = 0 ;
	if ((rs = start()) >= 0) ylikely {
	    if ((rs = proc()) >= 0) ylikely {
		n = rs ;
		rs = load(tlp) ;
	    } /* end if (vecobj_proc) */
	    rs1 = finish() ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (start-finish) */
	return (rs >= 0) ? n : rs ;
} /* end method */

int userterms::start() noex {
	cint		osz = szof(terment) ;
	cint		vn = 10 ;
	cint		vo = VECOBJ_OSORTED ;
	int		rs ;
	if ((rs = vecobj_start(&el,osz,vn,vo)) >= 0) {
	    if ((rs = lm_mp(&tbuf)) >= 0) {
		tlen = rs ;
		tl = intconv(strwcpy(tbuf,DEVDNAME) - tbuf) ;
	    } /* end if (memory-acquire) */
	    if (rs < 0) {
		vecobj_finish(&el) ;
	    } /* end if (error) */
	} /* end if (vecobj_start) */
	return rs ;
} /* end method */

int userterms::finish() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (tbuf) {
	    rs1 = lm_free(tbuf) ;
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
} /* end method */

int userterms::load(vecstr *tlp) noex {
	int		rs ;
	if ((rs = vecobj_sort(&el,revsortfunc)) >= 0) ylikely {
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
} /* end method */

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
} /* end method */

int userterms::proc() noex {
	cint		of = O_RDONLY ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (tmpx tx ; (rs = tmpx_open(&tx,nullptr,of)) >= 0) ylikely {
	    if (tmpx_cur cur ; (rs = tmpx_curbegin(&tx,&cur)) >= 0) ylikely {
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
} /* end method (userterms::proc) */

local int terment_start(TE *ep,cc *sp,int sl,time_t t) noex {
	int		rs ;
	ep->atime = t ;
	if (cchar *cp ; (rs = libmem.strw(sp,sl,&cp)) >= 0) ylikely {
	    ep->devpath = cp ;
	} /* end if (memory-acquire) */
	return rs ;
} /* end subroutine (terment_start) */

local int terment_finish(TE *ep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (ep) {
	    rs = SR_OK ;
	    if (ep->devpath) {
		void *vp = voidp(ep->devpath) ;
	        rs1 = libmem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->devpath = nullptr ;
	    } /* end if (memory-release) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (terment_finish) */

local int getatime(cc *termdev,time_t *tp) noex {
	int		rs ;
	*tp = 0 ;
	if (ustat sb ; (rs = u_stat(termdev,&sb)) >= 0) {
	    *tp = sb.st_atime ;
	    if ((sb.st_mode & S_IWGRP) != S_IWGRP) {
	        rs = SR_RDONLY ;
	    }
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	} /* end if */
	return rs ;
} /* end subroutine (getatime) */

local int revsortfunc(cvoid **v1pp,cvoid **v2pp) noex {
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
} /* end subroutine (revsortfunc) */


