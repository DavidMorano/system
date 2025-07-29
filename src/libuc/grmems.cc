/* grmems SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® group membership access and cache */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-01-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	grmems

	Description:
	This object provides a crude access manager and cache for
	UNIX® group membership lists.

	Implementation note:

	1. This was a rough (and somewhat crude) first attempt at
	this function.  We are fairly efficient at every step but
	there may be one or two places where a sorted map might
	have been useful.  But (so called) "n" is always relatively
	small so using structured sorted maps everywhere might have
	actually slowed things down also.

	2. The use of mapping the stored local 'passwd' data file
	can be changed to simple file reading if the need ever
	arises.  We did not use the FILEMAP object (because at first
	I thought it too simple for the requirement), but this could
	be hacked in later (as indeed it turns out that object is
	appropriate) if you want to make the code look cleaner.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/mman.h>
#include	<grp.h>
#include	<ctime>			/* |time_t| */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strncmp(3c)| */
#include	<new>			/* |nothrow(3c++)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<getax.h>
#include	<recarr.h>
#include	<vecobj.h>
#include	<ascii.h>
#include	<strn.h>
#include	<sncpyx.h>
#include	<strwcpy.h>
#include	<strdcpy.h>
#include	<cfdec.h>
#include	<strwcmp.h>
#include	<sysdbfn.h>		/* |sysdbfn_get(3uc)| */
#include	<isnot.h>
#include	<localmisc.h>

#include	"grmems.h"

import libutil ;

/* local defines */

#define	GRMEMS_DEFMAX		20	/* default maximum entries */
#define	GRMEMS_DEFTTL		(10*60)	/* default time-to-live */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */

#ifndef	STDSORTF_TYPEDEF
#define	STDSORTF_TYPEDEF
extern "C" {
    typedef int	(*stdsort_f)(cvoid *,cvoid *) noex ;
}
#endif


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	int		pagesize ;
	int		grlen ;
	int		usernamelen ;
	int		groupnamelen ;
	operator int () noex ;
    } ; /* end struct (vars) */
}

enum cts {
	ct_miss,
	ct_hit,
	ct_overlast
} ;

struct grmems_rec : pq_ent {
	cchar		**mems ;
	char		*gn ;
	time_t		ti_create ;
	time_t		ti_access ;
	uint		wcount ;
	int		nmems ;
	int		recidx ;
} ;

struct grmems_u {
	cchar		*up ;
	int		ul ;
} ;

struct grmems_ug {
	char		*un ;
	gid_t		gid ;
} ;


/* forward references */

template<typename ... Args>
static int grmems_ctor(grmems *op,Args ... args) noex {
	grmems_head	*hop = cast_static<grmems_head *>(op) ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    cnullptr	np{} ;
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->lrup = new(nothrow) pq) != np) ylikely {
		rs = SR_OK ;
	    } /* end if (new-pq) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (grmems_ctor) */

