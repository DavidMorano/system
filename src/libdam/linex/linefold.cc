/* linefold SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* manage folding of a line */
/* version %I% last-modified %G% */


/* revision history:

	= 2009-04-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2009 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	linefold

	Description:
	This object module takes a line of text as input and breaks
	it up into pieces that are folded so as to fit in a specified
	number of cols.

	Synopsis:
	int linefold_start(linefold *op,int cols,int ind,
		cchar *lbuf,int llen) noex {

	Arguments:
	op	object pointer
	cols	number of columns in a line (normally '80')
	ind	number of columns to indent for 2nd and subsequent lines
	lbuf	source line (c-string) pointer
	llen	source line (c-string) length

	Returns:
	>=0	OK
	<0	error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<usystem.h>
#include	<vecobj.h>
#include	<cfdec.h>
#include	<ncol.h>		/* |charcols(3uc)| */
#include	<rmx.h>			/* |rmeol(3uc)| */
#include	<char.h>		/* |CHAR_ISWHITE(3uc)| */
#include	<mkchar.h>
#include	<ischarx.h>		/* |iseol(3uc)| */
#include	<localmisc.h>		/* |NTABCOLS| + |COLUMNS| */

#include	"linefold.h"

import libutil ;

/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct params {
	int		cols ;
	int		ind ;
	int		nline ;
} ;

struct liner {
	cchar		*lp ;
	int		ll ;
} ;

typedef liner *		linerp ;


/* forward references */

