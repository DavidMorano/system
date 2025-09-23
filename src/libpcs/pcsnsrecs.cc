/* pcsnsrecs SUPPORT */
/* lang=C++20 */

/* PCS Name-Server Records */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-01-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object provides storage for PCS Name-Server records.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<cstdlib>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstring>
#include	<usystem.h>
#include	<recarr.h>
#include	<snwcpy.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"pcsnsrecs.h"


/* local defines */

#ifndef	PCSNSRECS_REC
#define	PCSNSRECS_REC	struct pcsnsrecs_r
#endif

#define	RECS		PCSNSRECS
#define	REC		PCSNSRECS_REC

#define	RECINFO		struct recinfo


/* external subroutines */


/* external variables */


/* local structures */

struct recinfo {
	cchar		*un ;
	cchar		*vbuf ;
	int		vlen ;
	int		w ;
	int		ttl ;		/* time-to-live */
} ;

enum cts {
	ct_null,
	ct_miss,
	ct_hit,
	ct_overlast
} ;

struct pcsnsrecs_r {
	pq_ent		linkage ;
	char		*vbuf ;		/* value buffer */
	time_t		ti_create ;
	time_t		ti_access ;
	uint		wcount ;	/* write count */
	int		vlen ;		/* value length */
	int		vl ;
	int		recidx ;	/* record index */
	int		w ;		/* "which" number */
	int		ttl ;
	char		un[USERNAMELEN+1] ; /* key */
} ;


/* forward references */

static int	pcsnsrecs_fetch(RECS *,PCSNSRECS_REC **,cchar *,int) noex ;
static int	pcsnsrecs_mkrec(RECS *,time_t,RECINFO *) noex ;
static int	pcsnsrecs_newrec(RECS *,time_t,
			PCSNSRECS_REC **,RECINFO *) noex ;
static int	pcsnsrecs_recstart(RECS *,time_t,
			PCSNSRECS_REC *,RECINFO *) noex ;
static int	pcsnsrecs_recdel(RECS *,PCSNSRECS_REC *) noex ;
static int	pcsnsrecs_recaccess(RECS *,time_t,PCSNSRECS_REC *) noex ;
static int	pcsnsrecs_recrear(RECS *,PCSNSRECS_REC *) noex ;
static int	pcsnsrecs_recfins(RECS *) noex ;
static int	pcsnsrecs_upstats(RECS *,int,int) noex ;

