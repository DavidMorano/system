/* ema_main SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* E-Mail Address */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	ema

	Description:
	This is an email address handling module object.  It can
	parse out and store hierarchically organized EMAs.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<mallocstuff.h>
#include	<strn.h>
#include	<vechand.h>
#include	<ascii.h>
#include	<mkchar.h>
#include	<char.h>
#include	<localmisc.h>

#include	"ema.h"

#pragma		GCC dependency	"mod/libutil.ccm"
#pragma		GCC dependency	"ema_entry.ccm"
#pragma		GCC dependency	"ema_asstr.ccm"
#pragma		GCC dependency	"ema_parts.ccm"

import libutil ;
import ema_entry ;
import ema_asstr ;
import ema_parts ;

/* local defines */

#define	EMA_DEFENTS	4		/* default number of entries */


/* imported namespaces */

using std::nothrow ;			/* constant */


/* local typedefs */

typedef ema_ent		ent ;
typedef ema_ent	*	entp ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static inline int ema_ctor(ema *op,Args ... args) noex {
    	EMA		*hop = op ;
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->elp = new(nothrow) vechand) != np) {
		rs = SR_OK ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (memfile_ctor) */

static inline int ema_dtor(ema *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->elp) {
		delete op->elp ;
		op->elp = nullptr ;
	    }
	}
	return rs ;
} /* end subroutine (memfile_dtor) */

namespace ema_ns {
    extern int	ema_parseit(ema *,asstr *) noex ;
}

local int	ema_addentone(ema *,ema_ent *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int ema_start(ema *op) noex {
	int		rs ;
	if ((rs = ema_ctor(op)) >= 0) {
	    cint	vn = EMA_DEFENTS ;
	    cint	vo = 0 ;
	    if ((rs = vechand_start(op->elp,vn,vo)) >= 0) {
	        op->magic = EMA_MAGIC ;
	    }
	    if (rs < 0) {
		ema_dtor(op) ;
	    }
	} /* end if (ema_ctor) */
	return rs ;
}
/* end subroutine (ema_start) */

int ema_finish(ema *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = ema_magic(op)) >= 0) {
	    op->n = -1 ;
	    if (op->elp) {
	        void	*vp{} ;
	        for (int i = 0 ; vechand_get(op->elp,i,&vp) >= 0 ; i += 1) {
		    ema_ent	*ep = entp(vp) ;
	            if (vp) {
		        {
	                    rs1 = entry_finish(ep) ;
	                    if (rs >= 0) rs = rs1 ;
		        }
		        {
	                    rs1 = uc_free(ep) ;
	                    if (rs >= 0) rs = rs1 ;
		        }
	            } /* end if (non-null) */
	        } /* end for */
	        {
	            rs1 = vechand_finish(op->elp) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	    } /* end if (non-null) */
	    {
	        rs1 = ema_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ema_finish) */

int ema_parse(ema *op,cchar *sp,int sl) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = ema_magic(op,sp)) >= 0) {
	    if (sl < 0) sl = lenstr(sp) ;
	    if (asstr desc ; (rs = desc.start(sp,sl)) >= 0) {
	        if ((rs = ema_ns::ema_parseit(op,&desc)) > 0) {
	            op->n += rs ;
	        }
		rs1 = desc.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ema_parse) */

/* whew! this is not easy to implement (like any of this was)! */
int ema_addent(ema *op,ema_ent *ep) noex {
	int		rs ;
	if ((rs = ema_magic(op)) >= 0) {
	    rs = ema_addentone(op,ep) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ema_addent) */

/* get the ema under the current cursor */
int ema_get(ema *op,int i,ema_ent **epp) noex {
	int		rs ;
	if ((rs = ema_magic(op,epp)) >= 0) {
	    if (void *vp ; (rs = vechand_get(op->elp,i,&vp)) >= 0) {
	        *epp = entp(vp) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ema_get) */

int ema_getbestaddr(ema *op,int i,cchar **rpp) noex {
	int		rs ;
	int		rl = 0 ;
	if ((rs = ema_magic(op)) >= 0) {
	    cchar	*rp = nullptr ;
	    if (void *vp ; (rs = vechand_get(op->elp,i,&vp)) >= 0) {
		ema_ent		*ep = entp(vp) ;
	        if (vp) {
	    	    if ((rl == 0) && (ep->rp != nullptr) && (ep->rl > 0)) {
		        rp = ep->rp ;
		        rl = ep->rl ;
		    }
	    	    if ((rl == 0) && (ep->ap != nullptr) && (ep->al > 0)) {
		        rp = ep->ap ;
		        rl = ep->al ;
		    }
	        } /* end if (non-null) */
	    } /* end if (get) */
	    if (rpp) {
	        *rpp = (rs >= 0) ? rp : nullptr ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (ema_getbestaddr) */

/* return the number of EMAs we have so far */
int ema_count(ema *op) noex {
	int		rs ;
	if ((rs = ema_magic(op)) >= 0) {
	    rs = vechand_count(op->elp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ema_count) */

int ema_addents(ema *op,ema *oop) noex {
	int		rs ;
	if ((rs = ema_magic(op)) >= 0) {
	    vechand	*elp = oop->elp ;
	    void	*vp{} ;
	    for (int i = 0 ; elp->get(i,&vp) >= 0 ; i += 1) {
	        ema_ent	*oep = entp(vp) ;
	        rs = ema_addentone(op,oep) ;
	        if (rs < 0) break ;
	    } /* end for */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ema_addents) */


/* private subroutines */

namespace ema_ns {
    int ema_starter(ema *op,cchar *sp,int sl) noex {
	int		rs ;
	if ((rs = ema_start(op)) >= 0) {
	    if (sp) {
	        rs = ema_parse(op,sp,sl) ;
	    } /* end if (optional data) */
	    if (rs < 0) {
	        ema_finish(op) ;
	    }
	} /* end if (ema_start) */
	return rs ;
   } /* end subroutine (ema_starter) */
} /* end namespace (ema_ns) */

local int ema_addentone(ema *op,ema_ent *ep) noex {
	cint		sz = szof(ema_ent) ;
	int		rs ;
	if (void *vp ; (rs = uc_malloc(sz,&vp)) >= 0) {
	    ema_ent *nep = entp(vp) ;
	    if ((rs = entry_startload(nep,ep)) >= 0) {
	        if ((rs = vechand_add(op->elp,nep)) >= 0) {
	            op->n += 1 ;
	        }
	        if (rs < 0) {
	            entry_finish(nep) ;
		}
	    } /* end if (entry_startload) */
	    if (rs < 0) {
	        uc_free(nep) ;
	    }
	} /* end if (memory-allocation) */
	return rs ;
}
/* end subroutine (ema_addentone) */


