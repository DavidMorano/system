/* recip_main SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* recipient object for DMAIL¦DMAILBOX */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-03-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	recip

	Description:
	This is a recipient address (RECIP) handling module object.
	It can parse out and store hierarchically organized RECIPs.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<vecitem.h>
#include	<strwcpy.h>
#include	<strwcmp.h>
#include	<localmisc.h>

#include	"recip.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| */

/* local defines */


/* external namespaces */

using libuc::libmem ;			/* variable */
using std::nothrow ;			/* constant */


/* external typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static inline int recip_ctor(recip *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    op->magic = 0 ;
	    if ((op->mdp = new(nothrow) vecitem) != np) {
		rs = SR_OK ;
	    } /* end if (new-vecitem) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (recip_ctor) */

static inline int recip_dtor(recip *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->mdp) {
		delete op->mdp ;
	        op->mdp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (recip_dtor) */

template<typename ... Args>
static int recip_magic(recip *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == RECIP_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (recip_magic) */


/* local variables */


/* exported variables */


/* exported subroutines */

int recip_start(recip *op,cchar *sp,int sl) noex {
	int		rs ;
	if ((rs = recip_ctor(op,sp)) >= 0) {
	    op->uid = -1 ;
	    if (cchar *cp ; (rs = libmem.strw(sp,sl,&cp)) >= 0) {
	        op->recipient = cp ;
	        if ((rs = vecitem_start(op->mdp,10,0)) >= 0) {
	            op->magic = RECIP_MAGIC ;
	        }
	        if (rs < 0) {
		    void *vp = voidp(cp) ;
	            libmem.free(vp) ;
	            op->recipient = nullptr ;
	        } /* end if (error) */
	    } /* end if (memory-allocation) */
	    if (rs < 0) {
		recip_dtor(op) ;
	    }
	} /* end if (recip_ctor) */
	return rs ;
}
/* end subroutine (recip_start) */

int recip_finish(recip *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = recip_magic(op)) >= 0) {
	    if (op->recipient) {
	        {
		    void *vp = voidp(op->recipient) ;
	            rs1 = libmem.free(vp) ;
	            if (rs >= 0) rs = rs1 ;
	            op->recipient = nullptr ;
		}
	        if (op->name) {
		    void *vp = voidp(op->name) ;
	            rs1 = libmem.free(vp) ;
	            if (rs >= 0) rs = rs1 ;
	            op->name = nullptr ;
	        }
	        if (op->maildname) {
		    void *vp = voidp(op->maildname) ;
	            rs1 = libmem.free(vp) ;
	            if (rs >= 0) rs = rs1 ;
	            op->maildname = nullptr ;
	        }
		{
	            rs1 = vecitem_finish(op->mdp) ;
	            if (rs >= 0) rs = rs1 ;
		}
	    } else {
	        rs = SR_BUGCHECK ;
	    }
	    {
		rs1 = recip_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (recip_finish) */

int recip_get(recip *op,cchar **rpp) noex {
	int		rs ;
	if ((rs = recip_magic(op)) >= 0) {
	    cchar	*rp = nullptr ;
	    if (op->recipient) {
	        rp = op->recipient ;
	        rs = lenstr(rp) ;
	    } else {
	        rs = SR_NOTOPEN ;
	    }
	    if (rpp) {
	        *rpp = rp ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (recip_get) */

int recip_setuser(recip *op,uid_t uid) noex {
	int		rs ;
	if ((rs = recip_magic(op)) >= 0) {
	    op->fl.user = true ;
	    op->uid = uid ;
	}
	return rs ;
}
/* end subroutine (recip_setuser) */

int recip_setname(recip *op,cchar *sp,int sl) noex {
	int		rs ;
	if ((rs = recip_magic(op,sp)) >= 0) {
	    rs = SR_INVALID ;
	    if (op->name != nullptr) {
		void *vp = voidp(op->name) ;
	        libmem.free(vp) ;
	        op->name = nullptr ;
	    }
	    if (sp[0]) {
	        if (sl < 0) sl = lenstr(sp) ;
	        if (cchar *cp ; (rs = libmem.strw(sp,sl,&cp)) >= 0) {
	            op->name = cp ;
	        } /* end if (m-a) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (recip_setuser) */

int recip_setmailspool(recip *op,cchar *sp,int sl) noex {
	int		rs ;
	if ((rs = recip_magic(op,sp)) >= 0) {
	    rs = SR_INVALID ;
	    if (op->maildname) {
		void *vp = voidp(op->maildname) ;
	        libmem.free(vp) ;
	        op->maildname = nullptr ;
	    } /* end if */
	    if (sp[0]) {
	        if (sl < 0) sl = lenstr(sp) ;
	        if (cchar *cp ; (rs = libmem.strw(sp,sl,&cp)) >= 0) {
	            op->maildname = cp ;
	        } /* end if (m-a) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (recip_setmailspool) */

int recip_mbo(recip *op,int mbo) noex {
	int		rs ;
	if ((rs = recip_magic(op)) >= 0) {
	    op->mbo = mbo ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (recip_mbo) */

int recip_mo(recip *op,int moff,int mlen) noex {
	int		rs ;
	if ((rs = recip_magic(op)) >= 0) {
	    recip_ent	mo{} ;
	    cint	esz = szof(recip_ent) ;
	    mo.moff = moff ;
	    mo.mlen = mlen ;
	    op->n += 1 ;
	    rs = vecitem_add(op->mdp,&mo,esz) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (recip_mo) */

int recip_ds(recip *op,int ds) noex {
	int		rs ;
	if ((rs = recip_magic(op)) >= 0) {
	    op->ds = ds ;		/* delivery-status */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (recip_ds) */

int recip_match(recip *op,cchar *sp,int sl) noex {
	int		rs ;
	int		f = false ;
	if ((rs = recip_magic(op,sp)) >= 0) {
	    rs = SR_INVALID ;
	    if (sp[0]) {
		rs = SR_OK ;
	        if (sl < 0) sl = lenstr(sp) ;
	        if (op->recipient) {
	            f = (strwcmp(op->recipient,sp,sl) == 0) ;
	        }
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (recip_match) */

int recip_getmbo(recip *op) noex {
	int		rs ;
	int		mbo = 0 ;
	if ((rs = recip_magic(op)) >= 0) {
	    if (op->ds >= 0) {
	        mbo = op->mbo ;		/* mail-box offset */
	    } else {
	        mbo = op->ds ;		/* delivery-status */
	    }
	} /* end if (magic) */
	return (rs >= 0) ? mbo : rs ;
}
/* end subroutine (recip_getmbo) */

int recip_getmo(recip *op,int i,int *offp) noex {
	int		rs ;
	int		ml = 0 ;
	if ((rs = recip_magic(op)) >= 0) {
	    int		mo = 0 ;
	    if (op->ds >= 0) {
	        recip_ent	*ep ;
	        if ((rs = vecitem_get(op->mdp,i,&ep)) >= 0) {
	            mo = ep->moff ;
	            ml = ep->mlen ;
	        }
	    } else {
	        rs = op->ds ;
	    }
	    if (offp) {
	        *offp = (rs >= 0) ? mo : 0 ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? ml : rs ;
}
/* end subroutine (recip_getmo) */

int recip_getname(recip *op,cchar **rpp) noex {
	int		rs ;
	if ((rs = recip_magic(op)) >= 0) {
	    if (rpp) {
	        *rpp = op->name ;
	    }
	    if (op->name) {
	        rs = lenstr(op->name) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (recip_getname) */

int recip_getmailspool(recip *op,cchar **rpp) noex {
	int		rs ;
	if ((rs = recip_magic(op)) >= 0) {
	    if (rpp) {
	        *rpp = op->maildname ;
	    }
	    if (op->maildname) {
	        rs = lenstr(op->maildname) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (recip_getmailspool) */

int recip_isuser(recip *op) noex {
	int		rs ;
	int		f = false ;
	if ((rs = recip_magic(op)) >= 0) {
	    f = op->fl.user ;
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (recip_isuser) */

int recip_getuser(recip *op,uid_t *up) noex {
	int		rs ;
	int		f = false ;
	if ((rs = recip_magic(op)) >= 0) {
	    if (up) *up = op->uid ;
	    f = op->fl.user ;
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (recip_getuser) */


