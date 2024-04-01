/* envmgr SUPPORT */
/* lang=C++20 */

/* Environment Manager */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-18, David A­D­ Morano
	This object module was first written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Manage an environment ensemble.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<estrings.h>
#include	<sfx.h>
#include	<vstrkeycmpx.h>
#include	<localmisc.h>

#include	"envmgr.h"


/* local defines */


/* external subroutines */

using std::nullptr_t ;			/* type */


/* external variables */

extern cchar	**environ ;


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int envmgr_start(envmgr *emp) noex {
	int		rs = SR_FAULT ;
	if (emp) {
	    vechand	*elp = &emp->envlist ;
	    cint	vo = (VECHAND_OCOMPACT | VECHAND_OSORTED) ;
	    if ((rs = vechand_start(elp,10,vo)) >= 0) {
	        vecstr		*slp = &emp->envstrs ;
	        if ((rs = vecstr_start(slp,2,0)) >= 0) {
		    for (int i = 0 ; (rs >= 0) && environ[i] ; i += 1) {
	    	        rs = vechand_add(elp,environ[i]) ;
		    }
	        }
	        if (rs < 0) {
	            vechand_finish(&emp->envlist) ;
	        }
	    } /* end if (vechand_start) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (envmgr_start) */

int envmgr_finish(envmgr *emp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (emp) {
	    rs = SR_OK ;
	    {
	        rs1 = vecstr_finish(&emp->envstrs) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = vechand_finish(&emp->envlist) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (envmgr_finish) */

int envmgr_set(envmgr *emp,cchar *kp,cchar *vp,int vl) noex {
	int		rs = SR_FAULT ;
	if (emp && kp) {
	    vecstr	*esp = &emp->envstrs ;
	    if ((rs = vecstr_envset(esp,kp,vp,vl)) >= 0) {
	        vechand	*elp = &emp->envlist ;
	        cint	i = rs ;
	        cchar	*ep ;
	        if ((rs = vecstr_get(esp,i,&ep)) >= 0) {
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

int envmgr_getvec(envmgr *emp,cchar ***rppp) noex {
	int		rs = SR_FAULT ;
	if (emp && rppp) {
	    vechand	*elp = &emp->envlist ;
	    rs = vechand_getvec(elp,rppp) ;
	}
	return rs ;
}
/* end subroutine (envmgr_getvec) */


