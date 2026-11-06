/* vecfnmat1 MODULE (implementation -- Vector-Regular-Expression) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* vector of regular expressions */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 1998-09-16, David A­D­ Morano
	This module was originally written for RightCore 
	Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	vecfnmat

	Description:
	This object attempts to implement a sort of variable-length
	array of elements.

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<fnmatch.h>		/* CSTD */
#include	<new>			/* C++STD placement-new */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ulogerror.h>		/* LIBU */
#include	<strnul.hh>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<dprint.hh>		/* LIBU |DPRINTF(3u)| */

#include	"libutil.hh"
#include	"vecfnmat.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

module vecfnmat ;

import libutil ;			/* |memclear(3u)| */
import ureserve ;			/* |vecstr(3u)| */

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local inline int vecfnmat_ctor(vecfnmat *op,Args ... args) noex {
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    if (vecstr *slp = new(nt) vecstr) ylikely {
		op->slp = slp ;
		rs = SR_OK ;
	    } /* end if (new-vecstr) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (vecfnmat_ctor) */

local inline int vecfnmat_dtor(vecfnmat *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_BUGCHECK ;
	    if (vecstr *slp = resumelife<vecstr>(op->slp)) ylikely {
		delete slp ;
		op->slp = nullptr ;
		rs = SR_OK ;
	    } /* end if (delete-vecstr) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (vecfnmat_dtor) */


/* local variables */

cbool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int vecfnmat_start(vecfnmat *op) noex {
	int		rs ;
	if ((rs = vecfnmat_ctor(op)) >= 0) ylikely {
	    if (vecstr *slp = resumelife<vecstr>(op->slp)) ylikely {
		if ((rs = slp->start) >= 0) {
		    op->fl.sopen = true ;
		    op->magval = VECFNMAT_MAGIC ;
		} /* end */
	    } /* end if (non-null) */
	    if (rs < 0) {
		vecfnmat_dtor(op) ;
	    } /* end if (error) */
	} /* end if (vecfnmat_ctor) */
	return rs ;
} /* end subroutine (vecfnmat_start) */

int vecfnmat_finish(vecfnmat *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) ylikely {
	    rs = SR_NOTOPEN ;
	    if (vecstr *slp = resumelife<vecstr>(op->slp)) ylikely {
		rs = SR_OK ;
		if (op->fl.sopen) {
	    	    op->fl.sopen = false ;
	            rs1 = slp->finish ;
	            if (rs >= 0) rs = rs1 ;
		} /* end */
	    } /* end if */
	    {
		rs1 = vecfnmat_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end */
	    op->magval = 0 ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (vecfnmat_finish) */

int vecfnmat_add(vecfnmat *op,ccharp sp,int sl) noex {
    	int		rs = SR_FAULT ;
	if (op && sp) ylikely {
	    rs = SR_NOTOPEN ;
	    if (vecstr *slp = resumelife<vecstr>(op->slp)) ylikely {
		rs = slp->add(sp,sl) ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (vecfnmat_add) */

int vecfnmat_addfile(vecfnmat *op,ccharp fn) noex {
    	int		rs = SR_FAULT ;
	if (op && fn) ylikely {
	    rs = SR_INVALID ;
	    if (fn[0]) ylikely {
	        rs = SR_NOTOPEN ;
	        if (vecstr *slp = resumelife<vecstr>(op->slp)) ylikely {
		    rs = slp->addstrfile(fn) ;
	        } /* end if (open) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (vecfnmat_addfile) */

int vecfnmat_enumer(vecfnmat *op,int i,ccharpp rpp) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_NOTOPEN ;
	    if (vecstr *slp = resumelife<vecstr>(op->slp)) ylikely {
		rs = slp->get(i,rpp) ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (vecfnmat_enumer) */

int vecfnmat_del(vecfnmat *op,int i) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_NOTOPEN ;
	    if (vecstr *slp = resumelife<vecstr>(op->slp)) ylikely {
		if (i >= 0) {
		    rs = slp->del(i) ;
		} else {
		    rs = slp->delall ;
		} /* end */
	    } /* end if (non-null) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (vecfnmat_del) */

int vecfnmat_delall(vecfnmat *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_NOTOPEN ;
	    if (vecstr *slp = resumelife<vecstr>(op->slp)) ylikely {
		rs = slp->delall ;
	    } /* end */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (vecfnmat_delall) */

int vecfnmat_count(vecfnmat *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    if (vecstr *slp = resumelife<vecstr>(op->slp)) ylikely {
		rs = slp->count ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (vecfnmat_count) */

int vecfnmat_match(vecfnmat *op,cchar *sp,int sl,int fnmfl) noex {
	int		rs = SR_FAULT ;
	int		f = false ; /* return-value */
	DPRINTF("ent\n") ;
	if_constexpr (f_debug) {
	    if (strnul ds(sp,sl) ; ds.fok) {
	        DPRINTF("s=»%s«\n",ccp(ds)) ;
	    }
	}
	if (op && sp) ylikely {
	    rs = SR_NOTOPEN ;
	    if (vecstr *slp = resumelife<vecstr>(op->slp)) ylikely {
		ccharp cp ;
		if (int cl ; (cl = sfbasename(sp,sl,&cp)) > 0) {
		    if (strnul cs(cp,cl) ; cs.fok) {
			int	fnm = FNM_RETNOMATCH ; /* used-afterwards */
		        ccharp str = cs ;
		        ccharp pp ;
		        rs = SR_OK ;
		        for (int i = 0 ; slp->get(i,&pp) >= 0 ; i += 1) {
	        	    DPRINTF("p=»%s«\n",pp) ;
			    fnm = fnmatch(pp,str,fnmfl) ;
			    if (fnm != FNM_RETNOMATCH) break ;
		        } /* end for */
			switch (fnm) {
			case FNM_RETMATCH:
			    f = true ;
			    break ;
			case FNM_RETNOMATCH:
			    break ;
			case FNM_RETNOSYS:
			    rs = SR_NOSYS ;
			    break ;
			case FNM_RETERROR:
			    rs = SR_BADRQC ;
			    break ;
			default:
			    rs = SR_BADFMT ;
			    break ;
			} /* end switch (resolution) */
		    } /* end if (strnul) */
		} /* end if (sfbasename) */
	    } /* end if (open) */
	} /* end if (non-null) */
	DPRINTF("ret rs=%d f=%u\n",rs,f) ;
	return (rs >= 0) ? f : rs ;
} /* end subroutine (vecfnmat_match) */

int vecfnmat_audit(vecfnmat *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_NOTOPEN ;
	    if (vecstr *slp = resumelife<vecstr>(op->slp)) ylikely {
		rs = slp->audit ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (vecfnmat_audit) */


/* local subroutines */

int vecfnmat::enumer(int ii,ccharpp rpp) noex {
	return vecfnmat_enumer(this,ii,rpp) ;
} /* end method */

int vecfnmat::add(ccharp sp,int sl) noex {
	return vecfnmat_add(this,sp,sl) ;
} /* end method */

int vecfnmat::addfile(ccharp fn) noex {
	return vecfnmat_addfile(this,fn) ;
} /* end method */

int vecfnmat::match(ccharp sp,int sl,int fnmfl) noex {
	return vecfnmat_match(this,sp,sl,fnmfl) ;
} /* end method */

void vecfnmat::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("vecfnmat",rs,"fini-finish") ;
	} /* end */
} /* end method (vecfnmat::dtor) */

vecfnmat::operator int () noex {
    	return vecfnmat_count(this) ;
} /* end method (vecfnmat::operator) */

int vecfnmat_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case vecfnmatmem_start:
	        rs = vecfnmat_start(op) ;
	        break ;
	    case vecfnmatmem_count:
	        rs = vecfnmat_count(op) ;
	        break ;
	    case vecfnmatmem_del:
	        rs = vecfnmat_del(op,a) ;
	        break ;
	    case vecfnmatmem_delall:
	        rs = vecfnmat_delall(op) ;
	        break ;
	    case vecfnmatmem_audit:
	        rs = vecfnmat_audit(op) ;
	        break ;
	    case vecfnmatmem_finish:
	        rs = vecfnmat_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (vecfnmat_co::operator) */


