/* envhelp SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* help w/ environment */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	envhelp_start
	envhelp_finish

	Description:
	This little honey helps daddy management the environment
	list for launching new programs.

	Synopsis:
	int envhelp_start(envhelp *op,mainv envbads,mainv envv) noex

	Arguments:
	op		object pointer
	envbads		list of environment variables that are not included
	envv		environment array

	Returns:
	>=0		OK
	<0		error (system-return)

	Synopsis:
	int envhelp_finish(envhelp *op) noex

	Arguments:
	op		object pointer

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<usystem.h>
#include	<nulstr.h>
#include	<strwcpy.h>
#include	<matkeystr.h>
#include	<vstrkeycmpx.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"envhelp.h"


/* local defines */

#define	NENVS		80		/* default (start) number entries */
#define	DEFENVSTORESIZE	120


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */

extern cchar	**environ ; /* what is should be  */


/* forward reference */

template<typename ... Args>
static int envhelp_ctor(envhelp *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    if ((op->elp = new(nothrow) vechand) != np) {
	        if ((op->spp = new(nothrow) strpack) != np) {
		    rs = SR_OK ;
		} /* end if (new-strpack) */
	        if (rs < 0) {
		    delete op->elp ;
		    op->elp = nullptr ;
		}
	    } /* end if (new_vechand) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (envhelp_ctor) */

static int envhelp_dtor(envhelp *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->spp) {
		delete op->spp ;
		op->spp = nullptr ;
	    }
	    if (op->elp) {
		delete op->elp ;
		op->elp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (envhelp_dtor) */

static int	envhelp_copy(envhelp *,mainv,mainv) noex ;

static int	vechand_addover(vechand *,cchar *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int envhelp_start(envhelp *op,mainv envbads,mainv envv) noex {
	cint		vo = (VECHAND_OCOMPACT | VECHAND_OSORTED) ;
	int		rs ;
	if (envv == nullptr) envv = environ ;
	if ((rs = envhelp_ctor(op)) >= 0) {
	    cint	vn = NENVS ;
	    if ((rs = vechand_start(op->elp,vn,vo)) >= 0) {
	        cint	size = DEFENVSTORESIZE ;
	        if ((rs = strpack_start(op->spp,size)) >= 0) {
		    if (envv != nullptr) {
	    	        rs = envhelp_copy(op,envbads,envv) ;
		    }
	            if (rs < 0) {
		        strpack_finish(op->spp) ;
		    }
	        } /* end if (strpack_start) */
	        if (rs < 0) {
		    vechand_finish(op->elp) ;
	        }
	    } /* end if (vechand_start) */
	    if (rs < 0) {
		envhelp_dtor(op) ;
	    }
	} /* end if (envhelp_ctor) */
	return rs ;
}
/* end subroutine (envhelp_start) */

int envhelp_finish(envhelp *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    {
	        rs1 = strpack_finish(op->spp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = vechand_finish(op->elp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = envhelp_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (envhelp_finish) */

int envhelp_envset(envhelp *op,cchar *kp,cchar *vp,int vl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		i = INT_MAX ;
	if (op && kp) {
	    vechand	*elp = op->elp ;
	    int		elen = 1 ; /* for the equals sign character */
	    elen += strlen(kp) ;
	    if (vp) {
	        if (vl < 0) vl = strlen(vp) ;
	        elen += strnlen(vp,vl) ;
	    }
	    if (char *ebuf{} ; (rs = uc_malloc((elen+1),&ebuf)) >= 0) {
	        cchar		*ep{} ;
	        char		*bp = ebuf ;
	        bp = strwcpy(bp,kp,-1) ;
	        *bp++ = '=' ;
	        if (vp) {
		    bp = strwcpy(bp,vp,vl) ;
		}
	        if ((rs = strpack_store(op->spp,ebuf,(bp-ebuf),&ep)) >= 0) {
		    rs = vechand_addover(elp,ep) ;
		    i = rs ;
	        } /* end if */
	        rs1 = uc_free(ebuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (memory-allocation) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (envhelp_envset) */

int envhelp_present(envhelp *op,cchar *kp,int kl,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		i = 0 ;
	if (op && kp) {
	    cchar	*cp{} ;
	    if (nulstr ks ; (rs = ks.start(kp,kl,&cp)) >= 0) {
	        vechand_vcmp	vcf = vechand_vcmp(vstrkeycmp) ;
	        vechand		*elp = op->elp ;
	        void		*vp{} ;
	        if ((rs = vechand_search(elp,cp,vcf,&vp)) >= 0) {
		    if (rpp) {
			*rpp = charp(vp) ;
		    }
	            i = rs ;
	        } /* end if (vechand_search) */
	        rs1 = ks.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (nulstr) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (envhelp_present) */

int envhelp_sort(envhelp *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    vechand_vcmp	vcf = vechand_vcmp(vstrkeycmp) ;
	    rs = vechand_sort(op->elp,vcf) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (envhelp_sort) */

int envhelp_count(envhelp *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = vechand_count(op->elp) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (envhelp_count) */

int envhelp_getvec(envhelp *op,cchar ***eppp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    vechand_vcmp	vcf = vechand_vcmp(vstrkeycmp) ;
	    vechand		*elp = op->elp ;
	    if ((rs = vechand_sort(elp,vcf)) >= 0) {
	        if (eppp) {
	            rs = vechand_getvec(elp,eppp) ;
	        } else {
	            rs = vechand_count(elp) ;
	        }
	    } /* end if (vechand_sort) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (envhelp_getvec) */


/* private subroutines */

static int envhelp_copy(envhelp *op,mainv envbads,mainv envv) noex {
	vechand		*elp = op->elp ;
	int		rs = SR_OK ;
	int		n = 0 ;
	if (envv != nullptr) {
	    cchar	*ep{} ;
	    if (envbads != nullptr) {
	        for (int i = 0 ; (rs >= 0) && envv[i] ; i += 1) {
	            ep = envv[i] ;
	            if (matkeystr(envbads,ep,-1) < 0) {
	                n += 1 ;
	                rs = vechand_add(elp,ep) ;
	            } /* end if (was not bad) */
	        } /* end for (copy ENVs) */
	    } else {
	        for (int i = 0 ; (rs >= 0) && envv[i] ; i += 1) {
	            ep = envv[i] ;
	            n += 1 ;
	            rs = vechand_add(elp,ep) ;
	        } /* end for (copy ENVs) */
	    } /* end if (envbads) */
	} /* end if (envv) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (envhelp_copy) */

static int vechand_addover(vechand *elp,cchar *ep) noex {
	vechand_vcmp	vcf = vechand_vcmp(vstrkeycmp) ;
	cnullptr	np{} ;
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	if ((rs = vechand_search(elp,ep,vcf,np)) >= 0) {
	    rs = vechand_del(elp,rs) ;
	} else if (rs == rsn) {
	    rs = SR_OK ;
	}
	if (rs >= 0) {
	    rs = vechand_add(elp,ep) ;
	}
	return rs ;
}
/* end subroutine (vechand_addover) */

int envhelp::start(mainv argv,mainv envv) noex {
	return envhelp_start(this,argv,envv) ;
}

int envhelp::present(cchar *kp,int kl,cchar **rpp) noex {
	return envhelp_present(this,kp,kl,rpp) ;
}

int envhelp::envset(cchar *kp,cchar *vp,int vl) noex {
    	return envhelp_envset(this,kp,vp,vl) ;
}

int envhelp::getvec(mainv *rp) noex {
	return envhelp_getvec(this,rp) ;
}

void envhelp::dtor() noex {
	if (cint rs = int(finish) ; rs < 0) {
	    ulogerror("envhelp",rs,"fini-finish") ;
	}
}

envhelp_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case envhelpmem_sort:
	        rs = envhelp_sort(op) ;
	        break ;
	    case envhelpmem_count:
	        rs = envhelp_count(op) ;
	        break ;
	    case envhelpmem_finish:
	        rs = envhelp_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (envhelp_co::operator) */


