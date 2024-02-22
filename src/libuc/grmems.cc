/* grmems SUPPORT */
/* lang=C++20 */

/* UNIX® group membership access and cache */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-01-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Description:
	This object provides a crude access manager and cache for
	UNIX® group membership lists.

	Implementation note:

	1. This was a rough (and somewhat crude) first attempt at
	this function.  We are fairly efficient at every step but
	there may be one or two places where a sorted map might
	have been useful. But (so called) "n" is always relatively
	small so using structure sorted maps everywhere might have
	actually slowed things down also.

	2. The use of mapping the stored local 'passwd' data file
	can be changed to simple file reading if the need ever
	arises. We did not use the FILEMAP object (because at first
	I thought it too simple for the requirement), but this could
	be hacked in later (as indeed it turns out that object is
	appropriate) if you want to make the code look cleaner.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/mman.h>
#include	<grp.h>
#include	<climits>		/* |INT_MAX| */
#include	<cstdlib>
#include	<ctime>
#include	<cstring>
#include	<new>			/* |nothrow(3c++)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<usupport.h>		/* |memclear(3u)| */
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<getax.h>
#include	<recarr.h>
#include	<vecelem.h>
#include	<ascii.h>
#include	<strn.h>
#include	<sncpyx.h>
#include	<strwcpy.h>
#include	<strdcpy.h>
#include	<cfdec.h>
#include	<strwcmp.h>
#include	<sysdbfname.h>		/* |sysdbfnameget(3uc)| */
#include	<isnot.h>
#include	<localmisc.h>

#include	"grmems.h"


/* local defines */

#define	GRMEMS_DEFMAX		20	/* default maximum entries */
#define	GRMEMS_DEFTTL		(10*60)	/* default time-to-live */

#define	GRMEMS_REC		struct grmems_r
#define	GRMEMS_USER		struct grmems_u
#define	GRMEMS_USERGID		struct grmems_ug


/* local namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef GRMEMS_REC		grmems_rec ;

#ifndef	STDSORTF_TYPEDEF
#define	STDSORTF_TYPEDEF
typedef int	(*stdsort_f)(cvoid *,cvoid *) noex ;
#endif


/* external subroutines */


/* external variables */


/* local structures */

struct vars {
	int		usernamelen ;
	int		groupnamelen ;
} ;

enum cts {
	ct_miss,
	ct_hit,
	ct_overlast
} ;

struct grmems_r : pq_ent {
	cchar		**mems ;
	time_t		ti_create ;
	time_t		ti_access ;
	uint		wcount ;
	int		nmems ;
	int		recidx ;
	char		gn[GROUPNAMELEN+1] ;
} ;

struct grmems_u {
	cchar		*up ;
	int		ul ;
} ;

struct grmems_ug {
	gid_t		gid ;
	char		un[USERNAMELEN] ;
} ;


/* forward references */

