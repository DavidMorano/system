/* dirseen SUPPORT */
/* encoding=ISO8859-1 */
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
#include	<sys/stat.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<nulstr.h>
#include	<sncpyx.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"dirseen.h"


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
	cchar		*name ;
	ino_t		ino ;
	dev_t		dev ;
	int		namelen ;
	dirseen_ent() noex : name(nullptr), namelen(0), dev(0L), ino(0L) { } ;
	dirseen_ent(cchar *sp,int sl = -1) noex : name(sp) {
	    if (sl < 0) sl = strlen(sp) ;
	    namelen = sl ;
	} ;
	dirseen_ent(dev_t d,ino_t i) noex : dev(d), ino(i) { } ;
    } ; /* end struct (dirseen_ent) */
}

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
	if (op) {
	    rs = SR_NOMEM ;
	    op->magic = 0 ;
	    op->strsize = 0 ;
	    if ((op->dlistp = new(nothrow) vecobj) != nullptr) {
		cint	vn = DIRSEEN_NDEF ;
	        cint	vo = 0 ;
	        cint	esz = szof(dirseen_ent) ;
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
	if ((rs = dirseen_magic(op)) >= 0) {
	    rs = SR_BUGCHECK ;
	    if (op->dlistp) {
	        void	*vp{} ;
	        for (int i = 0 ; vecobj_get(op->dlistp,i,&vp) >= 0 ; i += 1) {
	            if (vp) {
	                dirseen_ent	*ep = entp(vp) ;
	                rs1 = entry_finish(ep) ;
	                if (rs >= 0) rs = rs1 ;
	            }
	        } /* end for */
	        {
	            rs1 = vecobj_finish(op->dlistp) ;
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

int dirseen_add(dirseen *op,cchar *sp,int sl,USTAT *sbp) noex {
	int		rs ;
	if ((rs = dirseen_magic(op,sp,sbp)) >= 0) {
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
	            if (pl < 0) pl = strlen(pp) ;
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
	if ((rs = dirseen_magic(op,sp)) >= 0) {
	    dirseen_ent		e(sp,sl) ;
	    rs = vecobj_search(op->dlistp,&e,vcmpname,nullptr) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirseen_havename) */

int dirseen_havedevino(dirseen *op,USTAT *sbp) noex {
	int		rs ;
	if ((rs = dirseen_magic(op,sbp)) >= 0) {
	    dirseen_ent		e(sbp->st_dev,sbp->st_ino) ;
	    rs = vecobj_search(op->dlistp,&e,vcmpdevino,nullptr) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirseen_havedevino) */

int dirseen_count(dirseen *op) noex {
	int		rs ;
	if ((rs = dirseen_magic(op)) >= 0) {
	    rs = vecobj_count(op->dlistp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirseen_count) */

int dirseen_curbegin(dirseen *op,dirseen_cur *curp) noex {
	int		rs ;
	if ((rs = dirseen_magic(op,curp)) >= 0) {
	    memclear(curp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirseen_curbegin) */

int dirseen_curend(dirseen *op,dirseen_cur *curp) noex {
	int		rs ;
	if ((rs = dirseen_magic(op,curp)) >= 0) {
	    memclear(curp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirseen_curend) */

int dirseen_curenum(dirseen *op,dirseen_cur *curp,char *rbuf,int rlen) noex {
	int		rs ;
	if ((rs = dirseen_magic(op,curp,rbuf)) >= 0) {
	    cint	i = (curp->i >= 0) ? curp->i : 0 ;
	    void	*vp{} ;
	    while ((rs = vecobj_get(op->dlistp,i,&vp)) >= 0) {
	        if (vp) break ;
	    } /* end while */
	    if ((rs >= 0) && vp) {
	        dirseen_ent	*ep = entp(vp) ;
	        if ((rs = sncpy1(rbuf,rlen,ep->name)) >= 0) {
	            curp->i = (i + 1) ;
	        }
	    } /* end if (got one) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirseen_curenum) */


/* private subroutines */

int dirseen::add(cchar *sp,int sl,USTAT *sbp) noex {
	return dirseen_add(this,sp,sl,sbp) ;
}

int dirseen::havename(cchar *sp,int sl) noex {
	return dirseen_havename(this,sp,sl) ;
}

int dirseen::havedevino(USTAT *sbp) noex {
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
	if (cchar *cp{} ; (rs = uc_mallocstrw(sp,sl,&cp)) >= 0) {
	    ep->name = cp ;
	    ep->namelen = rs ;
	}
	return rs ;
}
/* end subroutine (entry_start) */

int entry_finish(dirseen_ent *ep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (ep) {
	    rs = SR_OK ;
	    if (ep->name) {
	        rs1 = uc_free(ep->name) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->name = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entry_finish) */

static int vcmpname(cvoid **v1pp,cvoid **v2pp) noex {
	dirseen_ent	**e1pp = entpp(v1pp) ;
	dirseen_ent	**e2pp = entpp(v2pp) ;
	int		rc = 0 ;
	{
	    dirseen_ent	*e1p = *e1pp ;
	    dirseen_ent	*e2p = *e2pp ;
	    if (e1p || e2p) {
	        rc = 1 ;
	        if (e1p) {
		    rc = -1 ;
	            if (e2p) {
		        cint	ml = min(e1p->namelen,e2p->namelen) ;
			cint	ch1 = mkchar(e1p->name[0]) ;
			cint	ch2 = mkchar(e2p->name[0]) ;
			if ((rc = (ch1 - ch2)) == 0) {
	                    rc = strncmp(e1p->name,e2p->name,ml) ;
			}
		    }
	        }
	    } /* end if */
	} /* end block */
	return rc ;
}
/* end subroutine (vcmpname) */

static int vcmpdevino(cvoid **v1pp,cvoid **v2pp) noex {
	dirseen_ent	**e1pp = entpp(v1pp) ;
	dirseen_ent	**e2pp = entpp(v2pp) ;
	int		rc = 0 ;
	{
	    dirseen_ent	*e1p = *e1pp ;
	    dirseen_ent	*e2p = *e2pp ;
	    if (e1p || e2p) {
	        rc = 1 ;
	        if (e1p) {
		    rc = -1 ;
	            if (e2p) {
			dev_t	dev = (e1p->dev - e2p->dev) ;
			rc = 0 ;
			if (dev == 0) {
			    ino_t	ino = (e1p->ino - e2p->ino) ;
			    if (ino) {
			        rc = (ino > 0) ? +1 : -1 ;
			    }
			} else {
			    rc = (dev > 0) ? +1 : -1 ;
			}
		    }
	        }
	    } /* end if */
	} /* end block */
	return rc ;
}
/* end subroutine (vcmpdevino) */


