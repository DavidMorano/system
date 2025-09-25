/* mailfiles SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* manage mail files (mailboxes) */
/* version %I% last-modified %G% */

#define	CF_MAILBOXZERO	1

/* revision history:

	= 1988-02-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1988 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

  	Name:
	mailfiles

	Description:
	This object module is used to manage a set of mail files.

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |memset(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<new>			/* |nothrow(3C++)| */
#include	<usystem.h>
#include	<vecobj.h>
#include	<strwcpy.h>
#include	<strn.h>
#include	<sfx.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"mailfiles.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| */

/* local defines */

#define	MF		mailfiles
#define	MF_ENT		mailfiles_ent


/* local namespaces */

using libuc::libmem ;			/* variable */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int mailfiles_ctor(MF *op,Args ... args) noex {
    	MAILFILES	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->elp = new(nothrow) vecobj) != np) {
		rs = SR_OK ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailfiles_ctor) */

static int mailfiles_dtor(MF *op) noex {
	int		rs ;
	if ((rs = mailfiles_magic(op)) >= 0) {
	    if (op->elp) {
		delete op->elp ;
		op->elp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailfiles_dtor) */

static int	entry_start(MF_ENT *,cchar *,int) noex ;
static int	entry_finish(MF_ENT *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int mailfiles_start(MF *op) noex {
	int		rs ;
	if ((rs = mailfiles_ctor(op)) >= 0) {
	    vecobj	*elp = op->elp ;
	    cint	vsz = szof(MF_ENT) ;
	    cint	vn = 4 ;
	    cint	vo = VECOBJ_OCOMPACT ;
	    if ((rs = elp->start(vsz,vn,vo)) >= 0) {
		op->magic = MAILFILES_MAGIC ;
	    }
	    if (rs < 0) {
		mailfiles_dtor(op) ;
	    }
	} /* end if (mailfiles_ctor) */
	return rs ;
}
/* end subroutine (mailfiles_start) */

int mailfiles_finish(MF *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = mailfiles_magic(op)) >= 0) {
	    vecobj	*elp = op->elp ;
	    if (elp) {
	        void	*vp{} ;
	        for (int i = 0 ; elp->get(i,&vp) >= 0 ; i += 1) {
	            MF_ENT	*ep = (MF_ENT *) vp ;
	            if (vp) {
	                rs1 = entry_finish(ep) ;
	                if (rs >= 0) rs = rs1 ;
	            }
	        } /* end for */
	        {
	            rs1 = vecobj_finish(elp) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	    }
	    {
		rs1 = mailfiles_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailfiles_finish) */

int mailfiles_add(MF *op,cchar *sp,int sl) noex {
    	int		rs ;
	if ((rs = mailfiles_magic(op,sp)) >= 0) {
	    cchar	*cp ;
	    if (cint cl = sfshrink(sp,sl,&cp) ; cl > 0) {
		if (MF_ENT e ; (rs = entry_start(&e,cp,cl)) >= 0) {
		    vecobj	*elp = op->elp ;
		    rs = elp->add(&e) ;
		    if (rs < 0) {
			entry_finish(&e) ;
		    }
		} /* end if (entry_start) */
	    } /* end if (sfshrink) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailfiles_add) */

int mailfiles_addpath(MF *op,cchar *sp,int sl) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		n = 0 ;
	if ((rs = mailfiles_magic(op,sp)) >= 0) {
	    if (cchar *tp ; (tp = strnrchr(sp,sl,'?')) != np) {
	        sl -= intconv((sp + sl) - tp) ;
	    }
	    for (cc *tp ; (tp = strnchr(sp,sl,':')) != np ; ) {
		if ((tp - sp) > 0) {
		    cint	ll = intconv(tp - sp) ;
	            rs = mailfiles_add(op,sp,ll) ;
	    	    n += (rs < INT_MAX) ;
		}
	        sl -= intconv((tp + 1) - sp) ;
	        sp = (tp + 1) ;
		if (rs < 0) break ;
	    } /* end for */
	    if ((rs >= 0) && (sl > 0)) {
	        rs = mailfiles_add(op,sp,sl) ;
	    	n += (rs < INT_MAX) ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (mailfiles_addpath) */

int mailfiles_get(MF *op,int i,MF_ENT **epp) noex {
	int		rs ;
	if ((rs = mailfiles_magic(op,epp)) >= 0) {
	    vecobj	*elp = op->elp ;
	    if (void *vp ; (rs = elp->get(i,&vp)) >= 0) {
		*epp = (MF_ENT *) vp ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailfiles_get) */

int mailfiles_count(MF *op) noex {
	int		rs ;
	if ((rs = mailfiles_magic(op)) >= 0) {
	    vecobj	*elp = op->elp ;
	    rs = elp->count ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailfiles_count) */

int mailfiles_check(MF *op) noex {
	int		rs ;
	int		nchg = 0 ;
	if ((rs = mailfiles_magic(op)) >= 0) {
	    vecobj	*elp = op->elp ;
	    void	*vp{} ;
	    for (int i = 0 ; (rs = elp->get(i,&vp)) >= 0 ; i += 1) {
	        MF_ENT	*ep = (MF_ENT *) vp ;
		if (vp) {
	            if (! ep->f_changed) {
		        cchar	*fn = ep->mailfname ;
	                if (USTAT sb ; (rs = u_stat(fn,&sb)) >= 0) {
	                    if (sb.st_size > ep->lastsize) {
	        		ep->lasttime = sb.st_mtime ;
	        		ep->lastsize = sb.st_size ;
	                        ep->f_changed = true ;
	                        nchg += 1 ;
			    }
			} else if (isNotPresent(rs)) {
	                    ep->f_changed = true ;
			    nchg += 1 ;
			    rs = SR_OK ;
			}
	            } else {
			nchg += 1 ;
		    }
		} /* end if (non-null) */
		if (rs < 0) break ;
	    } /* end for */
	} /* end if (magic) */
	return (rs >= 0) ? nchg : rs ;
}
/* end subroutine (mailfiles_check) */


/* local subroutines */

static int entry_start(MF_ENT *ep,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (ep && sp) {
	    memclear(ep) ;
	    ep->f_changed = false ;
	    if (cchar *cp ; (rs = libmem.strw(sp,sl,&cp)) >= 0) {
	        ep->mailfname = cp ;
		if (ustat sb ; (rs = u_stat(cp,&sb)) >= 0) {
		    ep->lasttime = sb.st_mtime ;
		    ep->lastsize = sb.st_size ;
		}
		if (rs < 0) {
		    void *vp = voidp(ep->mailfname) ;
		    libmem.free(vp) ;
		    ep->mailfname = nullptr ;
		} /* end if (error) */
	    } /* end if (memory-allocation) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entry_start) */

static int entry_finish(MF_ENT *ep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (ep->mailfname) {
	    void *vp = voidp(ep->mailfname) ;
	    rs1 = libmem.free(vp) ;
	    if (rs >= 0) rs = rs1 ;
	    ep->mailfname = nullptr ;
	}
	return rs ;
}
/* end subroutine (entry_finish) */


