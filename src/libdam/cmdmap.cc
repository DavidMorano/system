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
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"cmdmap.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#ifndef	CF_FASTDEF
#define	CF_FASTDEF	1		/* use fast-default */
#endif


/* imported namespaces */


/* local typedefs */

typedef cmdmap_ent	ent ;
typedef cmdmap_ent *	entp ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local int cmdmap_ctor(cmdmap *op,Args ... args) noex {
	CMDMAP		*hop = op ;
	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->mlp = new(nt) vecobj) != np) {
		rs = SR_OK ;
	    } /* end if (new-vecobj) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (cmdmap_ctor) */

local int cmdmap_dtor(cmdmap *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->mlp) ylikely {
		delete op->mlp ;
		op->mlp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (cmdmap_dtor) */

template<typename ... Args>
local inline int cmdmap_magic(cmdmap *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == CMDMAP_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (cmdmap_magic) */

local int cmdmap_defmap(cmdmap *,const cmdmap_ent *) noex ;

extern "C" {
    local int	vcmpfind(cvoid **,cvoid **) noex ;
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
	    cint	vo = vecobjm.reuse ;
	    if ((rs = vecobj_start(op->mlp,sz,vn,vo)) >= 0) {
	        if (defmap) {
		    rs = cmdmap_defmap(op,defmap) ;
		}
	        if (rs >= 0) {
	            op->magval = CMDMAP_MAGIC ;
	        }
	        if (rs < 0) {
		    vecobj_finish(op->mlp) ;
	        } /* end if (error) */
	    } /* end if (vecobj-started) */
	    if (rs < 0) {
		cmdmap_dtor(op) ;
	    } /* end if (error) */
	} /* end if (cmdmap_ctor) */
	return rs ;
} /* end subroutine (cmdmap_start) */

int cmdmap_finish(cmdmap *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = cmdmap_magic(op)) >= 0) ylikely {
	    {
		rs1 = vecobj_finish(op->mlp) ;
		if (rs < 0) rs = rs1 ;
	    }
	    {
		rs1 = cmdmap_dtor(op) ;
		if (rs < 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (cmdmap_finish) */

int cmdmap_load(cmdmap *op,int key,int cmd) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	if ((rs = cmdmap_magic(op)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (key >= 0) {
	        cmdmap_ent	e{} ;
	        bool		f_add = true ;
	        e.key = key ;
	        e.cmd = cmd ;
		cauto vos = vecobj_search ;
	        cauto vcf = vcmpfind ;
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
} /* end subroutine (cmdmap_load) */

int cmdmap_lookup(cmdmap *op,int key) noex {
	int		rs ;
	int		cmd = 0 ;
	if ((rs = cmdmap_magic(op)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (key >= 0) {
		rs = SR_OK ;
	        if (! op->fl.sorted) {
	            op->fl.sorted = true ;
	            rs = vecobj_sort(op->mlp,vcmpfind) ;
	        }
	        if (rs >= 0) ylikely {
	            cmdmap_ent	te{} ;
	            te.key = key ;
		    cauto	vos = vecobj_search ;
	            cauto	vcf = vcmpfind ;
	            if (void *vp ; (rs = vos(op->mlp,&te,vcf,&vp)) >= 0) {
	                if (vp) {
			    cmdmap_ent	*ep = entp(vp) ;
	                    cmd = ep->cmd ;
	                }
	            } /* end if (vecobj_search) */
	        } /* end if (ok) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? cmd : rs ;
} /* end subroutine (cmdmap_lookup) */


/* private subroutines */

local int cmdmap_defmap(cmdmap *op,const cmdmap_ent *defmap) noex {
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
} /* end subroutine (cmdmap_defmap) */

local int vcmpfind(cvoid **v1pp,cvoid **v2pp) noex {
	cmdmap_ent	*e1p = (cmdmap_ent *) *v1pp ;
	cmdmap_ent	*e2p = (cmdmap_ent *) *v2pp ;
	int		rc = 0 ;
	if (e1p || e2p) ylikely {
	    rc = +1 ;
	    if (e1p) {
		rc = -1 ;
	        if (e2p) {
	            rc = e1p->key - e2p->key ;
	        }
	    }
	} /* end if */
	return rc ;
} /* end subroutine (vcmpfind) */


