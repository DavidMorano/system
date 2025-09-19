/* cmdmap SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* command mapping management */
/* version %I% last-modified %G% */

#define	CF_FASTDEF	1		/* use fast-default */

/* revision history:

	= 2009-01-20, David A­D­ Morano
	This was written from scratch.

*/

/* Copyright © 2009 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	cmdmap

	Description:
	This object manages the mapping between keys and commands.
	Using the "lookup" function, one can provide a key (in the
	form of a KEYSYM value) and this object will return the
	index of a 'command'.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"cmdmap.h"


/* local defines */

#ifndef	CF_FASTDEF
#define	CF_FASTDEF	1		/* use fast-default */
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef cmdmap_ent	ent ;
typedef cmdmap_ent *	entp ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int cmdmap_ctor(cmdmap *op,Args ... args) noex {
	CMDMAP		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->mlp = new(nothrow) vecobj) != np) {
		rs = SR_OK ;
	    } /* end if (new-vecobj) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (cmdmap_ctor) */

static int cmdmap_dtor(cmdmap *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->mlp) {
		delete op->mlp ;
		op->mlp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (cmdmap_dtor) */

template<typename ... Args>
static inline int cmdmap_magic(cmdmap *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == CMDMAP_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (cmdmap_magic) */

static int cmdmap_defmap(cmdmap *,const cmdmap_ent *) noex ;

extern "C" {
    static int	vcmpfind(cvoid **,cvoid **) noex ;
}


/* local variables */

constexpr bool		f_fastdef = CF_FASTDEF ;


/* exported variables */


/* exported subroutines */

int cmdmap_start(cmdmap *op,const cmdmap_ent *defmap) noex {
	int		rs ;
	if ((rs = cmdmap_ctor(op)) >= 0) {
	    cint	sz = szof(cmdmap_ent) ;
	    cint	vn = 10 ;
	    cint	vo = VECOBJ_OREUSE ;
	    if ((rs = vecobj_start(op->mlp,sz,vn,vo)) >= 0) {
	        if (defmap) {
		    rs = cmdmap_defmap(op,defmap) ;
		}
	        if (rs >= 0) {
	            op->magic = CMDMAP_MAGIC ;
	        }
	        if (rs < 0) {
		    vecobj_finish(op->mlp) ;
	        }
	    } /* end if (vecobj-started) */
	    if (rs < 0) {
		cmdmap_dtor(op) ;
	    }
	} /* end if (cmdmap_ctor) */
	return rs ;
}
/* end subroutine (cmdmap_start) */

int cmdmap_finish(cmdmap *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = cmdmap_magic(op)) >= 0) {
	    {
		rs1 = vecobj_finish(op->mlp) ;
		if (rs < 0) rs = rs1 ;
	    }
	    {
		rs1 = cmdmap_dtor(op) ;
		if (rs < 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (cmdmap_finish) */

int cmdmap_load(cmdmap *op,int key,int cmd) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	if ((rs = cmdmap_magic(op)) >= 0) {
	    rs = SR_INVALID ;
	    if (key >= 0) {
	        cmdmap_ent	e{} ;
	        bool		f_add = true ;
	        e.key = key ;
	        e.cmd = cmd ;
		auto vos = vecobj_search ;
	        auto vcf = vcmpfind ;
	        if (void *vp{} ; (rs = vos(op->mlp,&e,vcf,&vp)) >= 0) {
	            cmdmap_ent	*ep = entp(vp) ;
		    cint	idx = rs ;
	            if (ep->cmd != e.cmd) {
	                rs = vecobj_del(op->mlp,idx) ;
	            } else {
		        f_add = false ;
	            }
	        } else if (rs == rsn) {
	            rs = SR_OK ;
	        } /* end if */
	        if ((rs >= 0) && f_add) {
	            op->fl.sorted = false ;
	            rs = vecobj_add(op->mlp,&e) ;
	        }
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (cmdmap_load) */

int cmdmap_lookup(cmdmap *op,int key) noex {
	int		rs ;
	int		cmd = 0 ;
	if ((rs = cmdmap_magic(op)) >= 0) {
	    rs = SR_INVALID ;
	    if (key >= 0) {
		rs = SR_OK ;
	        if (! op->fl.sorted) {
	            op->fl.sorted = true ;
	            rs = vecobj_sort(op->mlp,vcmpfind) ;
	        }
	        if (rs >= 0) {
	            cmdmap_ent	te{} ;
	            te.key = key ;
		    auto	vos = vecobj_search ;
	            auto	vcf = vcmpfind ;
	            if (void *vp{} ; (rs = vos(op->mlp,&te,vcf,&vp)) >= 0) {
	                if (vp) {
			    cmdmap_ent	*ep = entp(vp) ;
	                    cmd = ep->cmd ;
	                }
	            } /* end if (vecobj_search) */
	        } /* end if (ok) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? cmd : rs ;
}
/* end subroutine (cmdmap_lookup) */


/* private subroutines */

static int cmdmap_defmap(cmdmap *op,const cmdmap_ent *defmap) noex {
	int		rs = SR_OK ;
	int		i = 0 ;
	if_constexpr (f_fastdef) {
	    cmdmap_ent	*ep ;
	    for (i = 0 ; (rs >= 0) && (defmap[i].key >= 0) ; i += 1) {
	        ep = entp(defmap + i) ;
	        rs = vecobj_add(op->mlp,ep) ;
	    } /* end for */
	} else {
	    for (i = 0 ; (rs >= 0) && (defmap[i].key >= 0) ; i += 1) {
	        rs = cmdmap_load(op,defmap[i].key,defmap[i].cmd) ;
	    } /* end for */
	} /* end if_constexpr (f_fastdef) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (cmdmap_defmap) */

static int vcmpfind(cvoid **v1pp,cvoid **v2pp) noex {
	cmdmap_ent	*e1p = (cmdmap_ent *) *v1pp ;
	cmdmap_ent	*e2p = (cmdmap_ent *) *v2pp ;
	int		rc = 0 ;
	if (e1p || e2p) {
	    rc = +1 ;
	    if (e1p) {
		rc = -1 ;
	        if (e2p) {
	            rc = e1p->key - e2p->key ;
	        }
	    }
	}
	return rc ;
}
/* end subroutine (vcmpfind) */


