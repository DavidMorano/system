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
	This module provides operations and management on a static
	set of strings.

*******************************************************************************/

#include	<envstandards.h>	/* must be ordered first to configure */
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<new>			/* placement-new + |nothrow| */
#include	<usystem.h>
#include	<localmisc.h>

#include	"vsetstr.h"

import libutil ;

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
static int vsetstr_ctor(vsetstr *op,Args ... args) noex {
    	VSETSTR		*hop = op ;
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if (vecpstr *elp ; (elp = new(nothrow) vecpstr) != np) {
		op->elp = elp ;
		rs = SR_OK ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vsetstr_ctor) */

static int vsetstr_dtor(vsetstr *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    vecpstr	*elp = op->elp ;
	    rs = SR_OK ;
	    if (elp) {
		delete elp ;
		op->elp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vsetstr_dtor) */

template<typename ... Args>
static inline int vsetstr_magic(vsetstr *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == VSETSTR_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (vsetstr_magic) */


/* local variables */


/* exported variables */


/* exported subroutines */

int vsetstr_start(VS *op,int vn) noex {
	int		rs ;
	if ((rs = vsetstr_ctor(op)) >= 0) {
	    vecpstr	*elp = op->elp ;
	    cint	vsz = (vn > 0) ? (vn * 6) : 0 ;
	    if ((rs = elp->start(vn,vsz,0)) >= 0) {
	        op->magic = VSETSTR_MAGIC ;
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
	if ((rs = vsetstr_magic(op)) >= 0) {
	    vecpstr	*elp = op->elp ;
	    {
	        rs1 = elp->finish ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = vsetstr_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vsetstr_finish) */

int vsetstr_look(VS *op,cchar *sbuf,int slen) noex {
	int		rs ;
	if ((rs = vsetstr_magic(op,sbuf)) >= 0) {
	    vecpstr	*elp = op->elp ;
	    rs = elp->already(sbuf,slen) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vsetstr_look) */

/* add a string to the database */
int vsetstr_add(VS *op,cchar *sbuf,int slen) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	if ((rs = vsetstr_magic(op,sbuf)) >= 0) {
	    vecpstr	*elp = op->elp ;
	    if (slen < 0) slen = lenstr(sbuf) ;
	    if ((rs = elp->findn(sbuf,slen)) == rsn) {
	        rs = elp->add(sbuf,slen) ;
	    } else if (rs >= 0) {
	        rs = INT_MAX ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vsetstr_add) */

int vsetstr_curbegin(VS *op,VS_CUR *curp) noex {
    	int		rs ;
	if ((rs = vsetstr_magic(op,curp)) >= 0) {
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
	if ((rs = vsetstr_magic(op,curp)) >= 0) {
	    vecpstr	*elp = op->elp ;
	    cint	i = curp->i ;
	    rs = elp->del(i) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vsetstr_curdel) */

int vsetstr_already(VS *op,cchar *sp,int sl) noex {
	int		rs ;
	if ((rs = vsetstr_magic(op)) >= 0) {
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

/* enumerate all of the entries */
int vsetstr_curenum(VS *op,VS_CUR *curp,cchar **vpp) noex {
	int		rs ;
	int		rl = 0 ; /* return-value */
	if ((rs = vsetstr_magic(op,curp)) >= 0) {
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
	if ((rs = vsetstr_magic(op,curp)) >= 0) {
	    int		i = (curp->i >= 0) ? (curp->i + 1) : 0 ;
	    curp->i = i ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vsetstr_curnext) */

int vsetstr_count(VS *op) noex {
	int		rs ;
	if ((rs = vsetstr_magic(op)) >= 0) {
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