static int record_start(PCSNSRECS_REC *,time_t,int,RECINFO *) noex ;
static int record_access(PCSNSRECS_REC *,time_t) noex ;
static int record_old(PCSNSRECS_REC *,time_t,int) noex ;
static int record_finish(PCSNSRECS_REC *) noex ;
static int record_get(PCSNSRECS_REC *,char *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int pcsnsrecs_start(RECS *op,int nmax,int ttl) noex {
	int		rs ;

	if (op == NULL) return SR_FAULT ;

	if (nmax < PCSNSRECS_DEFMAX)
	    nmax = PCSNSRECS_DEFMAX ;

	if (ttl < PCSNSRECS_DEFTTL)
	    ttl = PCSNSRECS_DEFTTL ;

	memclear(op) ;

	{
	    cint	sz = sizeof(RECARR) ;
	    char	*p ;
	    if ((rs = lm_mall(sz,&p)) >= 0) {
	        int	ro = 0 ;
	        ro |= RECARR_OSTATIONARY ;
	        ro |= RECARR_OREUSE ;
	        ro |= RECARR_OCONSERVE ;
	        op->recs = p ;
	        if ((rs = recarr_start(op->recs,nmax,ro)) >= 0) {
	            if ((rs = pq_start(&op->lru)) >= 0) {
	                op->nmax = nmax ;
	                op->ttl = ttl ;
	                op->ti_check = time(NULL) ;
	                op->magic = PCSNSRECS_MAGIC ;
	            }
	            if (rs < 0)
	                recarr_finish(op->recs) ;
	        }
	        if (rs < 0) {
	            lm_free(op->recs) ;
	            op->recs = NULL ;
	        }
	    } /* end if (memory-allocation) */
	} /* end block */

	return rs ;
}
/* end subroutine (pcsnsrecs_start) */

int pcsnsrecs_finish(RECS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != PCSNSRECS_MAGIC) return SR_NOTOPEN ;

/* finish up the LRU queue */

	rs1 = pq_finish(&op->lru) ;
	if (rs >= 0) rs = rs1 ;

/* loop freeing up all cache entries */

	rs1 = pcsnsrecs_recfins(op) ;
	if (rs >= 0) rs = rs1 ;

/* free up everyting else */

	rs1 = recarr_finish(op->recs) ;
	if (rs >= 0) rs = rs1 ;

	if (op->recs != NULL) {
	    rs1 = lm_free(op->recs) ;
	    if (rs >= 0) rs = rs1 ;
	    op->recs = NULL ;
	}

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (pcsnsrecs_finish) */

int pcsnsrecs_store(RECS *op,cchar *vbuf,int vlen,cchar *un,
		int w,int ttl) noex {
	RECINFO		ri{} ;
	custime		dt = getustime ;
	int		rs ;

	if (op == NULL) return SR_FAULT ;
	if (vbuf == NULL) return SR_FAULT ;
	if (un == NULL) return SR_FAULT ;

	if (op->magic != PCSNSRECS_MAGIC) return SR_NOTOPEN ;

	if (vlen < 1) return SR_INVALID ;
	if (un[0] == '\0') return SR_INVALID ;

	ri.un = un ;
	ri.vbuf = vbuf ;
	ri.vlen = vlen ;
	ri.w = w ;
	ri.ttl = ttl ;
	rs = pcsnsrecs_mkrec(op,dt,&ri) ;

	return rs ;
}
/* end subroutine (pcsnsrecs_store) */

int pcsnsrecs_lookup(RECS *op,char *rbuf,int rlen,cchar *un,int w) noex {
	PCSNSRECS_REC	*ep = NULL ;
	int		rs ;
	int		ct = 0 ;

	if (op == NULL) return SR_FAULT ;
	if (rbuf == NULL) return SR_FAULT ;
	if (un == NULL) return SR_FAULT ;

	if (op->magic != PCSNSRECS_MAGIC) return SR_NOTOPEN ;

	if (rlen < 1) return SR_INVALID ;
	if (un[0] == '\0') return SR_INVALID ;

	op->s.total += 1 ;

	if ((rs = pcsnsrecs_fetch(op,&ep,un,w)) >= 0) {
	    time_t	dt = time(NULL) ;
	    if ((rs = record_old(ep,dt,op->ttl)) > 0) {
	        ct = ct_miss ;
	        if ((rs = pcsnsrecs_recdel(op,ep)) >= 0) {
	            pq_ent	*pep = (pq_ent *) ep ;
	            rs = pq_unlink(&op->lru,pep) ;
	            lm_free(ep) ;
		    if (rs >= 0) rs = 0 ;
	        }
	    } else {
	        ct = ct_hit ;
	        rs = pcsnsrecs_recaccess(op,dt,ep) ;
	    }
	} else if (rs == SR_NOTFOUND) {
	    ct = ct_miss ;
	} /* end if (hit or miss) */

	pcsnsrecs_upstats(op,ct,rs) ;

	if (rs > 0) { /* not '>=' */
	    rs = record_get(ep,rbuf,rlen) ;
	} else if (rs == 0) {
	    rs = SR_NOTFOUND ;
	}

	if (rs <= 0) rbuf[0] = '\0' ;

	return rs ;
}
/* end subroutine (pcsnsrecs_lookup) */

int pcsnsrecs_invalidate(RECS *op,cchar *un,int w) noex {
	PCSNSRECS_REC	*ep ;
	int		rs ;
	int		rs1 ;
	int		f_found = FALSE ;

	if (op == NULL) return SR_FAULT ;
	if (un == NULL) return SR_FAULT ;

	if (op->magic != PCSNSRECS_MAGIC) return SR_NOTOPEN ;

	if (un[0] == '\0') return SR_INVALID ;

	if ((rs = pcsnsrecs_fetch(op,&ep,un,w)) >= 0) {
	    cint	ri = rs ;
	    pq_ent	*pep = (pq_ent *) ep ;

	    f_found = TRUE ;

	    rs1 = pq_unlink(&op->lru,pep) ;
	    if (rs >= 0) rs = rs1 ;

	    rs1 = recarr_del(op->recs,ri) ;
	    if (rs >= 0) rs = rs1 ;

	    rs1 = record_finish(ep) ;
	    if (rs >= 0) rs = rs1 ;

	    rs1 = lm_free(ep) ;
	    if (rs >= 0) rs = rs1 ;

	} else if (rs == SR_NOTFOUND) {
	    rs = SR_OK ;
	}

	return (rs >= 0) ? f_found : rs ;
}
/* end subroutine (pcsnsrecs_invalidate) */

int pcsnsrecs_check(RECS *op,time_t dt) noex {
	PCSNSRECS_REC	*ep ;
	RECARR		*rlp = op->recs ;
	int		rs ;
	int		i ;
	int		f = FALSE ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != PCSNSRECS_MAGIC) return SR_NOTOPEN ;

/* loop checking all cache entries */

	for (i = 0 ; recarr_get(rlp,i,&ep) >= 0 ; i += 1) {
	    if (ep != NULL) {
	        if (dt == 0) dt = time(NULL) ;
	        if ((rs = record_old(ep,dt,op->ttl)) > 0) {
	            f = TRUE ;
	            if ((rs = pcsnsrecs_recdel(op,ep)) >= 0) {
	                pq_ent	*pep = (pq_ent *) ep ;
	                rs = pq_unlink(&op->lru,pep) ;
	                lm_free(ep) ;
	            }
	        } /* end if (entry-old) */
	    }
	    if (rs < 0) break ;
	} /* end for */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (pcsnsrecs_check) */

int pcsnsrecs_stats(RECS *op,PCSNSRECS_ST *sp) noex {
	int		rs ;

	if (op == NULL) return SR_FAULT ;
	if (sp == NULL) return SR_FAULT ;

	if (op->magic != PCSNSRECS_MAGIC) return SR_NOTOPEN ;

	if ((rs = recarr_count(op->recs)) >= 0) {
	    *sp = op->s ;
	    sp->nentries = rs ;
	}

	return rs ;
}
/* end subroutine (pcsnsrecs_stats) */


/* private subroutines */

static int pcsnsrecs_fetch(RECS *op,PCSNSRECS_REC **epp,cchar *un,int w) noex {
	RECARR		*rlp = op->recs ;
	PCSNSRECS_REC	*ep ;
	int		rs ;
	int		i{} ;
	for (i = 0 ; (rs = recarr_get(rlp,i,&ep)) >= 0 ; i += 1) {
	    if (ep != NULL) {
	        if ((un[0] == ep->un[0]) && (w == ep->w)) {
	            if (strcmp(un,ep->un) == 0) break ;
	        }
	    }
	} /* end for */
	if (epp) {
	    *epp = (rs >= 0) ? ep : NULL ;
	}
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (pcsnsrecs_fetch) */

static int pcsnsrecs_mkrec(RECS *op,time_t dt,RECINFO *rip) noex {
	int		rs ;
	int		rs1 ;
	int		vl = 0 ;
	if ((rs = recarr_count(op->recs)) >= 0) {
	    pq_ent	*pep ;
	    cint	n = rs ;
	    if (n >= op->nmax) {
	        if ((rs = pq_rem(&op->lru,&pep)) >= 0) {
	            PCSNSRECS_REC	*ep = (PCSNSRECS_REC *) pep ;
	            if ((rs = pcsnsrecs_recdel(op,ep)) >= 0) {
	                rs = pcsnsrecs_recstart(op,dt,ep,rip) ;
	                vl = rs ;
	            }
	            rs1 = pq_ins(&op->lru,pep) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (removed entry) */
	    } else {
	        PCSNSRECS_REC	*ep ;
	        if ((rs = pcsnsrecs_newrec(op,dt,&ep,rip)) >= 0) {
	            vl = rs ;
	            if (ep != NULL) {
	                pep = (pq_ent *) ep ;
	                rs = pq_ins(&op->lru,pep) ;
	            }
	        } /* end if (new-entry) */
	    } /* end if */
	} /* end if (recarr_count) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (pcsnsrecs_mkrec) */

static int pcsnsrecs_newrec(RECS *op,time_t dt,
		PCSNSRECS_REC **epp,RECINFO *rip) noex {
	PCSNSRECS_REC	*ep ;
	cint	size = sizeof(PCSNSRECS_REC) ;
	int		rs ;

	if ((rs = lm_mall(size,&ep)) >= 0) {
	    rs = pcsnsrecs_recstart(op,dt,ep,rip) ;
	    if (rs < 0)
	        lm_free(ep) ;
	} /* end if (m-a) */

	*epp = (rs >= 0) ? ep : NULL ;
	return rs ;
}
/* end subroutine (pcsnsrecs_newrec) */


static int pcsnsrecs_recstart(RECS *op,time_t dt,PCSNSRECS_REC *ep,
		RECINFO *rip) noex {
	cint		wc = op->wcount++ ;
	int		rs ;
	int		rl = 0 ;
	if ((rs = record_start(ep,dt,wc,rip)) >= 0) {
	    rl = rs ;
	    rs = recarr_add(op->recs,ep) ;
	    ep->recidx = rs ;
	    if (rs < 0)
	        record_finish(ep) ;
	} /* end if (entry-start) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (pcsnsrecs_recstart) */

static int pcsnsrecs_recaccess(RECS *op,time_t dt,PCSNSRECS_REC *ep) noex {
	int		rs ;
	if ((rs = pcsnsrecs_recrear(op,ep)) >= 0) {
	    rs = record_access(ep,dt) ;
	} /* end if */
	return rs ;
}
/* end subroutine (pcsnsrecs_recaccess) */

static int pcsnsrecs_recrear(RECS *op,PCSNSRECS_REC *ep) noex {
	pq_ent		*pcp = (pq_ent *) ep ;
	pq_ent		*pep ;
	int		rs ;

	if ((rs = pq_gettail(&op->lru,&pep)) >= 0) {
	    if (pcp != pep) {
	        pep = (pq_ent *) ep ;
	        if ((rs = pq_unlink(&op->lru,pep)) >= 0) {
	            rs = pq_ins(&op->lru,pep) ;
	            if (rs < 0) {
	                PCSNSRECS_REC	*ep = (PCSNSRECS_REC *) pep ;
	                record_finish(ep) ;
	                lm_free(pep) ;
	            }
	        }
	    }
	} /* end if (pq-gettail) */

	return rs ;
}
/* end subroutine (pcsnsrecs_recrear) */

static int pcsnsrecs_recdel(RECS *op,PCSNSRECS_REC *ep) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if ((rs1 = recarr_ent(op->recs,ep)) >= 0) {
	    rs1 = recarr_del(op->recs,rs1) ;
	}
	if (rs >= 0) rs = rs1 ;

	rs1 = record_finish(ep) ;
	if (rs >= 0) rs = rs1 ;

	return rs ;
}
/* end subroutine (pcsnsrecs_recdel) */

static int pcsnsrecs_recfins(RECS *op) noex {
	RECARR		*rlp = op->recs ;
	PCSNSRECS_REC	*ep ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		i ;

	for (i = 0 ; recarr_get(rlp,i,&ep) >= 0 ; i += 1) {
	    if (ep != NULL) {
	        rs1 = record_finish(ep) ;
	        if (rs >= 0) rs = rs1 ;
	        rs1 = lm_free(ep) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */

	return rs ;
}
/* end subroutine (pcsnsrecs_recfins) */

static int pcsnsrecs_upstats(RECS *op,int ct,int rs) noex {
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
/* end subroutine (pcsnsrecs_upstats) */

static int record_start(PCSNSRECS_REC *ep,time_t dt,int wc,RECINFO *rip) noex {
	int		rs ;
	int		vlen ;
	int		vl = 0 ;
	char		*vbuf ;

	if (ep == NULL) return SR_FAULT ;
	if (rip == NULL) return SR_FAULT ;

	if (rip->un[0] == '\0') return SR_INVALID ;

	vlen = rip->vlen ;

	memclear(ep) ;
	ep->w = rip->w ;
	strwcpy(ep->un,rip->un,USERNAMELEN) ;

	if ((rs = lm_mall((vlen+1),&vbuf)) >= 0) {
	    vl = (strwcpy(vbuf,rip->vbuf,vlen) - vbuf) ;
	    ep->vbuf = vbuf ;
	    ep->vlen = vlen ;
	    ep->vl = vl ;
	} /* end if (m-a) */

	if (rs >= 0) {
	    ep->ti_create = dt ;
	    ep->ti_access = dt ;
	    ep->wcount = wc ;
	    ep->ttl = rip->ttl ;
	}

	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (record_start) */

static int record_finish(PCSNSRECS_REC *ep) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (ep == NULL) return SR_FAULT ;

	if (ep->vbuf != NULL) {
	    rs1 = lm_free(ep->vbuf) ;
	    if (rs >= 0) rs = rs1 ;
	    ep->vbuf = NULL ;
	    ep->vlen = 0 ;
	}

	ep->un[0] = '\0' ;
	return rs ;
}
/* end subroutine (record_finish) */

static int record_access(PCSNSRECS_REC *ep,time_t dt) noex {
	int		rs = SR_OK ;
	int		vl = 0 ;

	if (ep == NULL) return SR_FAULT ;

	ep->ti_access = dt ;
	vl = ep->vl ;
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (record_access) */

static int record_old(PCSNSRECS_REC *ep,time_t dt,int ttl) noex {
	int		f_old = false ;
	if (ep == NULL) return SR_FAULT ;
	if ((ep->ttl > 0) && (ep->ttl < ttl)) ttl = ep->ttl ;
	f_old = ((dt - ep->ti_create) >= ttl) ;
	return f_old ;
}
/* end subroutine (record_old) */

static int record_get(PCSNSRECS_REC *ep,char *rbuf,int rlen) noex {
	int		rs ;
	rs = snwcpy(rbuf,rlen,ep->vbuf,ep->vl) ;
	return rs ;
}
/* end subroutine (record_get) */


