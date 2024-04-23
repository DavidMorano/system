/* gncache SUPPORT */
/* lang=C++20 */

/* group-name cache */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-01-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object provides a crude cache for GROUP-DB entries.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>		/* system-types */
#include	<ctime>			/* |time(2)| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<vechand.h>
#include	<cq.h>
#include	<strwcpy.h>
#include	<localmisc.h>		/* |GROUPNAMELEN| + |TIME_MAX| */

#include	"gncache.h"


/* local defines */

#define	TO_CHECK	5


/* external subroutines */

extern int	getgroupname(char *,int,gid_t) ;


/* local structures */

enum cts {
	ct_miss,
	ct_hit,
	ct_overlast
} ;

struct gncache_rec {
	time_t		ti_create ;		/* creation time */
	time_t		ti_access ;		/* last access time */
	gid_t		gid ;
	char		gn[GROUPNAMELEN + 1] ;
} ;

typedef	gncache_rec	rec ;
typedef	gncache_rec *	recp ;


/* forward references */

template<typename ... Args>
static int gncache_ctor(gncache *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(op) ;
	    if ((op->flp = new(nothrow) cq) != np) {
	        if ((op->rlp = new(nothrow) vechand) != np) {
		    rs = SR_OK ;
		}
		if (rs < 0) {
		    delete op->flp ;
		    op->flp = nullptr ;
		}
	    } /* end if (new-cq) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (gncache_ctor) */

static int gncache_dtor(gncache *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->rlp) {
		delete op->rlp ;
		op->rlp = nullptr ;
	    }
	    if (op->flp) {
		delete op->flp ;
		op->flp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (gncache_dtor) */

template<typename ... Args>
static inline int gncache_magic(gncache *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == GNCACHE_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (gncache_magic) */

static int gncache_searchgid(GNCACHE *,GNCACHE_REC **,gid_t) ;
static int gncache_newrec(GNCACHE *,time_t,GNCACHE_REC **,gid_t,cchar *) ;
static int gncache_recaccess(GNCACHE *,GNCACHE_REC *,time_t) ;
static int gncache_allocrec(GNCACHE *,GNCACHE_REC **) ;
static int gncache_recfree(GNCACHE *,GNCACHE_REC *) ;
static int gncache_maintenance(GNCACHE *,time_t) ;
static int gncache_record(GNCACHE *,int,int) ;

#ifdef	COMMENT
static int gncache_recdel(GNCACHE *,GNCACHE_REC *) ;
#endif

static int record_start(GNCACHE_REC *,time_t,gid_t,const char *) ;
static int record_old(GNCACHE_REC *,time_t,int) ;
static int record_refresh(GNCACHE_REC *,time_t) ;
static int record_update(GNCACHE_REC *,time_t,const char *) ;
static int record_access(GNCACHE_REC *,time_t) ;
static int record_finish(GNCACHE_REC *) ;

static int	entry_load(GNCACHE_ENT *,GNCACHE_REC *) ;


/* local variables */


/* exported variables */


/* exported subroutines */

int gncache_start(GNCACHE *op,int nmax,int to) noex {
	cint		defnum = GNCACHE_DEFENT ;
	int		rs ;
	if ((rs = gncache_ctor(op)) >= 0) {
	    if (nmax < 4) nmax = GNCACHE_DEFMAX ;
	    if (to < 1) to = GNCACHE_DEFTTL ;
	    if ((rs = cq_start(op->flp)) >= 0) {
	        if ((rs = vechand_start(&op->recs,defnum,0)) >= 0) {
	            op->nmax = nmax ;
	            op->ttl = to ;
	            op->ti_check = time(nullptr) ;
	            op->magic = GNCACHE_MAGIC ;
	        }
	        if (rs < 0) {
	            cq_finish(op->flp) ;
	        }
	    } /* end if (cq-start) */
	    if (rs < 0) {
		gncache_dtor(op) ;
	    }
	} /* end if (gncache_dtor) */
	return rs ;
}
/* end subroutine (gncache_start) */


int gncache_finish(GNCACHE *op)
{
	GNCACHE_REC	*rp ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		i ;
	void		*vp ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != GNCACHE_MAGIC) return SR_NOTOPEN ;

/* loop freeing up all cache entries */

	for (i = 0 ; vechand_get(&op->recs,i,&rp) >= 0 ; i += 1) {
	    if (rp == nullptr) continue ;
	    rs1 = record_finish(rp) ;
	    if (rs >= 0) rs = rs1 ;
	    rs1 = uc_free(rp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end while */
	if (rs >= 0) rs = rs1 ;

	rs1 = vechand_finish(&op->recs) ;
	if (rs >= 0) rs = rs1 ;

	while (cq_rem(op->flp,&vp) >= 0) {
	    rs1 = uc_free(vp) ;
	    if (rs >= 0) rs = rs1 ;
	}

	rs1 = cq_finish(op->flp) ;
	if (rs >= 0) rs = rs1 ;

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (gncache_finish) */


int gncache_add(GNCACHE *op,gid_t gid,cchar gn[])
{
	GNCACHE_REC	*rp ;
	time_t		dt = time(nullptr) ;
	int		rs = SR_OK ;
	int		gl = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (gn == nullptr) return SR_FAULT ;

	if (op->magic != GNCACHE_MAGIC) return SR_NOTOPEN ;

	if (gid < 0) return SR_INVALID ;

	if (gn[0] == '\0') return SR_INVALID ;

	if ((rs = gncache_searchgid(op,&rp,gid)) >= 0) {
	    gl = rs ;
	    rs = record_update(rp,dt,gn) ;
	} else if (rs == SR_NOTFOUND) {
	    rs = gncache_newrec(op,dt,nullptr,gid,gn) ;
	    gl = rs ;
	}

	return (rs >= 0) ? gl : rs ;
}
/* end subroutine (gncache_add) */


int gncache_lookgid(GNCACHE *op,GNCACHE_ENT *ep,gid_t gid)
{
	GNCACHE_REC	*rp ;
	const time_t	dt = time(nullptr) ;
	int		rs ;
	int		ct ;
	int		gl = 0 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != GNCACHE_MAGIC) return SR_NOTOPEN ;

	if (gid < 0) return SR_INVALID ;

	if ((rs = gncache_searchgid(op,&rp,gid)) >= 0) {
	    ct = ct_hit ;
	    rs = gncache_recaccess(op,rp,dt) ;
	    gl = rs ;
	} else if (rs == SR_NOTFOUND) {
	    char	gn[GROUPNAMELEN + 1] ;
	    ct = ct_miss ;
	    if ((rs = getgroupname(gn,GROUPNAMELEN,gid)) >= 0) {
	        rs = gncache_newrec(op,dt,&rp,gid,gn) ;
	        gl = rs ;
	    }
	} /* end if (search-gid) */

	gncache_record(op,ct,rs) ;
	if (rs == 0) rs = SR_NOTFOUND ;

	if (rs >= 0) {
	    if (ep != nullptr) rs = entry_load(ep,rp) ;
	    gncache_maintenance(op,dt) ;
	} /* end if */

	return (rs >= 0) ? gl : rs ;
}
/* end subroutine (gncache_lookgid) */


int gncache_stats(GNCACHE *op,GNCACHE_STATS *sp)
{
	int		rs ;

	if (op == nullptr) return SR_FAULT ;
	if (sp == nullptr) return SR_FAULT ;

	if (op->magic != GNCACHE_MAGIC) return SR_NOTOPEN ;

	if ((rs = vechand_count(&op->recs)) >= 0) {
	    *sp = op->s ;
	    sp->nentries = rs ;
	}
	return rs ;
}
/* end subroutine (gncache_stats) */


int gncache_check(GNCACHE *op,time_t dt)
{
	int		rs = SR_OK ;
	int		f = false ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != GNCACHE_MAGIC) return SR_NOTOPEN ;

	if (dt == 0)
	    dt = time(nullptr) ;

	if ((dt - op->ti_check) >= TO_CHECK) {
	    f = true ;
	    op->ti_check = dt ;
	    rs = gncache_maintenance(op,dt) ;
	}

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (gncache_check) */


/* private subroutines */

static int gncache_newrec(GNCACHE *op,time_t dt,GNCACHE_REC **rpp,gid_t gid,
		cchar *gn) noex {
	GNCACHE_REC	*rp{} ;
	int		rs ;
	int		gl = 0 ;
	if ((rs = gncache_allocrec(op,&rp)) >= 0) {
	    if ((rs = record_start(rp,dt,gid,gn)) >= 0) {
	        gl = rs ;
	        rs = vechand_add(&op->recs,rp) ;
	        if (rs < 0) {
	            record_finish(rp) ;
		}
	    } /* end if (record-start) */
	    if (rs < 0) {
	        uc_free(rp) ;
	    }
	} /* end if */
	if (rpp) {
	    *rpp = (rs >= 0) ? rp : nullptr ;
	}
	return (rs >= 0) ? gl : rs ;
}
/* end subroutine (gncache_newrec) */

static int gncache_recaccess(GNCACHE *op,GNCACHE_REC *rp,time_t dt) noex {
	int		rs ;
	int		gl = 0 ;
	if ((rs = record_old(rp,dt,op->ttl)) > 0) {
	    rs = record_refresh(rp,dt) ;
	    gl = rs ;
	} else {
	    rs = record_access(rp,dt) ;
	    gl = rs ;
	}
	return (rs >= 0) ? gl : rs ;
}
/* end subroutine (gncache_recaccess) */

static int gncache_searchgid(GNCACHE *op,GNCACHE_REC **rpp,gid_t gid) noex {
	vechand		*rlp = op->rlp ;
	int		rs = SR_OK ;
	int		gl = 0 ;
	for (int i = 0 ; (rs = vechand_get(rlp,i,rpp)) >= 0 ; i += 1) {
	    if (*rpp == nullptr) continue ;
	    if ((*rpp)->gid == gid)
	        break ;
	} /* end for */
	if (rs >= 0) {
	    gl = strlen((*rpp)->gn) ;
	}
	return (rs >= 0) ? gl : rs ;
}
/* end subroutine (gncache_searchgid) */

static int gncache_maintenance(GNCACHE *op,time_t dt) noex {
	vechand		*rlp = op->rlp ;
	GNCACHE_REC	*rp ;
	time_t		ti_oldest = TIME_MAX ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		n ;
	int		iold = -1 ; /* the oldest one */
/* delete expired entries */
	for (int i = 0 ; vechand_get(rlp,i,&rp) >= 0 ; i += 1) {
	    if (rp == nullptr) continue ;
	    if ((dt - rp->ti_create) >= op->ttl) {
	        vechand_del(&op->recs,i) ;
	        record_finish(rp) ;
	        gncache_recfree(op,rp) ;
	    } else {
	        if (rp->ti_access < ti_oldest) {
	            ti_oldest = rp->ti_access ;
	            iold = i ;
	        }
	    }
	} /* end for */
/* delete entries (at least one) if we are too big */
	if ((rs >= 0) && (iold >= 0)) {
	    n = vechand_count(&op->recs) ;
	    if (n > op->nmax) {
	        rs1 = vechand_get(&op->recs,iold,&rp) ;
	        if ((rs1 >= 0) && (rp != nullptr)) {
	            vechand_del(&op->recs,iold) ;
	            record_finish(rp) ;
	            gncache_recfree(op,rp) ;
	        }
	    }
	} /* end if */
	return rs ;
}
/* end subroutine (gncache_maintenance) */

static int gncache_allocrec(GNCACHE *op,GNCACHE_REC **rpp) noex {
	cint		sz = sizeof(GNCACHE_REC) ;
	int		rs ;
	if ((rs = cq_rem(op->flp,rpp)) == SR_NOTFOUND) {
	    void	*vp ;
	    rs = uc_malloc(sz,&vp) ;
	    if (rs >= 0) *rpp = vp ;
	}
	return rs ;
}
/* end subroutine (gncache_allocrec) */

#ifdef	COMMENT
static int gncache_recdel(GNCACHE *op,GNCACHE_REC *ep) noex {
	int		rs ;
	int		rs1 ;
	if ((rs1 = vechand_ent(&op->db,ep)) >= 0) {
	    rs1 = vechand_del(rs1) ;
	}
	if (rs >= 0) rs = rs1 ;
	{
	    rs1 = record_finish(ep) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (gncache_recdel) */
#endif /* COMMENT */

static int gncache_recfree(GNCACHE *op,GNCACHE_REC *rp) noex {
	int		rs = SR_OK ;
	int		n = cq_count(op->flp) ;
	if (n < GNCACHE_MAXFREE) {
	    rs = cq_ins(op->flp,rp) ;
	    if (rs < 0) {
	        uc_free(rp) ;
	    }
	} else {
	    uc_free(rp) ;
	}
	return rs ;
}
/* end subroutine (gncache_recfree) */

static int gncache_record(GNCACHE *op,int ct,int rs) noex {
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
/* end subroutine (gncache_record) */

static int record_start(GNCACHE_REC *rp,time_t dt,gid_t gid,cchar *gn) noex {
	int		rs = SR_FAULT OK ;
	int		gl = 0 ;
	if (rp && gn) {
	    rs = SR_INVALID ;
	    if (gn[0]) {
	     if (dt == 0) dt = time(nullptr) ;
	     memclear(rp) ;
	     rp->gid = gid ;
	     rp->ti_create = dt ;
	     rp->ti_access = dt ;
	     gl = strwcpy(rp->gn,gn,GROUPNAMELEN) - rp->gn ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? gl : rs ;
}
/* end subroutine (record_start) */

static int record_finish(GNCACHE_REC *rp) noex {
	int		rs = SR_FAULT ;
	if (rp) {
	    rs = SR_OK ;
	    rp->gid = -1 ;
	    rp->gn[0] = '\0' ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (record_finish) */

static int record_old(GNCACHE_REC *rp,time_t dt,int ttl) noex {
	int		rs = SR_FAULT ;
	int		f = false ;
	if (rp) {
	    rs = SR_OK ;
	    f = ((dt - rp->ti_create) >= ttl) ;
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (record_old) */

static int record_refresh(GNCACHE_REC *rp,time_t dt) noex {
	int		rs ;
	int		gl = 0 ;
	char		gn[GROUPNAMELEN + 1] ;
	if ((rs = getgroupname(gn,GROUPNAMELEN,rp->gid)) >= 0) {
	    gl = rs ;
	    rs = record_update(rp,dt,gn) ;
	}
	return (rs >= 0) ? gl : rs ;
}
/* end subroutine (record_refresh) */

static int record_update(GNCACHE_REC *rp,time_t dt,cchar *gn) noex {
	int		rs = SR_FAULT ;
	int		f_changed = false ;
	if (rp) {
	    rp->ti_create = dt ;
	    rp->ti_access = dt ;
	    f_changed = (strcmp(rp->gn,gn) != 0) ;
	    if (f_changed) {
	        strwcpy(rp->gn,gn,GROUPNAMELEN) ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? f_changed : rs ;
}
/* end subroutine (record_update) */

static int record_access(GNCACHE_REC *rp,time_t dt) noex {
{
	int		gl ;
	rp->ti_access = dt ;
	gl = strlen(rp->gn) ;
	return gl ;
}
/* end subroutine (record_access) */

static int entry_load(GNCACHE_ENT *ep,GNCACHE_REC *rp) noex {
	cint		gnl = GROUPNAMELEN ;
	int		gl ;
	ep->gid = rp->gid ;
	gl = strwcpy(ep->groupname,rp->gn,gnl) - ep->groupname ;
	return gl ;
}
/* end subroutine (entry_load) */


