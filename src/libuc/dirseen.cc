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

using std::nullptr_t ;


/* local typedefs */

typedef DIRSEEN_ENT	ent ;
typedef DIRSEEN_ENT	*entp ;
typedef DIRSEEN_ENT	**entpp ;


/* external subroutines */


/* local structures */

struct dirseen_e {
	cchar		*name ;
	uino_t		ino ;
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

static int entry_start(DIRSEEN_ENT *,cchar *,int,dev_t,uino_t) noex ;
static int entry_finish(DIRSEEN_ENT *) noex ;

static int vcmpname(cvoid **,cvoid **) noex ;
static int vcmpdevino(cvoid **,cvoid **) noex ;


/* local variables */


/* exported subroutines */

int dirseen_start(DIRSEEN *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    cint	vo = 0 ;
	    cint	esz = sizeof(DIRSEEN_ENT) ;
	    memclear(op) ;
	    if ((rs = vecobj_start(&op->dlist,esz,DIRSEEN_NDEF,vo)) >= 0) {
	        op->magic = DIRSEEN_MAGIC ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (dirseen_start) */

int dirseen_finish(DIRSEEN *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = dirseen_magic(op)) >= 0) {
	    void	*vp{} ;
	    for (int i = 0 ; vecobj_get(&op->dlist,i,&vp) >= 0 ; i += 1) {
	        if (vp) {
	            DIRSEEN_ENT	*ep = entp(vp) ;
	            rs1 = entry_finish(ep) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	    } /* end for */
	    {
	        rs1 = vecobj_finish(&op->dlist) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirseen_finish) */

int dirseen_add(DIRSEEN *op,cchar *dbuf,int dlen,USTAT *sbp) noex {
	int		rs ;
	if ((rs = dirseen_magic(op,dbuf,sbp)) >= 0) {
	    DIRSEEN_ENT	e ;
	    dev_t	dev = 0 ;
	    uino_t	ino = 0 ;
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
	        rs = vecobj_add(&op->dlist,&e) ;
	        if (rs < 0) {
		    entry_finish(&e) ;
		}
	    } /* end if (entry_start) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirseen_add) */

int dirseen_havename(DIRSEEN *op,cchar *np,int nl) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = dirseen_magic(op,np)) >= 0) {
	    nulstr	s ;
	    cchar	*name = nullptr ;
	    if ((rs = nulstr_start(&s,np,nl,&name)) >= 0) {
	        {
	            DIRSEEN_ENT	e ;
	            e.name = name ;
	            rs = vecobj_search(&op->dlist,&e,vcmpname,nullptr) ;
	        }
	        rs1 = nulstr_finish(&s) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (nulstr) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirseen_havename) */

int dirseen_havedevino(DIRSEEN *op,USTAT *sbp) noex {
	int		rs ;
	if ((rs = dirseen_magic(op,sbp)) >= 0) {
	    DIRSEEN_ENT	e ;
	    e.dev = sbp->st_dev ;
	    e.ino = sbp->st_ino ;
	    rs = vecobj_search(&op->dlist,&e,vcmpdevino,nullptr) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirseen_havedevino) */

int dirseen_count(DIRSEEN *op) noex {
	int		rs ;
	if ((rs = dirseen_magic(op)) >= 0) {
	    rs = vecobj_count(&op->dlist) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirseen_count) */

int dirseen_curbegin(DIRSEEN *op,DIRSEEN_CUR *curp) noex {
	int		rs ;
	if ((rs = dirseen_magic(op,curp)) >= 0) {
	    memclear(curp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirseen_curbegin) */

int dirseen_curend(DIRSEEN *op,DIRSEEN_CUR *curp) noex {
	int		rs ;
	if ((rs = dirseen_magic(op,curp)) >= 0) {
	    memclear(curp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirseen_curend) */

int dirseen_enum(DIRSEEN *op,DIRSEEN_CUR *curp,char *rbuf,int rlen) noex {
	int		rs ;
	if ((rs = dirseen_magic(op,curp,rbuf)) >= 0) {
	    int		i = (curp->i >= 0) ? curp->i : 0 ;
	    void	*vp{} ;
	    while ((rs = vecobj_get(&op->dlist,i,&vp)) >= 0) {
	        if (vp) break ;
	    } /* end while */
	    if ((rs >= 0) && vp) {
	        DIRSEEN_ENT	*ep = entp(vp) ;
	        if ((rs = sncpy1(rbuf,rlen,ep->name)) >= 0) {
	            curp->i = (i + 1) ;
	        }
	    } /* end if (got one) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirseen_enum) */


/* private subroutines */

int entry_start(DIRSEEN_ENT *ep,cchar *np,int nl,dev_t dev,uino_t ino) noex {
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

int entry_finish(DIRSEEN_ENT *ep) noex {
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
	DIRSEEN_ENT	**e1pp = entpp(v1pp) ;
	DIRSEEN_ENT	**e2pp = entpp(v2pp) ;
	int		rc = 0 ;
	{
	    DIRSEEN_ENT	*e1p = *e1pp ;
	    DIRSEEN_ENT	*e2p = *e2pp ;
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
	DIRSEEN_ENT	**e1pp = entpp(v1pp) ;
	DIRSEEN_ENT	**e2pp = entpp(v2pp) ;
	int		rc = 0 ;
	{
	    DIRSEEN_ENT	*e1p = *e1pp ;
	    DIRSEEN_ENT	*e2p = *e2pp ;
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


