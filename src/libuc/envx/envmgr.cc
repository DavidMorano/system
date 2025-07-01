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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<new>			/* |nothrow(3c++)| */
#include	<usystem.h>
#include	<estrings.h>
#include	<sfx.h>
#include	<vstrkeycmpx.h>
#include	<localmisc.h>

#include	"envmgr.h"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */

extern cchar	**environ ;


/* local structures */


/* forward references */

template<typename ... Args>
static int envmgr_ctor(envmgr *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    if ((op->listp = new(nothrow) vechand) != np) {
	        if ((op->strp = new(nothrow) vecstr) != np) {
		    rs = SR_OK ;
		}
		if (rs < 0) {
		    delete op->listp ;
		    op->listp = nullptr ;
		}
	    } /* end if (new-vecstr) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (envmgr_ctor) */

static int envmgr_dtor(envmgr *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->listp) {
		delete op->listp ;
		op->listp = nullptr ;
	    }
	    if (op->strp) {
		delete op->strp ;
		op->strp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (envmgr_dtor) */


/* local variables */


/* exported variables */


/* exported subroutines */

int envmgr_start(envmgr *op) noex {
	int		rs ;
	if ((rs = envmgr_ctor(op)) >= 0) {
	    vechand	*elp = op->listp ;
	    cint	vn = 10 ;
	    cint	vo = (VECHAND_OCOMPACT | VECHAND_OSORTED) ;
	    if ((rs = vechand_start(elp,vn,vo)) >= 0) {
	        vecstr		*slp = op->strp ;
	        if ((rs = vecstr_start(slp,2,0)) >= 0) {
		    for (int i = 0 ; (rs >= 0) && environ[i] ; i += 1) {
	    	        rs = vechand_add(elp,environ[i]) ;
		    }
	        }
	        if (rs < 0) {
	            vechand_finish(op->listp) ;
	        }
	    } /* end if (vechand_start) */
	    if (rs < 0) {
		envmgr_dtor(op) ;
	    }
	} /* end if (envmgr_ctor) */
	return rs ;
}
/* end subroutine (envmgr_start) */

int envmgr_finish(envmgr *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->strp) {
	        rs1 = vecstr_finish(op->strp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->listp) {
	        rs1 = vechand_finish(op->listp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = envmgr_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (envmgr_finish) */

int envmgr_set(envmgr *op,cchar *kp,cchar *vp,int vl) noex {
	int		rs = SR_FAULT ;
	if (op && kp) {
	    vecstr	*esp = op->strp ;
	    if ((rs = vecstr_envset(esp,kp,vp,vl)) >= 0) {
	        vechand		*elp = op->listp ;
	        cint		i = rs ;
	        if (cchar *ep{} ; (rs = vecstr_get(esp,i,&ep)) >= 0) {
		    vechand_vcmp	vcf = vechand_vcmp(vstrkeycmp) ;
		    cnullptr		np{} ;
		    cint		nrs = SR_NOTFOUND ;
	            if ((rs = vechand_search(elp,kp,vcf,np)) >= 0) {
	                vechand_del(elp,rs) ;
	            } else if (rs == nrs) {
		        rs = SR_OK ;
		    }
	            if (rs >= 0) {
		        rs = vechand_add(elp,ep) ;
	            }
	        } /* end if (vecstr_get) */
	    } /* end if (vecstr_envset) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (envmgr_set) */

int envmgr_getvec(envmgr *op,cchar ***rppp) noex {
	int		rs = SR_FAULT ;
	if (op && rppp) {
	    vechand	*elp = op->listp ;
	    rs = vechand_getvec(elp,rppp) ;
	}
	return rs ;
}
/* end subroutine (envmgr_getvec) */