template<typename ... Args>
static int grmems_ctor(grmems *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    const nullptr_t	np{} ;
	    memclear(op) ;		/* dangerous */
	    rs = SR_NOMEM ;
	    if ((op->lrup = new(nothrow) pq) != np) {
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
	    if (op->lrup) {
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
	if (op && (args && ...)) {
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
static int	grmems_mkugload(grmems *,time_t,vecelem *) noex ;
static int	grmems_mkugstore(grmems *,time_t,vecelem *) noex ;

static int	grmems_recusers(grmems *,time_t,vecelem *,gid_t) noex ;

static int	grmems_pwmapbegin(grmems *,time_t,cchar *) noex ;
static int	grmems_pwmapend(grmems *) noex ;

static int record_start(grmems_rec *,time_t,int,vecelem *,GROUP *) noex ;
static int record_loadgruns(grmems_rec *,vecelem *,GROUP *) noex ;
static int record_access(grmems_rec *,time_t) noex ;
static int record_refresh(grmems_rec *,time_t,int,vecelem *,GROUP *) noex ;
static int record_mems(grmems_rec *,time_t,int,vecelem *,GROUP *) noex ;
static int record_isold(grmems_rec *,time_t,int) noex ;
static int record_getgnp(grmems_rec *,cchar **) noex ;
static int record_finish(grmems_rec *) noex ;

static int	usergid_load(GRMEMS_USERGID *,cchar *,int,gid_t) noex ;

static int	pwentparse(cchar *,int,gid_t *) noex ;

static int	vecelem_addouruniq(vecelem *,GRMEMS_USER *) noex ;

static int	ugcmp(cvoid *,cvoid *) noex ;

static int	mkvars() noex ;


/* local variables */

static vars		var ;


/* exported variables */


/* exported subroutines */

int grmems_start(grmems *op,int nmax,int ttl) noex {
	static int	rsv = mkvars() ;
	int		rs ;
	if (nmax < GRMEMS_DEFMAX) nmax = GRMEMS_DEFMAX ;
	if (ttl < GRMEMS_DEFTTL) ttl = GRMEMS_DEFTTL ;
	if ((rs = rsv) >= 0) {
	    if ((rs = grmems_ctor(op)) >= 0) {
	        op->fd = -1 ;
	        op->pagesize = getpagesize() ;
	        if ((rs = pq_start(op->lrup)) >= 0) {
	            op->nmax = nmax ;
	            op->ttl = ttl ;
	            op->magic = GRMEMS_MAGIC ;
	        }
	        if (rs < 0) {
		    grmems_dtor(op) ;
	        }
	    } /* end if (grmems_ctor) */
	} /* end if (mkvars) */
	return rs ;
}
/* end subroutine (grmems_start) */

int grmems_finish(grmems *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = grmems_magic(op)) >= 0) {
	    if (op->usergids) {
	        rs1 = uc_free(op->usergids) ;
	        if (rs >= 0) rs = rs1 ;
	        op->usergids = nullptr ;
	    }
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
	if ((rs = grmems_magic(op,curp)) >= 0) {
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
	if ((rs = grmems_magic(op,curp)) >= 0) {
	    rs = SR_NOTOPEN ;
	    if (curp->magic == GRMEMS_CURMAGIC) {
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
	if ((rs = grmems_magic(op,curp,gnp)) >= 0) {
	    rs = SR_NOTOPEN ;
	    if (curp->magic == GRMEMS_CURMAGIC) {
		rs = SR_INVALID ;
	        if (gnp[0]) {
		    rs = SR_OK ;
	            if (op->recs == nullptr) {
	                rs = grmems_starter(op) ;
	            }
	            if (rs >= 0) {
	                grmems_rec	*ep = nullptr ;
	                time_t		dt = time(nullptr) ;
	                int		ct{} ;
	                op->s.total += 1 ;
	                if ((rs = grmems_fetch(op,&ep,gnp,gnl)) >= 0) {
	                    ri = rs ;
	                    ct = ct_hit ;
	                    rs = grmems_recaccess(op,dt,ep) ;
	                } else if (rs == SR_NOTFOUND) {
	                    ct = ct_miss ;
	                    rs = grmems_mkrec(op,dt,&ep,gnp,gnl) ;
	                    ri = rs ;
	                } /* end if (hit or miss) */
	                grmems_upstats(op,ct,rs) ;
	                if (rs >= 0) {
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
	if ((rs = grmems_magic(op,curp,rbuf)) >= 0) {
	    rs = SR_NOTOPEN ;
	    if (curp->magic == GRMEMS_CURMAGIC) {
		rs = SR_OK ;
	        if (op->recs == nullptr) {
	            rs = grmems_starter(op) ;
	        }
	        if (rs >= 0) {
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
	if ((rs = grmems_magic(op,gnp)) >= 0) {
	    rs = SR_INVALID ;
	    if (gnp[0]) {
		rs = SR_OK ;
	        if (op->recs == nullptr) {
	            rs = grmems_starter(op) ;
	        }
	        if (rs >= 0) {
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
	if ((rs = grmems_magic(op)) >= 0) {
	    if (op->recs != nullptr) {
	        recarr		*rlp = op->recs ;
	        grmems_rec	*ep{} ;
	        for (int i = 0 ; recarr_get(rlp,i,&ep) >= 0 ; i += 1) {
	            if (ep) {
	                if (dt == 0) dt = time(nullptr) ;
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

int grmems_stats(grmems *op,GRMEMS_STATS *sp) noex {
	int		rs ;
	if ((rs = grmems_magic(op,sp)) >= 0) {
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
	if (op->recs == nullptr) {
	    cint	size = sizeof(recarr) ;
	    void	*vp{} ;
	    if ((rs = uc_malloc(size,&vp)) >= 0) {
	        int	ro = 0 ;
	        ro |= RECARR_OSTATIONARY ;
	        ro |= RECARR_OREUSE ;
	        ro |= RECARR_OCONSERVE ;
	        op->recs = (recarr *) vp ;
	        if ((rs = recarr_start(op->recs,op->nmax,ro)) >= 0) {
		    op->ti_check = time(nullptr) ;
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
	int		i = 0 ; /* used afterwards */
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
	if ((rs = recarr_count(op->recs)) >= 0) {
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
	if (epp) {
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
	    char	*grbuf{} ;
	    strdcpy1w(gn,gnlen,gnp,gnl) ;
	    if ((rs = malloc_gr(&grbuf)) >= 0) {
	        GROUP	gr{} ;
	        cint	grlen = rs ;
	        if ((rs = getgr_name(&gr,grbuf,grlen,gn)) >= 0) {
	            vecelem	u ;
	            const gid_t	gid = gr.gr_gid ;
	            cint	esize = sizeof(GRMEMS_USER) ;
	            if ((rs = vecelem_start(&u,esize,10,0)) >= 0) {
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
	                rs1 = vecelem_finish(&u) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (vecelem-user) */
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
	cchar		*gnp{} ;
	if ((rs = record_getgnp(ep,&gnp)) >= 0) {
	    GROUP	gr ;
	    cint	grlen = getbufsize(getbufsize_gr) ;
	    char	*grbuf{} ;
	    if ((rs = uc_malloc((grlen+1),&grbuf)) >= 0) {
	        if ((rs = getgr_name(&gr,grbuf,grlen,gnp)) >= 0) {
	            vecelem	u ;
	            const gid_t	gid = gr.gr_gid ;
	            cint	esize = sizeof(GRMEMS_USER) ;
	            if ((rs = vecelem_start(&u,esize,10,0)) >= 0) {
	                if ((rs = grmems_recusers(op,dt,&u,gid)) >= 0) {
	                    rs = record_refresh(ep,dt,wc,&u,&gr) ;
	                } /* end if (grmems-recusers) */
	                rs1 = vecelem_finish(&u) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (vecelem-user) */
	        } /* end if (getgr-name) */
	        rs1 = uc_free(grbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (memory-allocation) */
	} /* end if (record_getgnp) */
	return (rs >= 0) ? wc : rs ;
}
/* end subroutine (grmems_recrefresh) */

/* get all users w/ this specified GID */
static int grmems_recusers(grmems *op,time_t dt,vecelem *ulp,gid_t gid) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = grmems_mkug(op,dt)) >= 0) {
	    GRMEMS_USERGID	k, *ugp ;
	    GRMEMS_USERGID	*ugs = (GRMEMS_USERGID *) op->usergids ;
	    cint		esize = sizeof(GRMEMS_USERGID) ;
	    cint		n = op->nusergids ;
	    stdsort_f		scf = stdsort_f(ugcmp) ;
	    k.gid = gid ;
	    ugp = (GRMEMS_USERGID *) bsearch(&k,ugs,n,esize,scf) ;
	    if (ugp != nullptr) {
		cint	ulen = var.usernamelen ;
	        while (ugp > ugs) {
	            if (ugp[-1].gid != gid) break ;
	            ugp -= 1 ;
	        }
	        while ((ugp < (ugs+n)) && (ugp->gid == gid)) {
	            GRMEMS_USER		u ;
	            u.up = ugp->un ;
	            u.ul = strnlen(ugp->un,ulen) ;
	            rs = vecelem_add(ulp,&u) ;
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
	    vecelem	ug ;
	    cint	esize = sizeof(GRMEMS_USERGID) ;
	    if ((rs = vecelem_start(&ug,esize,10,0)) >= 0) {
	        if ((rs = grmems_mkugload(op,dt,&ug)) >= 0) {
		    vecelem_vcmp	vcf = vecelem_vcmp(ugcmp) ;
	            c = rs ;
	            if ((rs = vecelem_sort(&ug,vcf)) >= 0) {
	                rs = grmems_mkugstore(op,dt,&ug) ;
		    }
	        } /* end if (grmems-mkugload) */
	        rs1 = vecelem_finish(&ug) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (vecelem) */
	} /* end if (usergids) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (grmems_mkug) */

static int grmems_mkugload(grmems *op,time_t dt,vecelem *ulp) noex {
	const nullptr_t	np{} ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (cchar *fn{} ; (rs = sysdbfnameget(sysdbfile_pw,np,&fn)) >= 0) {
	    if ((rs = grmems_pwmapbegin(op,dt,fn)) >= 0) {
	        int	ml = op->fsize ;
	        cchar	*mp = charp(op->mapdata) ;
	        cchar	*tp ;
	        op->ti_access = dt ;
	        while ((tp = strnchr(mp,ml,CH_NL)) != np) {
	            gid_t	gid{} ;
	            cint	len = (tp-mp) ;
	            if ((rs = pwentparse(mp,len,&gid)) > 0) {
	                GRMEMS_USERGID	ug ;
		        cint		ul = rs ;
	                if (usergid_load(&ug,mp,ul,gid) > 0) {
	                    c += 1 ;
	                    rs = vecelem_add(ulp,&ug) ;
	                }
	            } /* end if (pwentparse) */
	            ml -= ((tp+1)-mp) ;
	            mp = (tp+1) ;
	            if (rs < 0) break ;
	        } /* end while (reading lines) */
	        rs1 = grmems_pwmapend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (grmems-pwmap) */
	} /* end if (sysdbfname_pw) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (grmems_mkugload) */

static int grmems_mkugstore(grmems *op,time_t dt,vecelem *ulp) noex {
	int		rs ;
	int		sz ;
	int		c = 0 ;
	if ((rs = vecelem_count(ulp)) >= 0) {
	    cint	esize = sizeof(GRMEMS_USERGID) ;
	    cint	n = rs ;
	    void	*vp{} ;
	    sz = ((n+1) * esize) ;
	    if ((rs = uc_malloc(sz,&vp)) >= 0) {
	        GRMEMS_USERGID	*ugs = (GRMEMS_USERGID *) vp ;
	        GRMEMS_USERGID	*ugp ;
	        for (int i = 0 ; vecelem_get(ulp,i,&ugp) >= 0 ; i += 1) {
	            if (ugp) {
	                ugs[c++] = *ugp ;
		    }
	        } /* end for */
	        ugs[c].un[0] = '\0' ;
	        op->usergids = ugs ;
	        op->nusergids = c ;
	        op->ti_usergids = dt ;
	    } /* end if (memory-allocation) */
	} /* end if (vecelem-count) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (grmems_mkugstore) */

static int grmems_recaccess(grmems *op,time_t dt,grmems_rec *ep) noex {
	int		rs ;
	if ((rs = grmems_recrear(op,ep)) >= 0) {
	    if ((rs = record_isold(ep,dt,op->ttl)) > 0) {
	        op->s.refreshes += 1 ;
	        rs = grmems_recrefresh(op,dt,ep) ;
	    } else {
	        rs = record_access(ep,dt) ;
	    }
	} /* end if */
	return rs ;
}
/* end subroutine (grmems_recaccess) */

static int grmems_recrear(grmems *op,grmems_rec *ep) noex {
	pq_ent		*pcp = (pq_ent *) ep ;
	pq_ent		*pep ;
	int		rs ;
	if ((rs = pq_gettail(op->lrup,&pep)) >= 0) {
	    if (pcp != pep) {
	        pep = (pq_ent *) ep ;
	        if ((rs = pq_unlink(op->lrup,pep)) >= 0) {
	            rs = pq_ins(op->lrup,pep) ;
	            if (rs < 0) {
	                grmems_rec	*ep = (grmems_rec *) pep ;
	                record_finish(ep) ;
	                uc_free(pep) ;
	            }
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
	const nullptr_t	np{} ;
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->mapdata == nullptr) {
	    cint		of = O_RDONLY ;
	    cmode		om = 0666 ;
	    if ((rs = uc_open(fn,of,om)) >= 0) {
	        op->fd = rs ;
	        op->ti_open = dt ;
	        if ((rs = uc_fsize(op->fd)) >= 0) {
	            size_t	ms = MAX(rs,op->pagesize) ;
	            int		mp = PROT_READ ;
	            int		mf = MAP_SHARED ;
	            int		fd = op->fd ;
	            void	*md{} ;
	            op->fsize = rs ;
	            if ((rs = u_mmapbegin(np,ms,mp,mf,fd,0L,&md)) >= 0) {
	                cint		madv = MADV_SEQUENTIAL ;
			const caddr_t	ma = caddr_t(md) ;
	                if ((rs = uc_madvise(ma,ms,madv)) >= 0) {
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
		vecelem *ulp,GROUP *grp) noex {
	int		rs = SR_FAULT ;
	int		n = 0 ;
	if (ep && ulp && grp) {
	    cint	gnlen = var.groupnamelen ;
	    memclear(ep) ;		/* dangerous */
	    strwcpy(ep->gn,grp->gr_name,gnlen) ;
	    rs = record_mems(ep,dt,wc,ulp,grp) ;
	    n = rs ;
	} /* end if (magic) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (record_start) */

static int record_finish(grmems_rec *ep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (ep) {
	    rs = SR_OK ;
	    if (ep->mems != nullptr) {
	        rs1 = uc_free(ep->mems) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->mems = nullptr ;
	    }
	    ep->gn[0] = '\0' ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (record_finish) */

static int record_refresh(grmems_rec *ep,time_t dt,int wc,
		vecelem *ulp,GROUP *grp) noex {
	int		rs = SR_FAULT ;
	int		n = 0 ;
	if (ep) {
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
		vecelem *ulp,GROUP *grp) noex {
	int		rs ;
	int		n = 0 ;
	if ((rs = record_loadgruns(ep,ulp,grp)) >= 0) {
	    if ((rs = vecelem_count(ulp)) >= 0) {
	        GRMEMS_USER	*up ;
		cint		ulen = var.usernamelen ;
	        int		size = 0 ;
	        int		masize ;
		int		ul ;
	        char		*bp ;
	        n = rs ;
	        masize = ((n + 1) * sizeof(cchar *)) ;
	        size += masize ;
	        for (int i = 0 ; vecelem_get(ulp,i,&up) >= 0 ; i += 1) {
	            if (up != nullptr) {
		        ul = up->ul ;
		        if (ul < 0) {
			    ul = strnlen(up->up,ulen) ;
			    up->ul = ul ;
		        }
	                size += (ul+1) ;
		    }
	        } /* end for */
	        if ((rs = uc_malloc(size,&bp)) >= 0) {
	            cchar	**mems = (cchar **) bp ;
	            int		c = 0 ;
	            bp += masize ;
	            for (int i = 0 ; vecelem_get(ulp,i,&up) >= 0 ; i += 1) {
	                if (up != nullptr) {
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
	    } /* end if (vecelem-count) */
	} /* end if (load user members) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (record_mems) */

static int record_loadgruns(grmems_rec *op,vecelem *ulp,GROUP *grp) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op) {
	    rs = SR_OK ;
	    if (grp->gr_mem != nullptr) {
	        cchar	**mems = (cchar **) grp->gr_mem ;
	        for (int i = 0 ; mems[i] != nullptr ; i += 1) {
	            GRMEMS_USER		u ;
	            u.up = mems[i] ;
	            u.ul = -1 ;
	            rs = vecelem_addouruniq(ulp,&u) ;
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
	if (ep) {
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
	if (ep) {
	    rs = SR_OK ;
	    f_old = ((dt - ep->ti_create) >= ttl) ;
	} /* end if (non-null) */
	return (rs >= 0) ? f_old : rs ;
}
/* end subroutine (record_isold) */

static int record_getgnp(grmems_rec *ep,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	if (ep && rpp) {
	    rs = SR_NOTOPEN ;
	    if (ep->gn[0]) {
		rs = SR_OK ;
	        *rpp = (rs >= 0) ? ep->gn : nullptr ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (record_getgnp) */

static int usergid_load(GRMEMS_USERGID *ugp,cchar *unp,int unl,gid_t gid) noex {
	cint		unlen = var.usernamelen ;
	int		ul ;
	ul = strnwcpy(ugp->un,unlen,unp,unl) - ugp->un ;
	ugp->gid = gid ;
	return ul ;
}
/* end subroutine (usergid_load) */

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
	            ul = (tp-lp) ;
	            break ;
	        case 3:
	            {
	                if (int v ; (rs = cfdeci(lp,(tp-lp),&v)) >= 0) {
			    fgid = true ;
	                    *gp = gid_t(v) ;
	                } else if (isNotValid(rs)) {
			    rs = SR_OK ;
	                    ul = 0 ;
			}
	            } /* end block */
	            break ;
	        } /* end switch */
	        ll -= ((tp+1)-lp) ;
	        lp = (tp+1) ;
	    } /* end if (separator) */
	} /* end for (looping through fields) */
	if (! fgid) ul = 0 ;
	return (rs >= 0) ? ul : rs ;
}
/* end subroutine (pwentparse) */

static int vecelem_addouruniq(vecelem *ulp,GRMEMS_USER *unp) noex {
	GRMEMS_USER	*ep{} ;
	cint		ulen = var.usernamelen ;
	int		rs = SR_OK ;
	bool		f = false ;
	for (int i = 0 ; (rs = vecelem_get(ulp,i,&ep)) >= 0 ; i += 1) {
	    if (ep) {
	        f = (ep->up[0] == unp->up[0]) ;
	        f = f && (strncmp(ep->up,unp->up,ulen) == 0) ;
	        if (f) break ;
	    }
	} /* end for */
	if (rs == SR_NOTFOUND) {
	    rs = vecelem_add(ulp,unp) ;
	} else {
	    rs = INT_MAX ;
	}
	return rs ;
}
/* end subroutine (vecelem_addouruniq) */

static int ugcmp(cvoid *v1p,cvoid *v2p) noex {
	GRMEMS_USERGID	*g1p = (GRMEMS_USERGID *) v1p ;
	GRMEMS_USERGID	*g2p = (GRMEMS_USERGID *) v2p ;
	cint		rc = (g1p->gid - g2p->gid) ;
	return rc ;
}
/* end subroutine (ugcmp) */

static int mkvars() noex {
	int		rs ;
	if ((rs = getbufsize(getbufsize_un)) >= 0) {
	    var.usernamelen = rs ;
	    if ((rs = getbufsize(getbufsize_gn)) >= 0) {
	        var.groupnamelen = rs ;
	    }
	}
	return rs ;
}


