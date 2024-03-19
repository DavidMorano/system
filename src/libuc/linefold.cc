/* linefold SUPPORT */
/* lang=C++20 */

/* manage folding of a line */
/* version %I% last-modified %G% */


/* revision history:

	= 2009-04-10, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 2009 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object module takes a line of text as input and breaks
	it up into pieces that are folded so as to fit in a specified
	number of cols.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<ucvariables.hh>	/* |varnames| */
#include	<vecobj.h>
#include	<ascii.h>
#include	<cfdec.h>
#include	<ncol.h>		/* |charcols(3uc)| */
#include	<char.h>
#include	<localmisc.h>

#include	"linefold.h"


/* local defines */

#ifndef	COLUMNS
#define	COLUMNS		80		/* output cols (should be 80) */
#endif

#ifndef	NTABCOLS
#define	NTABCOLS	8
#endif


/* imported namespaces */


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

static int	linefold_process(linefold *,int,int,cchar *,int) noex ;

static int	params_load(params *,int,int) noex ;
static int	params_nextline(params *,cchar *,int,cchar **) noex ;
static int	params_nline(params *) noex ;

static int	nextpiece(int,cchar *,int,int *) noex ;
static bool	isend(int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int linefold_start(linefold *op,int cols,int ind,cchar *lbuf,int llen) noex {
	cchar		*vncols = varname.columns ;
	int		rs = SR_OK ;
	int		nlines = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (lbuf == nullptr) return SR_FAULT ;

	if (llen < 0) llen = strlen(lbuf) ;

	memclear(op) ;
	if (cols <= 0) {
	    static cchar	*vvp = getenv(vncols) ;
	    int			v ;
	    if (vvp && (cfdeci(vvp,-1,&v) >= 0)) {
	        cols = v ;
	    }
	    if (cols <= 0) {
	        cols = COLUMNS ;
	    }
	} /* end if (default cols) */

	if (rs >= 0) {
	    cint	size = sizeof(liner) ;
	    cint	vo = (VECOBJ_OCOMPACT) ;
	    if ((rs = vecobj_start(&op->lines,size,10,vo)) >= 0) {
	        if (llen < 0) llen = strlen(lbuf) ;
	        while ((llen > 0) && isend(lbuf[llen-1])) llen -= 1 ;
	        if ((rs = linefold_process(op,cols,ind,lbuf,llen)) >= 0) {
	            nlines = rs ;
	            op->magic = LINEFOLD_MAGIC ;
	        }
	        if (rs < 0) {
		    vecobj_finish(&op->lines) ;
		}
	    } /* end if (vecobj_start) */
	} /* end if (ok) */

	return (rs >= 0) ? nlines : rs ;
}
/* end subroutine (linefold_start) */

int linefold_finish(linefold *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != LINEFOLD_MAGIC) return SR_NOTOPEN ;

	rs1 = vecobj_finish(&op->lines) ;
	if (rs >= 0) rs = rs1 ;

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (linefold_finish) */

int linefold_get(linefold *op,int li,cchar **rpp) noex {
	liner		*lep ;
	int		rs ;
	int		ll = 0 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != LINEFOLD_MAGIC) return SR_NOTOPEN ;

	if (li < 0) return SR_INVALID ;

	void		*vp{} ;
	if ((rs = vecobj_get(&op->lines,li,&vp)) >= 0) {
	    lep = linerp(vp) ;
	    ll = lep->ll ;
	}

	if (rpp) {
	    *rpp = (rs >= 0) ? lep->lp : nullptr ;
	}

	return (rs >= 0) ? ll : rs ;
}
/* end subroutine (linefold_get) */

int linefold_getline(linefold *op,int li,cchar **rpp) noex {
	liner		*lep ;
	int		rs ;
	int		ll = 0 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != LINEFOLD_MAGIC) return SR_NOTOPEN ;

	if (li < 0) return SR_INVALID ;

	void		*vp{} ;
	if ((rs = vecobj_get(&op->lines,li,&vp)) >= 0) {
	    lep = linerp(vp) ;
	    ll = lep->ll ;
	} else if (rs == SR_NOTFOUND)
	    rs = SR_OK ;

	if (rpp) {
	    *rpp = ((rs >= 0) && (ll > 0)) ? lep->lp : nullptr ;
	}

	return (rs >= 0) ? ll : rs ;
}
/* end subroutine (linefold_getline) */


/* private subroutines */

static int linefold_process(linefold *op,int cols,int ind,
		cc *lbuf,int llen) noex {
	liner		le ;
	params		p ;
	int		rs = SR_OK ;
	int		sl, ll ;
	int		nline = 0 ;
	cchar		*sp ;
	cchar		*lp ;

	params_load(&p,cols,ind) ;

	if (llen < 0)
	    llen = strlen(lbuf) ;

	sp = lbuf ;
	sl = llen ;
	while ((ll = params_nextline(&p,sp,sl,&lp)) > 0) {
	    le.lp = (cchar *) lp ;
	    le.ll = ll ;
	    rs = vecobj_add(&op->lines,&le) ;
	    sl -= ((lp + ll) - sp) ;
	    sp = (lp + ll) ;
	    if (rs < 0) break ;
	} /* end while */
	if (rs >= 0) {
	    nline = params_nline(&p) ;
	}

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

/* move up to the first non-whitespace character */

	if (pp->nline > 0) {
	    while (sl && CHAR_ISWHITE(sp[0])) {
	        sp += 1 ;
	        sl -= 1 ;
	    }
	}

	*rpp = sp ;
	ncol = (pp->nline > 0) ? pp->ind : 0 ;

/* continue */

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

	if ((rs >= 0) && (ll > 0))
	    pp->nline += 1 ;

	return (rs >= 0) ? ll : rs ;
}
/* end subroutine (params_nextline) */

static int params_nline(params *pp) noex {
	return pp->nline ;
}
/* end subroutine (params_nline) */

static int nextpiece(int ncol,cchar *sp,int sl,int *ncp) noex {
	int		ncs = 0 ;
	int		cl = sl ;
	int		n ;
	int		pl = 0 ;
	cchar		*cp = sp ;
/* skip over whitespace */
	while (cl && CHAR_ISWHITE(cp[0])) {
	    n = charcols(NTABCOLS,ncol,cp[0]) ;
	    cp += 1 ;
	    cl -= 1 ;
	    ncs += n ;
	    ncol += n ;
	} /* end while */
/* skip over the non-whitespace */
	while (cl && cp[0] && (! CHAR_ISWHITE(cp[0]))) {
	    n = charcols(NTABCOLS,ncol,cp[0]) ;
	    cp += 1 ;
	    cl -= 1 ;
	    ncs += n ;
	    ncol += n ;
	} /* end while */
/* done */
	*ncp = ncs ;
	pl = (cp - sp) ;
	return pl ;
}
/* end subroutine (nextpiece) */

static bool isend(int ch) noex {
	return ((ch == '\n') || (ch == '\r')) ;
}
/* end subroutine (isend) */


