/* tardir1 MODULE (module-interface-unit) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* enumerate filenames */
/* version %I% last-modified %G% */


/* revision history:

	= 2017-03-21, David A-D- Morano
	This subroutine was originally written.

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	tardir

*******************************************************************************/

module ;

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdio>
#include	<new>			/* |nothrow(3c++)| */
#include	<usyscalls.h>		/* |u_stat(3u)| */
#include	<strnul.hh>
#include	<strn.h>
#include	<sfx.h>
#include	<six.h>
#include	<strwcpy.h>
#include	<localmisc.h>
#include	<dprintf.hh>		/* debugging */

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/sif.ccm"
#pragma		GCC dependency		"mod/fonce.ccm"

module tardir ;

import libutil ;			/* |lenstr(3u)| */
import sif ;
import fonce ;

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */


/* local typedefs */

typedef fonce *		foncep ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local int tardir_ctor(tardir *op,Args ... args) noex {
	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (... && args)) {
	    rs = SR_NOMEM ;
	    if (vecent *dlp ; (dlp = new(nt) vecent) != np) {
		op->dirp = dlp ;
	        if (fonce *slp ; (slp = new(nt) fonce) != np) {
		    op->seenp = slp ;
		    rs = SR_OK ;
	        } /* end if (new-fonce) */
	        if (rs < 0) {
		    delete dlp ;
		    op->dirp = nullptr ;
	        }
	    } /* end if (new-vecent) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (tardir_ctor) */

local int tardir_dtor(tardir *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->seenp) {
		fonce *p = foncep(op->seenp) ;
		delete p ;
		op->seenp = nullptr ;
	    }
	    if (op->dirp) {
		vecent *p = vecentp(op->dirp) ;
		delete p ;
		op->dirp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (tardir_dtor) */


/* local variables */


/* exported variables */


/* exported subroutines */


/* local subroutines */

int tardir::istart() noex {
    	int		rs ;
	if ((rs = tardir_ctor(this)) >= 0) {
	    vecent *tlp = vecentp(dirp) ;
	    if ((rs = tlp->start) >= 0) ylikely {
	        fonce *slp = foncep(seenp) ;
	        if ((rs = slp->start) >= 0) ylikely {
		    fl.open = true ;
		    magval = tardir_magicval ;
	        }
	        if (rs < 0) {
		    tlp->finish() ;
	        }
	    } /* end if (dirs.start) */
	    if (rs < 0) {
		tardir_dtor(this) ;
	    }
	} /* end if (tardir_ctor) */
	return rs ;
} /* end method (tardir::istart) */

int tardir::ifinish() noex {
    	int		rs ;
	int		rs1 ;
	if ((rs = magic) >= 0) ylikely {
	    if (seenp) {
	        fonce *slp = foncep(seenp) ;
		rs1 = slp->finish ;
		if (rs >= 0) rs = rs1 ;
	    }
	    if (dirp) {
	        vecent *tlp = vecentp(dirp) ;
		rs1 = tlp->finish() ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = tardir_dtor(this) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    magval = 0 ;
	} /* end if (tardir_magic) */
	return rs ;
} /* end method (tardir::ifinish) */

int tardir::icount() noex {
    	int		rs ;
	int		c = 0 ;
	if ((rs = magic) >= 0) ylikely {
	    vecent *dlp = vecentp(dirp) ;
	    rs = dlp->count ;
	    c = rs ;
	} /* end if (tardir_magic) */
	return (rs >= 0) ? c : rs ;
} /* end method (tardir::icount) */

int tardir::add(cchar *sp,int sl) noex {
    	int		rs ;
	int		c = 0 ;
	if ((rs = magic(sp)) >= 0) ylikely {
	    sif so(sp,sl,':') ;
	    cchar *cp ;
	    for (int cl ; (cl = so(&cp)) > 0 ; ) {
	        strnul dn(cp,cl) ; 
	        if (ustat sb ; (rs = u_stat(dn,&sb)) >= 0) {
		    rs = SR_NOTDIR ;
		    if (S_ISDIR(sb.st_mode)) {
		        rs = iaddone(&sb,sp,sl) ;
			c += rs ;
		    }
	        } /* end if (u_stat) */
		if (rs < 0) break ;
	    } /* end for */
	} /* end if (tardir_magic) */
	return (rs >= 0) ? c : rs ;
} /* end method (tardir::add) */

int tardir::get(int idx,ccharpp rpp) noex {
    	int		rs ;
	if ((rs = magic) >= 0) ylikely {
	    vecent *dlp = vecentp(dirp) ;
	    rs = dlp->get(idx,nullptr,rpp) ;
	} /* end if (tardir_magic) */
	return (rs >= 0) ? idx : rs ;
} /* end method (tardir::get) */

int tardir::get(int idx,ustat *sbp,ccharpp rpp) noex {
    	int		rs ;
	if ((rs = magic) >= 0) ylikely {
	    vecent *dlp = vecentp(dirp) ;
	    rs = dlp->get(idx,sbp,rpp) ;
	} /* end if (tardir_magic) */
	return (rs >= 0) ? idx : rs ;
} /* end method (tardir::get) */

int tardir::curbegin(tardir_cur *curp) noex {
    	int		rs ;
	if ((rs = magic(curp)) >= 0) ylikely {
	    curp->i = -1 ;
	    rs = SR_OK ;
	} /* end if (tardir_magic) */
	return rs ;
} /* end method (tardir::curbegin) */

int tardir::curend(tardir_cur *curp) noex {
    	int		rs ;
	if ((rs = magic(curp)) >= 0) ylikely {
	    curp->i = -1 ;
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
} /* end method (tardir::curend) */

int tardir::curenum(tardir_cur *curp,ccharpp rpp) noex {
	cint		rsn = SR_NOTFOUND ;
    	int		rs ;
	int		rl = 0 ; /* return-value */
	if ((rs = magic(curp,rpp)) >= 0) ylikely {
		cint idx = (curp->i >= 0) ? (curp->i + 1) : 0 ;
	        if ((rs = get(idx,rpp)) >= 0) {
		    curp->i = idx ;
		    rl = lenstr(*rpp) ;
		} else if (rs == rsn) {
		    rs = SR_OK ;
		}
	} /* end if (tardir_magic) */
	return (rs >= 0) ? rl : rs ;
} /* end method (tardir::curenum) */

int tardir::iaddone(const ustat *sbp,cchar *sp,int sl) noex {
    	int		rs ;
	int		c = 0 ;
	{
	    strnul s(sp,sl) ;
	    DPRINTF("ent s=%s\n",ccp(s)) ;
	}
	if ((rs = magic(sbp,sp)) >= 0) ylikely {
	    fonce *slp = foncep(seenp) ;
	    if ((rs = slp->checkin(sbp)) > 0) ylikely {
	        vecent *dlp = vecentp(dirp) ;
	        rs = dlp->add(sbp,sp,sl) ;
	        c = rs ;
	    } /* end if (ssen.checkin) */
	} /* end if (tardir_magic) */
	DPRINTF("ret rs=%d c=%d\n",rs,c) ;
	return (rs >= 0) ? c : rs ;
} /* end method (tardir::iaddone) */

void tardir::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("tardir",rs,"fini-finish") ;
	}
} /* end method (tardir::dtor) */

tardir::operator int () noex {
	return icount() ;
}

int tardir_co::operator () (int) noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case tardirmem_start:
	        rs = op->istart() ;
	        break ;
	    case tardirmem_count:
	        rs = op->icount() ;
	        break ;
	    case tardirmem_finish:
	        rs = op->ifinish() ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (tardir_co::operator) */


