/* dirseen_main SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* unique directory manager */
/* version %I% last-modified %G% */


/* revision history:

	= 2006-09-10, David A­D­ Morano
	I created this from hacking something that was similar that
	was originally created for a PCS program.

*/

/* Copyright © 2006 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	dirseen

	Description:
	This object manages directory lists by:
	+ ensuring unique entries by name
	+ ensuring unique entries by dev-inode pair

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>		/* |ustat| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<new>			/* |nothrow(3c++)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<nulstr.h>
#include	<sncpyx.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"dirseen.h"

import libutil ;

/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct dirseen_ent {
	cchar		*namep ;
	ino_t		ino ;
	dev_t		dev ;
	int		namel ;
	dirseen_ent() noex : namep(nullptr), namel(0), dev(0L), ino(0L) { } ;
	dirseen_ent(cchar *sp,int sl = -1) noex : namep(sp) {
	    if (sl < 0) sl = lenstr(sp) ;
	    namel = sl ;
	} ;
	dirseen_ent(dev_t d,ino_t i) noex : dev(d), ino(i) { } ;
    } ; /* end struct (dirseen_ent) */
} /* end namespace */

typedef dirseen_ent	ent ;
typedef dirseen_ent	*entp ;
typedef dirseen_ent	**entpp ;


/* forward references */

static int entry_start(dirseen_ent *,cchar *,int,dev_t,ino_t) noex ;
static int entry_finish(dirseen_ent *) noex ;

extern "C" {
    static int vcmpname(cvoid **,cvoid **) noex ;
    static int vcmpdevino(cvoid **,cvoid **) noex ;
}


/* local variables */


/* exported variables */


/* exported subroutines */

