/* dirseen SUPPORT */
/* lang=C++20 */

/* directory list manager */
/* version %I% last-modified %G% */


/* revision history:

	= 2006-09-10, David A­D­ Morano
	I created this from hacking something that was similar that
	was originally created for a PCS program.

*/

/* Copyright © 2006 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object manages directory lists by:
	+ ensuring unique entries by name
	+ ensuring unique entries by dev-inode pair

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<usupport.h>
#include	<nulstr.h>
#include	<sncpyx.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"dirseen.h"


/* local defines */

#define	DIRSEEN_ENT	struct dirseen_e


/* local namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef DIRSEEN_ENT	dirseen_ent ;
typedef DIRSEEN_ENT	ent ;
typedef DIRSEEN_ENT	*entp ;
typedef DIRSEEN_ENT	**entpp ;


/* external subroutines */


/* local structures */

struct dirseen_e {
	cchar		*name ;
	ino_t		ino ;
	dev_t		dev ;
	int		namelen ;
	uint		f_stat:1 ;
} ;


/* forward references */

template<typename ... Args>
static inline int dirseen_magic(dirseen *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == DIRSEEN_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (dirseen_magic) */

static int entry_start(dirseen_ent *,cchar *,int,dev_t,ino_t) noex ;
static int entry_finish(dirseen_ent *) noex ;

static int vcmpname(cvoid **,cvoid **) noex ;
static int vcmpdevino(cvoid **,cvoid **) noex ;


/* local variables */


/* exported subroutines */

int dirseen_start(dirseen *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOMEM ;
	    op->magic = 0 ;
	    op->strsize = 0 ;
	    if ((op->dlistp = new(nothrow) vecobj) != nullptr) {
	        cint	vo = 0 ;
	        cint	esz = sizeof(dirseen_ent) ;
	        if ((rs = vecobj_start(op->dlistp,esz,DIRSEEN_NDEF,vo)) >= 0) {
	            op->magic = DIRSEEN_MAGIC ;
	        }
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
	    if (op->dlistp) {
		delete op->dlistp ;
		op->dlistp = nullptr ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirseen_finish) */

int dirseen_add(dirseen *op,cchar *dbuf,int dlen,USTAT *sbp) noex {
	int		rs ;
	if ((rs = dirseen_magic(op,dbuf,sbp)) >= 0) {
	    dirseen_ent	e ;
	    dev_t	dev = 0 ;
	    ino_t	ino = 0 ;
	    int		pl ;
	    cchar	*pp ;
	    if (dlen < 0) dlen = strlen(dbuf) ;
/* any NUL (blank) paths need to be converted to "." */
	    pp = dbuf ;
	    pl = dlen ;
	    if (pl == 0) {
	        pp = "." ;
	        pl = 1 ;
	    }
/* enter it */
	    if (sbp != nullptr) {
	        dev = sbp->st_dev ;
	        ino = sbp->st_ino ;
	    }
	    if ((rs = entry_start(&e,pp,pl,dev,ino)) >= 0) {
	        op->strsize += rs ;
	        rs = vecobj_add(op->dlistp,&e) ;
	        if (rs < 0) {
		    entry_finish(&e) ;
		}
	    } /* end if (entry_start) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirseen_add) */

int dirseen_havename(dirseen *op,cchar *np,int nl) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = dirseen_magic(op,np)) >= 0) {
	    nulstr	s ;
	    cchar	*name = nullptr ;
	    if ((rs = nulstr_start(&s,np,nl,&name)) >= 0) {
	        {
	            dirseen_ent	e ;
	            e.name = name ;
	            rs = vecobj_search(op->dlistp,&e,vcmpname,nullptr) ;
	        }
	        rs1 = nulstr_finish(&s) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (nulstr) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirseen_havename) */

int dirseen_havedevino(dirseen *op,USTAT *sbp) noex {
	int		rs ;
	if ((rs = dirseen_magic(op,sbp)) >= 0) {
	    dirseen_ent	e ;
	    e.dev = sbp->st_dev ;
	    e.ino = sbp->st_ino ;
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

int dirseen_enum(dirseen *op,dirseen_cur *curp,char *rbuf,int rlen) noex {
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
/* end subroutine (dirseen_enum) */


/* private subroutines */

int entry_start(dirseen_ent *ep,cchar *np,int nl,dev_t dev,ino_t ino) noex {
	int		rs ;
	cchar	*cp ;
	memclear(ep) ;
	ep->dev = dev ;
	ep->ino = ino ;
	if ((rs = uc_mallocstrw(np,nl,&cp)) >= 0) {
	    ep->name = cp ;
	    ep->namelen = (rs-1) ;
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
			cint	ch1 = mkchar(e1p->name[0]) ;
			cint	ch2 = mkchar(e2p->name[0]) ;
			if ((rc = (ch1 - ch2)) == 0) {
	                    rc = strcmp(e1p->name,e2p->name) ;
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
			ino_t	ino = (e1p->ino - e2p->ino) ;
			rc = 0 ;
			if (ino == 0) {
			    dev_t	dev = (e1p->dev - e2p->dev) ;
			    if (dev == 0) {
				rc = 0 ;
			    } else {
			        rc = (dev > 0) ? +1 : -1 ;
			    }
			} else {
			    rc = (ino > 0) ? +1 : -1 ;
			}
		    }
	        }
	    } /* end if */
	} /* end block */
	return rc ;
}
/* end subroutine (vcmpdevino) */