static int grmems_dtor(grmems *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->lrup) ylikely {
		delete op->lrup ;
		op->lrup = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (grmems_dtor) */

template<typename ... Args>
static int grmems_magic(grmems *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magic == GRMEMS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (grmems_magic) */

static int	grmems_starter(grmems *) noex ;
static int	grmems_fetch(grmems *,grmems_rec **,cchar *,int) noex ;
static int	grmems_mkrec(grmems *,time_t,grmems_rec **,cchar *,int) noex ;
static int	grmems_newrec(grmems *,time_t,grmems_rec **,cchar *,int) noex ;
static int	grmems_recstart(grmems *,time_t,grmems_rec *,cchar *,int) noex ;
static int	grmems_recrefresh(grmems *,time_t,grmems_rec *) noex ;
static int	grmems_recdel(grmems *,grmems_rec *) noex ;
static int	grmems_recaccess(grmems *,time_t,grmems_rec *) noex ;
static int	grmems_recrear(grmems *,grmems_rec *) noex ;
static int	grmems_recfins(grmems *) noex ;
static int	grmems_upstats(grmems *,int,int) noex ;

static int	grmems_mkug(grmems *,time_t) noex ;
static int	grmems_mkugload(grmems *,time_t,vecobj *) noex ;
static int	grmems_mkugstore(grmems *,time_t,vecobj *) noex ;
static int	grmems_mkugdeplete(grmems *) noex ;

static int	grmems_recusers(grmems *,time_t,vecobj *,gid_t) noex ;

static int	grmems_pwmapbegin(grmems *,time_t,cchar *) noex ;
static int	grmems_pwmapend(grmems *) noex ;

static int record_start(grmems_rec *,time_t,int,vecobj *,ucentgr *) noex ;
static int record_loadgruns(grmems_rec *,vecobj *,ucentgr *) noex ;
static int record_access(grmems_rec *,time_t) noex ;
static int record_refresh(grmems_rec *,time_t,int,vecobj *,ucentgr *) noex ;
static int record_mems(grmems_rec *,time_t,int,vecobj *,ucentgr *) noex ;
static int record_isold(grmems_rec *,time_t,int) noex ;
static int record_getgnp(grmems_rec *,cchar **) noex ;
static int record_finish(grmems_rec *) noex ;

static int	usergid_start(grmems_ug *,cchar *,int,gid_t) noex ;
static int	usergid_finish(grmems_ug *) noex ;

static int	pwentparse(cchar *,int,gid_t *) noex ;

static int	vecobj_addouruniq(vecobj *,grmems_u *) noex ;

static int	cmpug(cvoid *,cvoid *) noex ;

extern "C" {
    static int	vcmpug(cvoid **,cvoid **) noex ;
}


/* local variables */

static vars	var ;


/* exported variables */


/* exported subroutines */

int grmems_start(grmems *op,int nmax,int ttl) noex {
	int		rs ;
	if (nmax < GRMEMS_DEFMAX) nmax = GRMEMS_DEFMAX ;
	if (ttl < GRMEMS_DEFTTL) ttl = GRMEMS_DEFTTL ;
	if ((rs = grmems_ctor(op)) >= 0) {
	    static cint		rsv = var ;
	    if ((rs = rsv) >= 0) ylikely {
	        op->fd = -1 ;
	        op->pagesize = var.pagesize ;
	        if ((rs = pq_start(op->lrup)) >= 0) ylikely {
	            op->nmax = nmax ;
	            op->ttl = ttl ;
	            op->magic = GRMEMS_MAGIC ;
	        }
	    } /* end if (vars) */
	    if (rs < 0) {
		grmems_dtor(op) ;
	    }
	} /* end if (grmems_ctor) */
	return rs ;
}
/* end subroutine (grmems_start) */

int grmems_finish(grmems *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = grmems_magic(op)) >= 0) ylikely {
	    if (op->usergids) {
		{
		    rs1 = grmems_mkugdeplete(op) ;
	            if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = uc_free(op->usergids) ;
	            if (rs >= 0) rs = rs1 ;
	            op->usergids = nullptr ;
		}
	    } /* end if (usergids) */
	    {
	        rs1 = grmems_recfins(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->recs) {
	        {
	            rs1 = recarr_finish(op->recs) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        {
	            rs1 = uc_free(op->recs) ;
	            if (rs >= 0) rs = rs1 ;
	            op->recs = nullptr ;
	        }
	    }
	    {
	        rs1 = pq_finish(op->lrup) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = grmems_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (grmems_finish) */

int grmems_curbegin(grmems *op,grmems_cur *curp) noex {
	int		rs ;
	if ((rs = grmems_magic(op,curp)) >= 0) ylikely {
	    rs = memclear(curp) ;	/* dangerous */
	    curp->ri = -1 ;
	    curp->i = -1 ;
	    op->cursors += 1 ;
	    curp->magic = GRMEMS_CURMAGIC ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (grmems_curbegin) */

int grmems_curend(grmems *op,grmems_cur *curp) noex {
	int		rs ;
	if ((rs = grmems_magic(op,curp)) >= 0) ylikely {
	    rs = SR_NOTOPEN ;
	    if (curp->magic == GRMEMS_CURMAGIC) ylikely {
		rs = SR_OK ;
	        if (op->cursors > 0) op->cursors -= 1 ;
	        curp->ri = -1 ;
	        curp->i = -1 ;
	        curp->magic = 0 ;
	    } /* end if (magic) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (grmems_curend) */

int grmems_lookup(grmems *op,grmems_cur *curp,cchar *gnp,int gnl) noex {
	int		rs ;
	int		ri = 0 ;
	if ((rs = grmems_magic(op,curp,gnp)) >= 0) ylikely {
	    rs = SR_NOTOPEN ;
	    if (curp->magic == GRMEMS_CURMAGIC) ylikely {
		rs = SR_INVALID ;
	        if (gnp[0]) {
		    rs = SR_OK ;
	            if (op->recs == nullptr) {
	                rs = grmems_starter(op) ;
	            }
	            if (rs >= 0) ylikely {
	                grmems_rec	*ep = nullptr ;
	                time_t		dt = getustime ;
	                int		ct{} ;
	                op->s.total += 1 ;
	                if ((rs = grmems_fetch(op,&ep,gnp,gnl)) >= 0) ylikely {
	                    ri = rs ;
	                    ct = ct_hit ;
	                    rs = grmems_recaccess(op,dt,ep) ;
	                } else if (rs == SR_NOTFOUND) {
	                    ct = ct_miss ;
	                    rs = grmems_mkrec(op,dt,&ep,gnp,gnl) ;
	                    ri = rs ;
	                } /* end if (hit or miss) */
	                grmems_upstats(op,ct,rs) ;
	                if (rs >= 0) ylikely {
	                    curp->ri = ri ;
	                    curp->i = -1 ;
	                }
	            } /* end if (ok) */
	        } /* end if (valid) */
	    } /* end if (magic) */
	} /* end if (magic) */
	return (rs >= 0) ? ri : rs ;
}
/* end subroutine (grmems_lookup) */

int grmems_lookread(grmems *op,grmems_cur *curp,char *rbuf,int rlen) noex {
	int		rs ;
	if ((rs = grmems_magic(op,curp,rbuf)) >= 0) ylikely {
	    rs = SR_NOTOPEN ;
	    if (curp->magic == GRMEMS_CURMAGIC) ylikely {
		rs = SR_OK ;
	        if (op->recs == nullptr) {
	            rs = grmems_starter(op) ;
	        }
	        if (rs >= 0) ylikely {
	            cint	ri = curp->ri ;
	            if (ri >= 0) {
	                recarr		*rlp = (recarr *) op->recs ;
	                grmems_rec	*ep ;
	                int		i = (curp->i >= 0) ? (curp->i+1) : 0 ;
	                if ((rs = recarr_get(rlp,ri,&ep)) >= 0) {
	                    if (i < ep->nmems) {
				cchar	*mem = ep->mems[i] ;
	                        if ((rs = sncpy1(rbuf,rlen,mem)) >= 0) {
	                            curp->i = i ;
	                        }
	                    } else {
	                        rs = SR_NOTFOUND ;
		            }
	                } /* end if */
	            } else {
	                rs = SR_NOTFOUND ;
	            }
	        } /* end if (ok) */
	    } /* end if (magic) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (grmems_lookread) */

int grmems_invalidate(grmems *op,cchar *gnp,int gnl) noex {
	int		rs ;
	int		rs1 ;
	int		f_found = false ;
	if ((rs = grmems_magic(op,gnp)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (gnp[0]) ylikely {
		rs = SR_OK ;
	        if (op->recs == nullptr) {
	            rs = grmems_starter(op) ;
	        }
	        if (rs >= 0) ylikely {
	            grmems_rec	*ep{} ;
	            if ((rs = grmems_fetch(op,&ep,gnp,gnl)) >= 0) {
	                cint	ri = rs ;
	                pq_ent	*pep = (pq_ent *) ep ;
	                f_found = true ;
		        {
	                    rs1 = pq_unlink(op->lrup,pep) ;
	                    if (rs >= 0) rs = rs1 ;
		        }
		        {
	                    rs1 = recarr_del(op->recs,ri) ;
	                    if (rs >= 0) rs = rs1 ;
		        }
		        {
	                    rs1 = record_finish(ep) ;
	                    if (rs >= 0) rs = rs1 ;
		        }
		        {
	                    rs1 = uc_free(ep) ;
	                    if (rs >= 0) rs = rs1 ;
		        }
	            } else if (rs == SR_NOTFOUND) {
	                rs = SR_OK ;
	            }
	        } /* end if (ok) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? f_found : rs ;
}
/* end subroutine (grmems_invalidate) */

int grmems_check(grmems *op,time_t dt) noex {
	int		rs ;
	int		f = false ;
	if ((rs = grmems_magic(op)) >= 0) ylikely {
	    if (op->recs != nullptr) {
	        recarr		*rlp = op->recs ;
	        grmems_rec	*ep{} ;
	        for (int i = 0 ; recarr_get(rlp,i,&ep) >= 0 ; i += 1) {
	            if (ep) {
	                if (dt == 0) dt = getustime ;
	                if ((rs = record_isold(ep,dt,op->ttl)) > 0) {
	                    f = true ;
	                    if ((rs = grmems_recdel(op,ep)) >= 0) {
	                        pq_ent	*pep = (pq_ent *) ep ;
	                        rs = pq_unlink(op->lrup,pep) ;
	                        uc_free(ep) ;
	                    }
	                } /* end if (entry-old) */
		    } /* end if (non-null) */
	            if (rs < 0) break ;
	        } /* end for */
	    } /* end if (non-null) */
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (grmems_check) */

int grmems_stats(grmems *op,grmems_st *sp) noex {
	int		rs ;
	if ((rs = grmems_magic(op,sp)) >= 0) ylikely {
	    if (op->recs) {
	        if ((rs = recarr_count(op->recs)) >= 0) {
	            *sp = op->s ;
	            sp->nentries = rs ;
	        }
	    } else {
	        memclear(sp) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (grmems_stats) */


/* private subroutines */

static int grmems_starter(grmems *op) noex {
	int		rs = SR_OK ;
	if (op->recs == nullptr) ylikely {
	    cint	sz = sizeof(recarr) ;
	    if (void *vp ; (rs = uc_malloc(sz,&vp)) >= 0) ylikely {
	        int	ro = 0 ;
	        ro |= RECARR_OSTATIONARY ;
	        ro |= RECARR_OREUSE ;
	        ro |= RECARR_OCONSERVE ;
	        op->recs = (recarr *) vp ;
	        if ((rs = recarr_start(op->recs,op->nmax,ro)) >= 0) {
		    op->ti_check = getustime ;
	        } /* end if (recarr_start) */
	        if (rs < 0) {
	            uc_free(op->recs) ;
	            op->recs = nullptr ;
	        }
	    } /* end if (memory-allocation) */
	} /* end if (needed initialization) */
	return rs ;
}
/* end subroutine (grmems_starter) */

static int grmems_fetch(grmems *op,grmems_rec **epp,cchar *gnp,int gnl) noex {
	recarr		*rlp = op->recs ;
	grmems_rec	*ep{} ;
	int		rs ;
	int		i = 0 ; /* used-afterwards */
	for (i = 0 ; (rs = recarr_get(rlp,i,&ep)) >= 0 ; i += 1) {
	    if (ep) {
	        if (gnp[0] == ep->gn[0]) {
	            if (strwcmp(ep->gn,gnp,gnl) == 0) break ;
		}
	    }
	} /* end for */
	if (epp) {
	    *epp = (rs >= 0) ? ep : nullptr ;
	}
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (grmems_fetch) */

static int grmems_mkrec(grmems *op,time_t dt,grmems_rec **epp,
		cchar *gnp,int gnl) noex {
	int		rs ;
	int		rs1 ;
	int		ri = 0 ;
	*epp = nullptr ;
	if ((rs = recarr_count(op->recs)) >= 0) ylikely {
	    pq_ent	*pep ;
	    cint	n = rs ;
	    if (n >= op->nmax) {
	        if ((rs = pq_rem(op->lrup,&pep)) >= 0) {
	            grmems_rec	*ep = (grmems_rec *) pep ;
	            if ((rs = grmems_recdel(op,ep)) >= 0) {
	                rs = grmems_recstart(op,dt,ep,gnp,gnl) ;
	                ri = rs ;
	            }
	            rs1 = pq_ins(op->lrup,pep) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (removed entry) */
	        if (rs >= 0) {
		    *epp = (grmems_rec *) pep ;
		}
	    } else {
	        if ((rs = grmems_newrec(op,dt,epp,gnp,gnl)) >= 0) {
	            ri = rs ;
	            if (*epp) {
	                pep = (pq_ent *) *epp ;
	                rs = pq_ins(op->lrup,pep) ;
	            }
	        } /* end if (new-entry) */
	    } /* end if (at max entries or not) */
	} /* end if (recarr_count) */
	return (rs >= 0) ? ri : rs ;
}
/* end subroutine (grmems_mkrec) */

static int grmems_newrec(grmems *op,time_t dt,grmems_rec **epp,
		cchar *gnp,int gnl) noex {
	int		rs = SR_BUGCHECK ;
	if (epp) ylikely {
	    grmems_rec	*ep{} ;
	    cint	rsize = sizeof(grmems_rec) ;
	    if ((rs = uc_malloc(rsize,&ep)) >= 0) {
	        rs = grmems_recstart(op,dt,ep,gnp,gnl) ;
	        if (rs < 0) {
		    uc_free(ep) ;
		}
	    } /* end if (memory-allocation) */
	    *epp = (rs >= 0) ? ep : nullptr ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (grmems_newrec) */

static int grmems_recstart(grmems *op,time_t dt,grmems_rec *ep,
		cchar *gnp,int gnl) noex {
	cint		gnlen = var.groupnamelen ;
	int		rs ;
	int		rs1 ;
	int		ri = 0 ;
	{
	    cint	wc = op->wcount++ ;
	    char	gn[gnlen+1] ;
	    strdcpy1w(gn,gnlen,gnp,gnl) ;
	    if (char *grbuf ; (rs = malloc_gr(&grbuf)) >= 0) ylikely {
	        ucentgr	gr{} ;
	        cint	grlen = rs ;
	        if ((rs = getgr_name(&gr,grbuf,grlen,gn)) >= 0) {
	            const gid_t	gid = gr.gr_gid ;
	            cint	esz = szof(grmems_u) ;
		    cint	vn = 10 ;
	            if (vecobj u ; (rs = u.start(esz,vn,0)) >= 0) {
	                if ((rs = grmems_recusers(op,dt,&u,gid)) >= 0) {
	                    if ((rs = record_start(ep,dt,wc,&u,&gr)) >= 0) {
	                        if ((rs = recarr_add(op->recs,ep)) >= 0) {
	                            ri = rs ;
	                            ep->recidx = rs ;
	                        }
	                        if (rs < 0) {
	                            record_finish(ep) ;
			        }
	                    } /* end if (entry-start) */
	                } /* end if (grmems-recusers) */
	                rs1 = u.finish ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (vecobj-user) */
	        } /* end if (getgr-name) */
	        rs1 = uc_free(grbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end block */
	return (rs >= 0) ? ri : rs ;
}
/* end subroutine (grmems_recstart) */

static int grmems_recrefresh(grmems *op,time_t dt,grmems_rec *ep) noex {
	cint		wc = op->wcount++ ;
	int		rs ;
	int		rs1 ;
	if (cchar *gnp ; (rs = record_getgnp(ep,&gnp)) >= 0) ylikely {
	    if (char *grbuf ; (rs = malloc_gr(&grbuf)) >= 0) ylikely {
	        ucentgr		gr ;
		cint		grlen = rs ;
	        if ((rs = getgr_name(&gr,grbuf,grlen,gnp)) >= 0) {
	            const gid_t	gid = gr.gr_gid ;
	            cint	esz = szof(grmems_u) ;
		    cint	vn = 10 ;
		    cint	vo = 0 ;
	            if (vecobj u ; (rs = u.start(esz,vn,vo)) >= 0) {
	                if ((rs = grmems_recusers(op,dt,&u,gid)) >= 0) {
	                    if ((rs = record_refresh(ep,dt,wc,&u,&gr)) >= 0) {
	        		op->s.refreshes += 1 ;
			    }
	                } /* end if (grmems-recusers) */
	                rs1 = u.finish ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (vecobj-user) */
	        } /* end if (getgr-name) */
	        rs1 = uc_free(grbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (malloc_gr) */
	} /* end if (record_getgnp) */
	return (rs >= 0) ? wc : rs ;
}
/* end subroutine (grmems_recrefresh) */

/* get all users w/ this specified GID */
static int grmems_recusers(grmems *op,time_t dt,vecobj *ulp,gid_t gid) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = grmems_mkug(op,dt)) >= 0) ylikely {
	    grmems_ug	k{} ;
	    grmems_ug	*ugp ;
	    grmems_ug	*ugs = (grmems_ug *) op->usergids ;
	    csize	esize = sizeof(grmems_ug) ;
	    csize	nsize = size_t(op->nusergids) ;
	    stdsort_f	scf = stdsort_f(vcmpug) ;
	    k.gid = gid ;
	    ugp = (grmems_ug *) bsearch(&k,ugs,nsize,esize,scf) ;
	    if (ugp != nullptr) {
		cint	vn = int(nsize) ;
		cint	ulen = var.usernamelen ;
	        while (ugp > ugs) {
	            if (ugp[-1].gid != gid) break ;
	            ugp -= 1 ;
	        }
	        while ((ugp < (ugs + vn)) && (ugp->gid == gid)) {
	            grmems_u	u{} ;
	            u.up = ugp->un ;
	            u.ul = lenstr(ugp->un,ulen) ;
	            rs = ulp->add(&u) ;
	            c += 1 ;
	            ugp += 1 ;
	            if (rs < 0) break ;
	        } /* end while */
	    } /* end if (had some hits) */
	} /* end if (grmems-mkug) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (grmems_recusers) */

static int grmems_mkug(grmems *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	if (op->usergids == nullptr) {
	    cint	vn = 10 ;
	    cint	vo = 0 ;
	    cint	esz = szof(grmems_ug) ;
	    if (vecobj ugl ; (rs = ugl.start(esz,vn,vo)) >= 0) ylikely {
	        if ((rs = grmems_mkugload(op,dt,&ugl)) >= 0) ylikely {
		    vecobj_vcf	vcf = vecobj_vcf(vcmpug) ;
	            c = rs ;
	            if ((rs = ugl.sort(vcf)) >= 0) ylikely {
	                rs = grmems_mkugstore(op,dt,&ugl) ;
		    }
	        } /* end if (grmems-mkugload) */
	        rs1 = ugl.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (vecobj) */
	} /* end if (usergids) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (grmems_mkug) */

static int grmems_mkugload(grmems *op,time_t dt,vecobj *ulp) noex {
	cnullptr	np{} ;
	sysdbfiles	w = sysdbfile_pw ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (cchar *fn{} ; (rs = sysdbfn_get(w,np,&fn)) >= 0) ylikely {
	    if ((rs = grmems_pwmapbegin(op,dt,fn)) >= 0) ylikely {
	        int	ml = op->fsize ;
	        cchar	*mp = charp(op->mapdata) ;
	        cchar	*tp ;
	        op->ti_access = dt ;
	        while ((tp = strnchr(mp,ml,CH_NL)) != np) {
	            gid_t	gid{} ;
	            cint	len = intconv(tp - mp) ;
	            if ((rs = pwentparse(mp,len,&gid)) > 0) {
	                grmems_ug	ug ;
		        cint		ul = rs ;
	                if ((rs = usergid_start(&ug,mp,ul,gid)) >= 0) {
			    if (rs > 0) {
	                        c += 1 ;
	                        rs = ulp->add(&ug) ;
			    } else {
				usergid_finish(&ug) ;
			    }
			    if (rs < 0) {
				usergid_finish(&ug) ;
			    }
	                } /* end if (usergid_start) */
	            } /* end if (pwentparse) */
	            ml -= intconv((tp + 1) - mp) ;
	            mp = (tp+1) ;
	            if (rs < 0) break ;
	        } /* end while (reading lines) */
	        rs1 = grmems_pwmapend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (grmems-pwmap) */
	} /* end if (sysdbfn_get) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (grmems_mkugload) */

static int grmems_mkugstore(grmems *op,time_t dt,vecobj *ulp) noex {
	cint		esz = sizeof(grmems_ug) ;
	int		rs ;
	int		c = 0 ;
	if ((rs = vecobj_count(ulp)) >= 0) ylikely {
	    cint	sz = ((rs + 1) * esz) ;
	    if (void *vp ; (rs = uc_malloc(sz,&vp)) >= 0) ylikely {
	        grmems_ug	*ugs = (grmems_ug *) vp ;
	        for (int i = 0 ; ulp->get(i,&vp) >= 0 ; i += 1) {
	            if (vp) {
	        	grmems_ug	*ugp = (grmems_ug *) vp ;
	                ugs[c++] = *ugp ;
		    }
	        } /* end for */
	        ugs[c] = {} ;
	        op->usergids = ugs ;
	        op->nusergids = c ;
	        op->ti_usergids = dt ;
	    } /* end if (memory-allocation) */
	} /* end if (vecobj-count) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (grmems_mkugstore) */

static int grmems_mkugdeplete(grmems *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->usergids) {
	    cint	n = op->nusergids ;
	    grmems_ug	*ugs = (grmems_ug *) op->usergids ;
	    for (int i = 0 ; i < n ; i += 1) {
		rs1 = usergid_finish((ugs + i)) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end for */
	} /* end if (needed) */
	return rs ;
}
/* end subroutine (grmems_mkugdeplete) */

static int grmems_recaccess(grmems *op,time_t dt,grmems_rec *ep) noex {
	int		rs ;
	if ((rs = grmems_recrear(op,ep)) >= 0) ylikely {
	    if ((rs = record_isold(ep,dt,op->ttl)) > 0) {
	        rs = grmems_recrefresh(op,dt,ep) ;
	    } else {
	        rs = record_access(ep,dt) ;
	    }
	} /* end if */
	return rs ;
}
/* end subroutine (grmems_recaccess) */

static int grmems_recrear(grmems *op,grmems_rec *ep) noex {
	int		rs ;
	if (pq_ent *pep ; (rs = pq_gettail(op->lrup,&pep)) >= 0) {
	    pq_ent	*pcp = (pq_ent *) ep ;
	    if (pcp != pep) {
	        pep = (pq_ent *) ep ;
	        if ((rs = pq_unlink(op->lrup,pep)) >= 0) {
	            rs = pq_ins(op->lrup,pep) ;
	            if (rs < 0) {
	                grmems_rec	*gep = (grmems_rec *) pep ;
	                record_finish(gep) ;
	                uc_free(gep) ;
	            } /* end if (error) */
	        } /* end if (pq_unlink) */
	    }
	} /* end if (pq_gettail) */
	return rs ;
}
/* end subroutine (grmems_recrear) */

static int grmems_recdel(grmems *op,grmems_rec *ep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if ((rs1 = recarr_ent(op->recs,ep)) >= 0) {
	    rs1 = recarr_del(op->recs,rs1) ;
	}
	if (rs >= 0) rs = rs1 ;
	{
	    rs1 = record_finish(ep) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (grmems_recdel) */

static int grmems_recfins(grmems *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->recs != nullptr) {
	    recarr	*rlp = op->recs ;
	    grmems_rec	*ep{} ;
	    for (int i = 0 ; recarr_get(rlp,i,&ep) >= 0 ; i += 1) {
	        if (ep) {
		    {
	                rs1 = record_finish(ep) ;
	                if (rs >= 0) rs = rs1 ;
		    }
		    {
	                rs1 = uc_free(ep) ;
	                if (rs >= 0) rs = rs1 ;
		    }
		}
	    } /* end for */
	} /* end if */
	return rs ;
}
/* end subroutine (grmems_recfins) */

static int grmems_upstats(grmems *op,int ct,int rs) noex {
	int		f_got = (rs > 0) ;
	switch (ct) {
	case ct_hit:
	    if (f_got) op->s.phits += 1 ;
	    else op->s.nhits += 1 ;
	    break ;
	case ct_miss:
	    if (f_got) op->s.pmisses += 1 ;
	    else op->s.nmisses += 1 ;
	    break ;
	} /* end switch */
	return SR_OK ;
}
/* end subroutine (grmems_upstats) */

static int grmems_pwmapbegin(grmems *op,time_t dt,cchar *fn) noex {
	cnullptr	np{} ;
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->mapdata == nullptr) {
	    cint	of = O_RDONLY ;
	    cmode	om = 0666 ;
	    if ((rs = uc_open(fn,of,om)) >= 0) {
	        op->fd = rs ;
	        op->ti_open = dt ;
	        if ((rs = uc_fsize(op->fd)) >= 0) {
	            csize	ms = size_t(max(rs,op->pagesize)) ;
	            int		mp = PROT_READ ;
	            int		mf = MAP_SHARED ;
	            int		fd = op->fd ;
	            void	*md{} ;
	            op->fsize = rs ;
	            if ((rs = u_mmapbegin(np,ms,mp,mf,fd,0L,&md)) >= 0) {
	                cint		madv = MADV_SEQUENTIAL ;
			const caddr_t	ma = caddr_t(md) ;
	                if ((rs = u_madvise(ma,ms,madv)) >= 0) {
	                    op->mapdata = md ;
	                    op->mapsize = ms ;
	                } /* end if (advise) */
	                if (rs < 0) {
	                    u_mmapend(md,ms) ;
	                }
	            } /* end if (mmap) */
	        } /* end if (file-size) */
	        rs1 = u_close(op->fd) ;
		if (rs >= 0) rs = rs1 ;
	        op->fd = -1 ;
	    } /* end if (file-open) */
	} /* end if (need mapping) */
	return rs ;
}
/* end subroutine (grmems_pwmapbegin) */

static int grmems_pwmapend(grmems *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->mapdata != nullptr) {
	    csize	ms = op->mapsize ;
	    void	*md = op->mapdata ;
	    rs1 = u_mmapend(md,ms) ;
	    if (rs >= 0) rs = rs1 ;
	    op->mapdata = nullptr ;
	    op->mapsize = 0 ;
	    op->ti_open = 0 ;
	}
	return rs ;
}
/* end subroutine (grmems_pwmapend) */

static int record_start(grmems_rec *ep,time_t dt,int wc,
		vecobj *ulp,ucentgr *grp) noex {
	int		rs = SR_FAULT ;
	int		n = 0 ;
	if (ep && ulp && grp) ylikely {
	    memclear(ep) ;		/* dangerous */
	    if (char *gnbuf ; (rs = malloc_gn(&gnbuf)) >= 0) ylikely {
		cint	gnlen = rs ;
		ep->gn = gnbuf ;
		{
	            strwcpy(ep->gn,grp->gr_name,gnlen) ;
	            rs = record_mems(ep,dt,wc,ulp,grp) ;
	            n = rs ;
		}
		if (rs < 0) {
		    ep->gn[0] = '\0' ;
		    uc_free(ep->gn) ;
		    ep->gn = nullptr ;
		}
	    } /* end if (memory-allocation) */
	} /* end if (magic) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (record_start) */

static int record_finish(grmems_rec *ep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (ep) ylikely {
	    rs = SR_OK ;
	    if (ep->mems) {
	        rs1 = uc_free(ep->mems) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->mems = nullptr ;
	    }
	    if (ep->gn) {
	        ep->gn[0] = '\0' ;
		rs1 = uc_free(ep->gn) ;
	        if (rs >= 0) rs = rs1 ;
		ep->gn = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (record_finish) */

static int record_refresh(grmems_rec *ep,time_t dt,int wc,
		vecobj *ulp,ucentgr *grp) noex {
	int		rs = SR_FAULT ;
	int		n = 0 ;
	if (ep) ylikely {
	    rs = SR_OK ;
	    if (ep->mems != nullptr) {
	        rs = uc_free(ep->mems) ;
	        ep->mems = nullptr ;
	        ep->nmems = 0 ;
	    }
	    if (rs >= 0) {
	        rs = record_mems(ep,dt,wc,ulp,grp) ;
	        n = rs ;
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (record_refresh) */

static int record_mems(grmems_rec *ep,time_t dt,int wc,
		vecobj *ulp,ucentgr *grp) noex {
	int		rs ;
	int		n = 0 ; /* return-value */
	if ((rs = record_loadgruns(ep,ulp,grp)) >= 0) ylikely {
	    if ((rs = ulp->count) >= 0) ylikely {
		cint	ulen = var.usernamelen ;
	        int	sz = 0 ;
	        int	masz ;
		int	ul ;
		void	*vp{} ;
	        n = rs ;
	        masz = ((n + 1) * sizeof(cchar *)) ;
	        sz += masz ;
	        for (int i = 0 ; ulp->get(i,&vp) >= 0 ; i += 1) {
	            if (vp) {
	        	grmems_u	*up = (grmems_u *) vp ;
		        ul = up->ul ;
		        if (ul < 0) {
			    ul = lenstr(up->up,ulen) ;
			    up->ul = ul ;
		        }
	                sz += (ul+1) ;
		    } /* end if (non-null) */
	        } /* end for */
	        if (char *bp ; (rs = uc_malloc(sz,&bp)) >= 0) ylikely {
	            cchar	**mems = ccharpp(bp) ;
	            int		c = 0 ;
	            bp += masz ;
	            for (int i = 0 ; vecobj_get(ulp,i,&vp) >= 0 ; i += 1) {
	                if (vp) {
	        	    grmems_u	*up = (grmems_u *) vp ;
	                    mems[c++] = bp ;
	                    bp = (strwcpy(bp,up->up,up->ul) + 1) ;
			}
	            } /* end for */
	            mems[c] = nullptr ;
	            ep->ti_create = dt ;
	            ep->ti_access = dt ;
	            ep->wcount = wc ;
	            ep->mems = mems ;
	            ep->nmems = c ;
	        } /* end if (memory-allocation) */
	    } /* end if (vecobj-count) */
	} /* end if (load user members) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (record_mems) */

static int record_loadgruns(grmems_rec *op,vecobj *ulp,ucentgr *grp) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (grp->gr_mem != nullptr) {
	        cchar	**mems = ccharpp(grp->gr_mem) ;
	        for (int i = 0 ; mems[i] != nullptr ; i += 1) {
	            grmems_u	u ;
	            u.up = mems[i] ;
	            u.ul = -1 ;
	            rs = vecobj_addouruniq(ulp,&u) ;
	            if (rs < INT_MAX) c += 1 ;
	            if (rs < 0) break ;
	        } /* end for */
	    } /* end if (had members) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (record_loadgruns) */

static int record_access(grmems_rec *ep,time_t dt) noex {
	int		rs = SR_FAULT ;
	int		wc = 0 ;
	if (ep) ylikely {
	    rs = SR_OK ;
	    ep->ti_access = dt ;
	    wc = ep->wcount ;
	} /* end if (non-null) */
	return (rs >= 0) ? wc : rs ;
}
/* end subroutine (record_access) */

static int record_isold(grmems_rec *ep,time_t dt,int ttl) noex {
	int		rs = SR_FAULT ;
	int		f_old = false ;
	if (ep) ylikely {
	    rs = SR_OK ;
	    f_old = ((dt - ep->ti_create) >= ttl) ;
	} /* end if (non-null) */
	return (rs >= 0) ? f_old : rs ;
}
/* end subroutine (record_isold) */

static int record_getgnp(grmems_rec *ep,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	if (ep && rpp) ylikely {
	    rs = SR_BUGCHECK ;
	    if (ep->gn) ylikely {
	        rs = SR_NOTOPEN ;
	        if (ep->gn[0]) ylikely {
		    rs = SR_OK ;
	            *rpp = (rs >= 0) ? ep->gn : nullptr ;
	        } /* end if (valid) */
	    } /* end if (non-null) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (record_getgnp) */

static int usergid_start(grmems_ug *ugp,cchar *unp,int unl,gid_t gid) noex {
	int		rs ;
	int		ul = 0 ;
	if (char *unbuf ; (rs = malloc_un(&unbuf)) >= 0) ylikely {
	    cint	unlen = rs ;
	    ugp->un = unbuf ;
	    ul = intconv(strnwcpy(ugp->un,unlen,unp,unl) - ugp->un) ;
	    ugp->gid = gid ;
	} /* end if (memory-allocation) */
	return (rs >= 0) ? ul : rs ;
}
/* end subroutine (usergid_start) */

static int usergid_finish(grmems_ug *ugp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (ugp) ylikely {
	    rs = SR_OK ;
	    if (ugp->un) {
		ugp->un[0] = '\0' ;
		rs1 = uc_free(ugp->un) ;
		if (rs >= 0) rs = rs1 ;
		ugp->un = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (usergid_finish) */

/* PASSWD entry parsing */
static int pwentparse(cchar *lbuf,int llen,gid_t *gp) noex {
	int		rs = SR_OK ;
	int		ll = llen ;
	int		ul = 0 ;
	cchar		*lp = lbuf ;
	bool		fgid = false ;
	for (int fi = 0 ; fi < 4 ; fi += 1) {
	    if (cchar *tp ; (tp = strnchr(lp,ll,':')) != nullptr) {
	        switch (fi) {
	        case 0:
	            ul = intconv(tp - lp) ;
	            break ;
	        case 3:
	            {
			cint	tl = intconv(tp - lp) ;
	                if (int v ; (rs = cfdeci(lp,tl,&v)) >= 0) {
			    fgid = true ;
	                    *gp = gid_t(v) ;
	                } else if (isNotValid(rs)) {
			    rs = SR_OK ;
	                    ul = 0 ;
			}
	            } /* end block */
	            break ;
	        } /* end switch */
	        ll -= intconv((tp + 1) - lp) ;
	        lp = (tp+1) ;
	    } /* end if (separator) */
	} /* end for (looping through fields) */
	if (! fgid) ul = 0 ;
	return (rs >= 0) ? ul : rs ;
}
/* end subroutine (pwentparse) */

static int vecobj_addouruniq(vecobj *ulp,grmems_u *unp) noex {
	cint		ulen = var.usernamelen ;
	int		rs ;
	bool		f = false ;
	void		*vp{} ;
	for (int i = 0 ; (rs = ulp->get(i,&vp)) >= 0 ; i += 1) {
	    if (vp) {
	        grmems_u	*up = (grmems_u *) vp ;
	        f = (up->up[0] == unp->up[0]) ;
	        f = f && (strncmp(up->up,unp->up,ulen) == 0) ;
	        if (f) break ;
	    }
	} /* end for */
	if (rs == SR_NOTFOUND) {
	    rs = ulp->add(unp) ;
	} else {
	    rs = INT_MAX ;
	}
	return rs ;
}
/* end subroutine (vecobj_addouruniq) */

static int cmpug(cvoid *v1p,cvoid *v2p) noex {
	grmems_ug	*g1p = (grmems_ug *) v1p ;
	grmems_ug	*g2p = (grmems_ug *) v2p ;
	return (g1p->gid - g2p->gid) ;
}
/* end subroutine (cmpug) */

static int vcmpug(cvoid **v1pp,cvoid **v2pp) noex {
	grmems_ug	**g1pp = (grmems_ug **) v1pp ;
	grmems_ug	**g2pp = (grmems_ug **) v2pp ;
	int		rc = 0 ;
	{
	    grmems_ug	*g1p = *g1pp ;
	    grmems_ug	*g2p = *g2pp ;
	    if (g1p || g2p) {
		rc = +1 ;
		if (g1p) {
		    rc = -1 ;
		    if (g2p) {
			rc = cmpug(g1p,g2p) ;
		    }
		}
	    }
	} /* end block */
	return rc ;
}
/* end subroutine (vcmpug) */

vars::operator int () noex {
	int		rs ;
	if ((rs = ucpagesize) >= 0) ylikely {
	    pagesize = rs ;
	    if ((rs = getbufsize(getbufsize_gr)) >= 0) ylikely {
	        grlen = rs ;
	        if ((rs = getbufsize(getbufsize_un)) >= 0) ylikely {
	            usernamelen = rs ;
	            if ((rs = getbufsize(getbufsize_gn)) >= 0) ylikely {
	                groupnamelen = rs ;
	            }
	        }
	    }
	} /* end if (ucpagesize) */
	return rs ;
}
/* end method (vars::operator) */