int dirseen_start(dirseen *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_NOMEM ;
	    op->magic = 0 ;
	    op->strsize = 0 ;
	    if ((op->dlistp = new(nothrow) vecobj) != nullptr) {
	        cint	esz = szof(dirseen_ent) ;
		cint	vn = DIRSEEN_NDEF ;
	        cint	vo = 0 ;
	        if ((rs = vecobj_start(op->dlistp,esz,vn,vo)) >= 0) {
	            op->magic = DIRSEEN_MAGIC ;
	        } /* end if  */
		if (rs < 0) {
		    delete op->dlistp ;
		    op->dlistp = nullptr ;
		}
	    } /* end if (new-vecobj) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (dirseen_start) */

int dirseen_finish(dirseen *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = dirseen_magic(op)) >= 0) ylikely {
	    rs = SR_BUGCHECK ;
	    if (op->dlistp) {
		vecobj	*dlp = op->dlistp ;
	        void	*vp{} ;
	        for (int i = 0 ; dlp->get(i,&vp) >= 0 ; i += 1) {
	            dirseen_ent		*ep = entp(vp) ;
	            if (vp) {
	                rs1 = entry_finish(ep) ;
	                if (rs >= 0) rs = rs1 ;
	            }
	        } /* end for */
	        {
	            rs1 = dlp->finish ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        {
		    delete op->dlistp ;
		    op->dlistp = nullptr ;
	        }
	    } /* end if (bugcheck) */
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirseen_finish) */

int dirseen_add(dirseen *op,cchar *sp,int sl,ustat *sbp) noex {
	int		rs ;
	if ((rs = dirseen_magic(op,sp,sbp)) >= 0) ylikely {
	    dirseen_ent		e(sbp->st_dev,sbp->st_ino) ;
	    cnullptr		np{} ;
	    cint		rsn = SR_NOTFOUND ;
	    if ((rs = vecobj_search(op->dlistp,&e,vcmpdevino,np)) == rsn) {
	        dev_t	dev = sbp->st_dev ;
	        ino_t	ino = sbp->st_ino ;
	        int	pl = sl ;
	        cchar	*pp = sp ;
		/* any NUL (blank) paths need to be converted to "." */
	        if (pl == 0) {
	            pp = "." ;
	            pl = 1 ;
	        } else {
	            if (pl < 0) pl = lenstr(pp) ;
	 	}
	        if ((rs = entry_start(&e,pp,pl,dev,ino)) >= 0) {
	            op->strsize += rs ;
	            rs = vecobj_add(op->dlistp,&e) ;
	            if (rs < 0) {
		        entry_finish(&e) ;
		    }
	        } /* end if (entry_start) */
	    } /* end if (vecobj_search) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirseen_add) */

int dirseen_havename(dirseen *op,cchar *sp,int sl) noex {
	int		rs ;
	if ((rs = dirseen_magic(op,sp)) >= 0) ylikely {
	    dirseen_ent		e(sp,sl) ;
	    rs = vecobj_search(op->dlistp,&e,vcmpname,nullptr) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirseen_havename) */

int dirseen_havedevino(dirseen *op,ustat *sbp) noex {
	int		rs ;
	if ((rs = dirseen_magic(op,sbp)) >= 0) ylikely {
	    dirseen_ent		e(sbp->st_dev,sbp->st_ino) ;
	    rs = vecobj_search(op->dlistp,&e,vcmpdevino,nullptr) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirseen_havedevino) */

int dirseen_count(dirseen *op) noex {
	int		rs ;
	if ((rs = dirseen_magic(op)) >= 0) ylikely {
	    rs = vecobj_count(op->dlistp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirseen_count) */

int dirseen_curbegin(dirseen *op,dirseen_cur *curp) noex {
	int		rs ;
	if ((rs = dirseen_magic(op,curp)) >= 0) ylikely {
	    memclear(curp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirseen_curbegin) */

int dirseen_curend(dirseen *op,dirseen_cur *curp) noex {
	int		rs ;
	if ((rs = dirseen_magic(op,curp)) >= 0) ylikely {
	    memclear(curp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirseen_curend) */

int dirseen_curenum(dirseen *op,dirseen_cur *curp,char *rbuf,int rlen) noex {
	int		rs ;
	if ((rs = dirseen_magic(op,curp,rbuf)) >= 0) ylikely {
	    cint	i = (curp->i >= 0) ? curp->i : 0 ;
	    void	*vp{} ;
	    while ((rs = vecobj_get(op->dlistp,i,&vp)) >= 0) {
	        if (vp) break ;
	    } /* end while */
	    if ((rs >= 0) && vp) {
	        dirseen_ent	*ep = entp(vp) ;
	        if ((rs = sncpy1(rbuf,rlen,ep->namep)) >= 0) {
	            curp->i = (i + 1) ;
	        }
	    } /* end if (got one) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirseen_curenum) */


/* private subroutines */

int dirseen::add(cchar *sp,int sl,ustat *sbp) noex {
	return dirseen_add(this,sp,sl,sbp) ;
}

int dirseen::havename(cchar *sp,int sl) noex {
	return dirseen_havename(this,sp,sl) ;
}

int dirseen::havedevino(ustat *sbp) noex {
	return dirseen_havedevino(this,sbp) ;
}

void dirseen::dtor() noex {
	if (int rs ; (rs = finish) < 0) {
	    ulogerror("dirseen",rs,"dtor-finish") ;
	}
}

int dirseen_co::operator () () noex {
	int		rs = SR_BUGCHECK ;
	switch (w) {
	case dirseenmem_start:
	    rs = dirseen_start(op) ;
	    break ;
	case dirseenmem_count:
	    rs = dirseen_count(op) ;
	    break ;
	case dirseenmem_finish:
	    rs = dirseen_finish(op) ;
	    break ;
	} /* end switch */
	return rs ;
}
/* end method (dirseen_co::operator) */

int entry_start(dirseen_ent *ep,cchar *sp,int sl,dev_t dev,ino_t ino) noex {
	int		rs ;
	ep->dev = dev ;
	ep->ino = ino ;
	if (cchar *cp ; (rs = uc_mallocstrw(sp,sl,&cp)) >= 0) ylikely {
	    ep->namep = cp ;
	    ep->namel = rs ;
	}
	return rs ;
}
/* end subroutine (entry_start) */

int entry_finish(dirseen_ent *ep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (ep) {
	    rs = SR_OK ;
	    if (ep->namep) ylikely {
	        rs1 = uc_free(ep->namep) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->namep = nullptr ;
		ep->namel = 0 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entry_finish) */

static int cmpname(const entp e1p,const entp e2p) noex {
        cint    ml = min(e1p->namel,e2p->namel) ;
        cint    ch1 = mkchar(e1p->namep[0]) ;
        cint    ch2 = mkchar(e2p->namep[0]) ;
        int     rc ;
        if ((rc = (ch1 - ch2)) == 0) {
            rc = strncmp(e1p->namep,e2p->namep,ml) ;
        }
        return rc ;
} /* end subroutine (cmpname) */

static int vcmpname(cvoid **v1pp,cvoid **v2pp) noex {
	dirseen_ent	**e1pp = entpp(v1pp) ;
	dirseen_ent	**e2pp = entpp(v2pp) ;
	int		rc = 0 ;
	{
	    dirseen_ent	*e1p = *e1pp ;
	    dirseen_ent	*e2p = *e2pp ;
	    if (e1p || e2p) ylikely {
	        rc = +1 ;
	        if (e1p) {
		    rc = -1 ;
	            if (e2p) {
			rc = cmpname(e1p,e2p) ;
		    }
	        }
	    } /* end if */
	} /* end block */
	return rc ;
}
/* end subroutine (vcmpname) */

static int cmpdevino(const entp e1p,const entp e2p) noex {
        const dev_t	dev = (e1p->dev - e2p->dev) ;
        int		rc = 0 ;
        if (dev == 0) {
            const ino_t       ino = (e1p->ino - e2p->ino) ;
            if (ino) {
                rc = (ino > 0) ? +1 : -1 ;
            }
        } else {
            rc = (dev > 0) ? +1 : -1 ;
        }
	return rc ;
} /* end subroutine (cmpdevino) */

static int vcmpdevino(cvoid **v1pp,cvoid **v2pp) noex {
	dirseen_ent	**e1pp = entpp(v1pp) ;
	dirseen_ent	**e2pp = entpp(v2pp) ;
	int		rc = 0 ;
	{
	    dirseen_ent	*e1p = *e1pp ;
	    dirseen_ent	*e2p = *e2pp ;
	    if (e1p || e2p) ylikely {
	        rc = +1 ;
	        if (e1p) ylikely {
		    rc = -1 ;
	            if (e2p) ylikely {
			rc = cmpdevino(e1p,e2p) ;
		    }
	        }
	    } /* end if */
	} /* end block */
	return rc ;
} /* end subroutine (vcmpdevino) */


