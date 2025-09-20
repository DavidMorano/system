/* offindex SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* offset-index object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	offindex

	Description:
	This object is used to create a line-index of the TXTINDEX
	"tag" file.  The line-index consists of file offsets of the
	beginning of each line in the file.  Each line corresponds
	with a "tag" record.   The length of each "record" is also
	stored.

*******************************************************************************/

#include	<envstandards.h>	/* must be before others */
#include	<unistd.h>		/* |off_t| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<new>
#include	<usystem.h>
#include	<vecobj.h>
#include	<localmisc.h>

#include	"offindex.h"


/* local defines */

#define	OI_E		struct offindex_e

#define	NDEF		100


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct offindex_e {
	off_t		lineoff ;
	int		linelen ;
	offindex_e() = default ;
	offindex_e(off_t o,int l) noex : lineoff(o), linelen(l) { } ;
} ;


/* forward references */

template<typename ... Args>
static int offindex_ctor(offindex *op,Args ... args) noex {
    	OFFINDEX	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->oip = new(nothrow) vecobj) != np) {
		rs = SR_OK ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (offindex_ctor) */

static int offindex_dtor(offindex *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->oip) {
		delete op->oip ;
		op->oip = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (offindex_dtor) */

template<typename ... Args>
static inline int offindex_magic(offindex *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == OFFINDEX_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (offindex_magic) */

extern "C" {
    static int vecmp(cvoid **,cvoid **) noex ;
}

static int ecmpe(OI_E *,OI_E *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int offindex_start(offindex *op,int vn) noex {
	int		rs ;
	if (vn < NDEF) vn = NDEF ;
	if ((rs = offindex_ctor(op)) >= 0) {
	    cint	vsz = szof(OI_E) ;
	    cint	vo = 0 ;
	    if ((rs = vecobj_start(op->oip,vsz,vn,vo)) >= 0) {
	        op->magic = OFFINDEX_MAGIC ;
	    }
	    if (rs < 0) {
		offindex_dtor(op) ;
	    }
	} /* end if (offindex_ctor) */
	return rs ;
}
/* end subroutine (offindex_start) */

int offindex_finish(offindex *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = offindex_magic(op)) >= 0) {
	    {
	        rs1 = vecobj_finish(op->oip) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = offindex_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (offindex_finish) */

int offindex_add(offindex *op,off_t off,int len) noex {
	int		rs ;
	if ((rs = offindex_magic(op)) >= 0) {
	    OI_E	e ;
	    e.lineoff = off ;
	    e.linelen = len ;
	    rs = vecobj_add(op->oip,&e) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (offindex_add) */

int offindex_lookup(offindex *op,off_t off) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = offindex_magic(op)) >= 0) {
	    OI_E	key ;
	    rs = SR_NOSYS ;
	    if (! op->fl.setsorted) {
	        op->fl.setsorted = true ;
	        vecobj_setsorted(op->oip) ;
	    }
	    key.lineoff = off ;
	    key.linelen = 0 ;
	    void	*vp{} ;
	    if ((rs = vecobj_search(op->oip,&key,vecmp,&vp)) >= 0) {
	        rs = SR_BADFMT ;
	        if (vp) {
	            OI_E	*oep = (OI_E *) vp ;
	            rs = SR_OK ;
	            len = oep->linelen ;
	        }
	    }
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (offindex_lookup) */


/* private subroutines */

static int vecmp(cvoid **v1pp,cvoid **v2pp) noex {
	OI_E		*e1p = (OI_E *) *v1pp ;
	OI_E		*e2p = (OI_E *) *v2pp ;
	return ecmpe(e1p,e2p) ;
}
/* end subroutine (vecmp) */

static int ecmpe(OI_E *e1p,OI_E *e2p) noex {
	int		rc = 0 ;
	if (e1p || e2p) {
	    rc = +1 ;
	    if (e1p) {
		rc = -1 ;
	        if (e2p) {
	            rc = (e1p->lineoff - e2p->lineoff) ;
	        }
	    }
	}
	return rc ;
}
/* end subroutine (ecmpe) */


