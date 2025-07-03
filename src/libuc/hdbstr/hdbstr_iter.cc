/* hdbstr_iter SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Key-Value Hash DataBase for Strings */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-12, David A­D­ Morano
	This module was adapted from some previous code.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	hdbstr

	Description:
	This module provides a hash access container object for strings.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<hdb.h>
#include	<strwcpy.h>
#include	<strdcpyx.h>
#include	<localmisc.h>

#include	"hdbstr.h"


/* local defines */

#define	HS		hdbstr
#define	HS_CUR		hdbstr_cur


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	hdbstriter_finish(hdbstr_iter *) noex ;


/* local variables */

constexpr char		objname[] = "hdbstr_iter" ;

constexpr nullptr_t	np{} ;


/* exported variables */


/* exported subroutines */

hdbstr_iter::hdbstr_iter(const hdbstr_iter &oit) noex {
    	int		rs = SR_OK ;
	if (oit.op && oit.curp) {
	    op = oit.op ;
    	    if (curp) {
	        if ((rs = hdbstr_curend(op,curp)) >= 0) {
		    rs = hdbstr_curcopy(op,curp,oit.curp) ;
	        }
	    } else {
	        cint csz = szof(hdbstr_cur) ;
	        if (void *vp{} ; (rs = uc_malloc(csz,&vp)) >= 0) {
		    curp = (hdb_cur *) vp ;
		    rs = hdbstr_curcopy(op,curp,oit.curp) ;
		    if (rs < 0) {
			uc_free(curp) ;
			curp = nullptr ;
		    } /* end if (error handling) */
	        }
	    }
	    if (rs < 0) {
		ulogerror(objname,rs,"direct-initialization") ;
	    }
	}
}
/* end ctor */

hdbstr_iter &hdbstr_iter::operator = (const hdbstr_iter &oit) noex {
    	int		rs = SR_OK ;
	if (oit.op && oit.curp) {
	    op = oit.op ;
    	    if (curp) {
	        if ((rs = hdbstr_curend(op,curp)) >= 0) {
		    rs = hdbstr_curcopy(op,curp,oit.curp) ;
	        }
	    } else {
	        cint csz = szof(hdbstr_cur) ;
	        if (void *vp{} ; (rs = uc_malloc(csz,&vp)) >= 0) {
		    curp = (hdb_cur *) vp ;
		    rs = hdbstr_curcopy(op,curp,oit.curp) ;
		    if (rs < 0) {
			uc_free(curp) ;
			curp = nullptr ;
		    } /* end if (error handling) */
	        } /* end if (memory-allocation) */
	    }
	    if (rs < 0) {
		ulogerror(objname,rs,"copy") ;
	    }
	}
	return *this ;
}

bool hdbstr_iter::operator != (const hdbstr_iter &) noex {
    	int		rs ;
    	bool		f = false ;
	if ((rs = hdbstr_curdone(op,curp)) == 0) {
	    f = true ;
	} else if (rs < 0) {
	    ulogerror(objname,rs,"neq") ;
	}
    	return f ;
}

bool hdbstr_iter::operator == (const hdbstr_iter &) noex {
    	int		rs ;
    	bool		f = false ;
	if ((rs = hdbstr_curdone(op,curp)) > 0) {
	    f = true ;
	} else if (rs < 0) {
	    ulogerror(objname,rs,"neq") ;
	}
    	return f ;
}

cchar *hdbstr_iter::operator * () noex {
    	cint		rsn = SR_NOTFOUND ;
    	int		rs = SR_OK ;
	cchar		*rp = nullptr ;
	if (op && curp) {
	    if (cchar *vp{} ; (rs = op->curget(curp,np,&vp,np)) >= 0) {
		rp = vp ;
	    } else if (rs != rsn) {
		ulogerror(objname,rs,"dereference") ;
	    }
	}
    	return rp ;
}

hdbstr_iter hdbstr_iter::operator + (int inc) const noex {
    	hdbstr_iter	it(*this) ;
    	if (cint rs = it.increment(inc) ; rs < 0) {
	    ulogerror(objname,rs,"add-inc") ;
	}
	return it ;
}

hdbstr_iter &hdbstr_iter::operator += (int inc) noex {
    	if (cint rs = increment(inc) ; rs < 0) {
	    ulogerror(objname,rs,"add-inc") ;
	}
	return *this ;
}

hdbstr_iter &hdbstr_iter::operator ++ () noex {
    	if (cint rs = increment(1) ; rs < 0) {
	    ulogerror(objname,rs,"pre-inc") ;
	}
	return *this ;
}

hdbstr_iter hdbstr_iter::operator ++ (int) noex {
    	hdbstr_iter	it(*this) ;
    	if (cint rs = increment(1) ; rs < 0) {
	    ulogerror(objname,rs,"post-inc") ;
	}
	return it ;
}

int hdbstr_iter::increment(int inc) noex {
    	int		rs = SR_OK ;
	for (int i = 0 ; (rs >= 0) && (i < inc) ; i += 1) {
	    rs = hdbstr_curnext(op,curp) ;
	} /* end for */
	return rs ;
}

void hdbstr_iter::dtor() noex {
	if (cint rs = hdbstriter_finish(this) ; rs < 0) {
	    ulogerror(objname,rs,"fini-finish") ;
	}
}

static int hdbstriter_finish(hdbstr_iter *itp) noex {
    	int		rs = SR_OK ;
	int		rs1 ;
	if (itp->op && itp->curp) {
	    {
	        rs1 = hdb_curend(itp->op,itp->curp) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = uc_free(itp->curp) ;
		if (rs >= 0) rs = rs1 ;
		itp->curp = nullptr ;
	    }
	    itp->op = nullptr ;
	}
	return rs ;
}


