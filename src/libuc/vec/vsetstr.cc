/* vsetstr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* Vector-Implemented Ordered-Set-String object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-12, David A­D­ Morano
	This module was originally written (from scratch).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	vsetstr

	Description:
	This module provides operations and management on a
	set of strings.

*******************************************************************************/

#include	<envstandards.h>	/* must be ordered first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<new>			/* placement-new + |nothrow| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<localmisc.h>

#include	"vsetstr.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	VS	vsetstr
#define	VS_CUR	vsetstr_cur


/* imported namespaces */

using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local int vsetstr_ctor(vsetstr *op,Args ... args) noex {
    	VSETSTR		*hop = op ;
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if (vecpstr *elp ; (elp = new(nothrow) vecpstr) != np) ylikely {
		op->elp = elp ;
		rs = SR_OK ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (vsetstr_ctor) */

local int vsetstr_dtor(vsetstr *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    vecpstr	*elp = op->elp ;
	    rs = SR_OK ;
	    if (elp) ylikely {
		delete elp ;
		op->elp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (vsetstr_dtor) */

template<typename ... Args>
local inline int vsetstr_magic(vsetstr *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == VSETSTR_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (vsetstr_magic) */


/* local variables */


/* exported variables */


/* exported subroutines */

int vsetstr_start(VS *op,int vn) noex {
	int		rs ;
	if ((rs = vsetstr_ctor(op)) >= 0) ylikely {
	    vecpstr	*elp = op->elp ;
	    cint	vsz = (vn > 0) ? (vn * 6) : 0 ;
	    if ((rs = elp->start(vn,vsz,0)) >= 0) ylikely {
	        op->magval = VSETSTR_MAGIC ;
	    }
	    if (rs < 0) {
		vsetstr_dtor(op) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vsetstr_start) */

int vsetstr_finish(VS *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = vsetstr_magic(op)) >= 0) ylikely {
	    vecpstr	*elp = op->elp ;
	    {
	        rs1 = elp->finish ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = vsetstr_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vsetstr_finish) */

int vsetstr_look(VS *op,cchar *sbuf,int slen) noex {
	int		rs ;
	if ((rs = vsetstr_magic(op,sbuf)) >= 0) ylikely {
	    vecpstr	*elp = op->elp ;
	    rs = elp->already(sbuf,slen) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vsetstr_look) */

/* add a string to the database */
int vsetstr_add(VS *op,cchar *sp,int µsl) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	if ((rs = vsetstr_magic(op,sp)) >= 0) ylikely {
	    if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) {
	        vecpstr	*elp = op->elp ;
	        if ((rs = elp->findn(sp,sl)) == rsn) {
	            rs = elp->add(sp,sl) ;
	        } else if (rs >= 0) {
	            rs = INT_MAX ;
	        }
	    } /* end if (getlenstr) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vsetstr_add) */

int vsetstr_curbegin(VS *op,VS_CUR *curp) noex {
    	int		rs ;
	if ((rs = vsetstr_magic(op,curp)) >= 0) ylikely {
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vsetstr_curbegin) */

int vsetstr_curend(VS *op,VS_CUR *curp) noex {
    	int		rs ;
	if ((rs = vsetstr_magic(op,curp)) >= 0) {
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vsetstr_curend) */

int vsetstr_curdel(VS *op,VS_CUR *curp) noex {
	int		rs ;
	if ((rs = vsetstr_magic(op,curp)) >= 0) ylikely {
	    vecpstr	*elp = op->elp ;
	    cint	i = curp->i ;
	    rs = elp->del(i) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vsetstr_curdel) */

/* enumerate all of the entries */
int vsetstr_curenum(VS *op,VS_CUR *curp,cchar **vpp) noex {
	int		rs ;
	int		rl = 0 ; /* return-value */
	if ((rs = vsetstr_magic(op,curp)) >= 0) ylikely {
	    vecpstr	*elp = op->elp ;
	    int		i = (curp->i >= 0) ? (curp->i + 1) : 0 ;
	    cchar	*rp = nullptr ;
	    while ((rs = elp->get(i,&rp)) >= 0) {
	        if (rp) break ;
	        i += 1 ;
	    } /* end while */
	    if (rs >= 0) {
	        curp->i = i ;
	        rl = lenstr(rp) ;
	    }
	    if (vpp) {
	        *vpp = (rs >= 0) ? rp : nullptr ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (vsetstr_curenum) */

/* advance the cursor to the next entry regardless of key */
int vsetstr_curnext(VS *op,VS_CUR *curp) noex {
	int		rs ;
	if ((rs = vsetstr_magic(op,curp)) >= 0) ylikely {
	    int		i = (curp->i >= 0) ? (curp->i + 1) : 0 ;
	    curp->i = i ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vsetstr_curnext) */

int vsetstr_already(VS *op,cchar *sp,int sl) noex {
	int		rs ;
	if ((rs = vsetstr_magic(op)) >= 0) ylikely {
	    vecpstr *elp = op->elp ;
	    if ((rs = elp->findn(sp,sl)) >= 0) {
	        rs = true ;
	    } else if (rs == SR_NOTFOUND) {
	        rs = SR_OK ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vsetstr_already) */

int vsetstr_count(VS *op) noex {
	int		rs ;
	if ((rs = vsetstr_magic(op)) >= 0) ylikely {
	    vecpstr	*elp = op->elp ;
	    rs = elp->count ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vsetstr_count) */

int vsetstr_extent(VS *op) noex {
	return vsetstr_count(op) ;
}
/* end subroutine (vsetstr_extent) */