template<typename ... Args>
static int linefold_ctor(linefold *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(op) ;
	    if ((op->llp = new(nothrow) vecobj) != np) {
		rs = SR_OK ;
	    } /* end if (new-vecobj) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (linefold_ctor) */

static int linefold_dtor(linefold *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->llp) {
		delete op->llp ;
		op->llp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (linefold_dtor) */

template<typename ... Args>
static int linefold_magic(linefold *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == LINEFOLD_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (linefold_magic) */

static int	linefold_process(linefold *,int,int,cchar *,int) noex ;

static int	params_load(params *,int,int) noex ;
static int	params_nextline(params *,cchar *,int,cchar **) noex ;
static int	params_nline(params *) noex ;

static int	getcols(int) noex ;
static int	nextpiece(int,cchar *,int,int *) noex ;


/* local variables */

constexpr int	ntabcols = NTABCOLS ;


/* exported variables */


/* exported subroutines */

int linefold_start(linefold *op,int cols,int ind,cchar *lbuf,int llen) noex {
	int		rs ;
	int		nlines = 0 ;
	if ((rs = linefold_ctor(op,lbuf)) >= 0) {
	    if ((rs = getcols(cols)) >= 0) {
	        cint	sz = szof(liner) ;
		cint	ne = 10 ;
	        cint	vo = (VECOBJ_OCOMPACT) ;
		cols = rs ;
	        if ((rs = vecobj_start(op->llp,sz,ne,vo)) >= 0) {
		    cint	ll = rmeol(lbuf,llen) ;
	            if ((rs = linefold_process(op,cols,ind,lbuf,ll)) >= 0) {
	                nlines = rs ;
	                op->magic = LINEFOLD_MAGIC ;
	            }
	            if (rs < 0) {
		        vecobj_finish(op->llp) ;
		    }
	        } /* end if (vecobj_start) */
	    } /* end if (ok) */
	    if (rs < 0) {
		linefold_dtor(op) ;
	    }
	} /* end if (linefold_ctor) */
	return (rs >= 0) ? nlines : rs ;
}
/* end subroutine (linefold_start) */

int linefold_finish(linefold *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = linefold_magic(op)) >= 0) {
	    if (op->llp) {
	        rs1 = vecobj_finish(op->llp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = linefold_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (linefold_finish) */

int linefold_get(linefold *op,int li,cchar **rpp) noex {
	int		rs ;
	int		ll = 0 ;
	cchar		*rp = nullptr ;
	if ((rs = linefold_magic(op)) >= 0) {
	    rs = SR_INVALID ;
	    if (li >= 0) {
	        void	*vp{} ;
	        if ((rs = vecobj_get(op->llp,li,&vp)) >= 0) {
	            liner	*lep = linerp(vp) ;
	            ll = lep->ll ;
	            rp = lep->lp ;
	        }
	    } /* end if (valid) */
	} /* end if (magic) */
	if (rpp) {
	    *rpp = (rs >= 0) ? rp : nullptr ;
	}
	return (rs >= 0) ? ll : rs ;
}
/* end subroutine (linefold_get) */

int linefold_getln(linefold *op,int li,cchar **rpp) noex {
	int		rs ;
	int		ll = 0 ;
	cchar		*rp = nullptr ;
	if ((rs = linefold_magic(op)) >= 0) {
	    rs = SR_INVALID ;
	    if (li >= 0) {
	        void	*vp{} ;
	        if ((rs = vecobj_get(op->llp,li,&vp)) >= 0) {
	            liner	*lep = linerp(vp) ;
	            ll = lep->ll ;
	            rp = lep->lp ;
	        } else if (rs == SR_NOTFOUND) {
	            rs = SR_OK ;
	        }
	    } /* end if (valid) */
	} /* end if (magic) */
	if (rpp) {
	    *rpp = ((rs >= 0) && (ll > 0)) ? rp : nullptr ;
	}
	return (rs >= 0) ? ll : rs ;
}
/* end subroutine (linefold_getln) */

int linefold_count(linefold *op) noex {
	int		rs ;
	if ((rs = linefold_magic(op)) >= 0) {
	    rs = SR_BUGCHECK ;
	    if (op->llp) {
	        rs = vecobj_count(op->llp) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (linefold_count) */


/* private subroutines */

static int linefold_process(linefold *op,int cols,int ind,
		cc *lbuf,int llen) noex {
	int		rs ;
	int		nline = 0 ; /* return-value */
	if (llen < 0) llen = lenstr(lbuf) ;
	if (params p ; (rs = params_load(&p,cols,ind)) >= 0) {
	    liner	le{} ;
	    int		sl = llen ;
	    cchar	*sp = lbuf ;
	    cchar	*lp ;
	    for (int ll ; (ll = params_nextline(&p,sp,sl,&lp)) > 0 ; ) {
	        le.lp = lp ;
	        le.ll = ll ;
	        rs = vecobj_add(op->llp,&le) ;
	        sl -= intconv((lp + ll) - sp) ;
	        sp = (lp + ll) ;
	        if (rs < 0) break ;
	    } /* end for */
	    if (rs >= 0) {
	        nline = params_nline(&p) ;
	    }
	} /* end if (params_load) */
	return (rs >= 0) ? nline : rs ;
}
/* end subroutine (linefold_process) */

static int params_load(params *pp,int cols,int ind) noex {
	int		rs = SR_FAULT ;
	if (pp) {
	    rs = memclear(pp) ;		/* dangerous */
	    pp->cols = cols ;
	    pp->ind = ind ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (params_load) */

static int params_nextline(params *pp,cchar *sp,int sl,cchar **rpp) noex {
	int		rs = SR_OK ;
	int		ncol ;
	int		ncs ;
	int		pl ;
	int		ll = 0 ;
	if (pp->nline > 0) {
	    while (sl && CHAR_ISWHITE(sp[0])) {
	        sp += 1 ;
	        sl -= 1 ;
	    }
	}
	*rpp = sp ;
	ncol = (pp->nline > 0) ? pp->ind : 0 ;
	while ((pl = nextpiece(ncol,sp,sl,&ncs)) > 0) {
	    if ((ncol + ncs) > pp->cols) {
		if (ll == 0) ll = pl ;
	        break ;
	    }
	    ll += pl ;
	    ncol += ncs ;
	    sp += pl ;
	    sl -= pl ;
	} /* end while */
	if ((rs >= 0) && (ll > 0)) {
	    pp->nline += 1 ;
	}
	return (rs >= 0) ? ll : rs ;
}
/* end subroutine (params_nextline) */

static int params_nline(params *pp) noex {
	return pp->nline ;
}
/* end subroutine (params_nline) */

static int getcols(int cols) noex {
	cchar		*vncols = varname.columns ;
	int		rs = SR_OK ;
	if (cols <= 0) {
	    static cchar	*vcols = getenv(vncols) ;
	    if (int v ; vcols && (cfdeci(vcols,-1,&v) >= 0)) {
	        cols = v ;
	    }
	    if (cols <= 0) {
	        cols = COLUMNS ;
	    }
	} /* end if (default cols) */
	return (rs >= 0) ? cols : rs ;
}
/* end subroutine (getcols) */

static int nextpiece(int ncol,cchar *sp,int sl,int *ncp) noex {
	int		pl = 0 ;
	int		ncs = 0 ;
	int		cl = sl ;
	int		ch ;
	cchar		*cp = sp ;
	/* skip over whitespace */
	while (cl && ((ch = mkchar(cp[0])),CHAR_ISWHITE(ch))) {
	    cint	n = charcols(ntabcols,ncol,ch) ;
	    cp += 1 ;
	    cl -= 1 ;
	    ncs += n ;
	    ncol += n ;
	} /* end while */
	/* skip over the non-whitespace */
	while (cl && ((ch = mkchar(cp[0]))) && (! CHAR_ISWHITE(ch))) {
	    cint	n = charcols(ntabcols,ncol,ch) ;
	    cp += 1 ;
	    cl -= 1 ;
	    ncs += n ;
	    ncol += n ;
	} /* end while */
	/* done */
	*ncp = ncs ;
	pl = intconv(cp - sp) ;
	return pl ;
}
/* end subroutine (nextpiece) */

int linefold::start(int cols,int ind,cchar *sp,int sl) noex {
	return linefold_start(this,cols,ind,sp,sl) ;
}

int linefold::get(int ai,cchar **rpp) noex {
	return linefold_get(this,ai,rpp) ;
}

int linefold::getln(int ai,cchar **rpp) noex {
	return linefold_getln(this,ai,rpp) ;
}

void linefold::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("linefold",rs,"fini-finish") ;
	}
} /* end method (linefold_co::dtor) */

linefold_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case linefoldmem_count:
	        rs = linefold_count(op) ;
	        break ;
	    case linefoldmem_finish:
	        rs = linefold_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (linefold_co::operator) */

bool linefold_iter::operator == (const linefold_iter &oit) noex {
	return (i == oit.i) ;
}

bool linefold_iter::operator != (const linefold_iter &oit) noex {
	return (i != oit.i) ;
}

linefold_iter linefold_iter::operator + (int n) const noex {
	linefold_iter	rit(op,i) ;
	rit.i = ((i + n) >= 0) ? (i + n) : 0 ;
	return rit ;
}

linefold_iter linefold_iter::operator += (int n) noex {
	linefold_iter	rit(op,i) ;
	i = ((i + n) >= 0) ? (i + n) : 0 ;
	rit.i = i ;
	return rit ;
}

linefold_iter linefold_iter::operator ++ () noex { /* pre */
	increment() ;
	return (*this) ;
}

linefold_iter linefold_iter::operator ++ (int) noex { /* post */
	linefold_iter	pre(*this) ;
	increment() ;
	return pre ;
}

void linefold_iter::increment(int n) noex {
	i += n ;
}
/* end method (linefold_iter::increment) */

cchar *linefold_iter::operator * () noex {
	cchar		*rp = nullptr ;
	cchar		*cp{} ;
	if (op->getln(i,&cp) >= 0) {
	    rp = cp ;
	}
	return rp ;
}
/* end method (linefold_iter::operator) */


