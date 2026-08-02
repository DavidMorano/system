/* envmgr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Environment Manager */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-18, David A­D­ Morano
	This object module was first written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	envmgr

	Description:
	Manage an environment ensemble.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<new>			/* C++STD |nothrow(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<sfx.h>			/* LIBUC */
#include	<vstrkeycmp.h>		/* LIBUC |vstrkeycmp(3uc)| */
#include	<localmisc.h>		/* LIBU */

#include	"envmgr.h"


/* local defines */


/* imported namespaces */

using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local int envmgr_ctor(envmgr *op,Args ... args) noex {
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    if ((op->listp = new(nothrow) vechand) != np) ylikely {
	        if ((op->strp = new(nothrow) vecstr) != np) ylikely {
		    rs = SR_OK ;
		}
		if (rs < 0) {
		    delete op->listp ;
		    op->listp = nullptr ;
		} /* end if (error) */
	    } /* end if (new-vecstr) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (envmgr_ctor) */

local int envmgr_dtor(envmgr *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->listp) ylikely {
		delete op->listp ;
		op->listp = nullptr ;
	    }
	    if (op->strp) ylikely {
		delete op->strp ;
		op->strp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (envmgr_dtor) */

local int envmgr_envv(envmgr *op,mainv ev) noex {
    	int	rs = SR_OK ;
	if ((op->envv = ev) == nullptr) {
	    if ((rs = u_getenviron(&ev)) >= 0) {
		op->envv = ev ;
	    }
	}
	return rs ;
} /* end subroutine (envmgr_envv) */


/* local variables */


/* exported variables */


/* exported subroutines */

int envmgr_start(envmgr *op) noex {
	int		rs ;
	if ((rs = envmgr_ctor(op)) >= 0) ylikely {
	    if ((rs = envmgr_envv(op,nullptr)) >= 0) ylikely {
	        vechand	*elp = op->listp ;
	        cint	vn = 10 ;
	        cint	vo = (vechandm.compact | vechandm.sorted) ;
	        if ((rs = elp->start(vn,vo)) >= 0) ylikely {
	            vecstr *slp = op->strp ;
	            if ((rs = slp->start(2,0)) >= 0) ylikely {
		        for (int i = 0 ; (rs >= 0) && op->envv[i] ; i += 1) {
	    	            rs = elp->add(op->envv[i]) ;
		        }
		        if (rs < 0) {
			    slp->finish() ;
		        }
	            } /* end if (vecstr_start) */
	            if (rs < 0) {
	                elp->finish() ;
	            }
	        } /* end if (vechand_start) */
	        if (rs < 0) {
		    envmgr_dtor(op) ;
	        }
	    } /* end if (envmgr_envv) */
	} /* end if (envmgr_ctor) */
	return rs ;
} /* end subroutine (envmgr_start) */

int envmgr_finish(envmgr *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->strp) ylikely {
	        rs1 = vecstr_finish(op->strp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->listp) ylikely {
	        rs1 = vechand_finish(op->listp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = envmgr_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (envmgr_finish) */

int envmgr_set(envmgr *op,cchar *kp,cchar *vp,int vl) noex {
	cnullptr	np{} ;
	cint		nrs = SR_NOTFOUND ;
	int		rs = SR_FAULT ;
	if (op && kp) ylikely {
	    vecstr	*esp = op->strp ;
	    if ((rs = vecstr_envset(esp,kp,vp,vl)) >= 0) ylikely {
	        vechand		*elp = op->listp ;
	        cint		i = rs ;
	        if (cchar *ep ; (rs = vecstr_get(esp,i,&ep)) >= 0) ylikely {
		    vechand_vcmp	vcf = vechand_vcmp(vstrkeycmp) ;
	            if ((rs = vechand_search(elp,kp,vcf,np)) >= 0) {
	                vechand_del(elp,rs) ;
	            } else if (rs == nrs) {
		        rs = SR_OK ;
		    }
	            if (rs >= 0) {
		        rs = vechand_add(elp,ep) ;
	            } /* end if (ok) */
	        } /* end if (vecstr_get) */
	    } /* end if (vecstr_envset) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (envmgr_set) */

int envmgr_getvec(envmgr *op,cchar ***rppp) noex {
	int		rs = SR_FAULT ;
	if (op && rppp) ylikely {
	    vechand	*elp = op->listp ;
	    rs = vechand_getvec(elp,rppp) ;
	}
	return rs ;
} /* end subroutine (envmgr_getvec) */


