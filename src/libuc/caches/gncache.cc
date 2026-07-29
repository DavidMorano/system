/* gncache SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* group-name cache */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-01-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	gncache

	Description:
	This object provides a crude cache for GROUP-DB entries.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>		/* system-types */
#include	<ctime>			/* |time(2)| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strcmp(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>		/* |geustime(3u)| */
#include	<uclibmem.h>
#include	<bufsizeget.h>
#include	<getgroupname.h>
#include	<vechand.h>
#include	<cq.h>
#include	<sncpy.h>
#include	<strwcpy.h>
#include	<localmisc.h>		/* |GROUPNAMELEN| + |TIME_MAX| */

#include	"gncache.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| + |lenstr(3u)| */

/* local defines */

#define	GN		gncache

#define	TO_CHECK	5


/* imported namespaces */

using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

enum cts {
	ct_miss,
	ct_hit,
	ct_overlast
} ;

struct gncache_rec {
	char		*gn ;			/* memory-allocated */
	time_t		ti_create ;		/* creation time */
	time_t		ti_access ;		/* last access time */
	gid_t		gid ;
} ;

typedef	gncache_rec	rec ;
typedef	gncache_rec *	recp ;

namespace {
    struct vars {
	int		groupnamelen ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */

template<typename ... Args>
local int gncache_ctor(gncache *op,Args ... args) noex {
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    memclear(op) ;
	    if ((op->flp = new(nothrow) cq) != np) ylikely {
	        if ((op->rlp = new(nothrow) vechand) != np) ylikely {
		    rs = SR_OK ;
		}
		if (rs < 0) {
		    delete op->flp ;
		    op->flp = nullptr ;
		} /* end if (error) */
	    } /* end if (new-cq) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (gncache_ctor) */

local int gncache_dtor(gncache *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->rlp) ylikely {
		delete op->rlp ;
		op->rlp = nullptr ;
	    }
	    if (op->flp) ylikely {
		delete op->flp ;
		op->flp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (gncache_dtor) */

template<typename ... Args>
local inline int gncache_magic(gncache *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == GNCACHE_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (gncache_magic) */

local int gncache_searchgid(GN *,rec **,gid_t) noex ;
local int gncache_newrec(GN *,time_t,rec **,gid_t,cc *) noex ;
local int gncache_recaccess(GN *,rec *,time_t) noex ;
local int gncache_allocrec(GN *,rec **) noex ;
local int gncache_recfree(GN *,rec *) noex ;
local int gncache_maintenance(GN *,time_t) noex ;
local int gncache_record(GN *,int,int) noex ;

#ifdef	COMMENT
local int gncache_recdel(GN *,rec *) noex ;
#endif

local int record_start(rec *,time_t,gid_t,cchar *) noex ;
local int record_old(rec *,time_t,int) noex ;
local int record_refresh(rec *,time_t) noex ;
local int record_update(rec *,time_t,cchar *) noex ;
local int record_access(rec *,time_t) noex ;
local int record_finish(rec *) noex ;

local int mkvars() noex ;


/* local variables */

static vars		var ;

constexpr gid_t		gidend = -1 ;


/* exported variables */


/* exported subroutines */

int gncache_start(GN *op,int nmax,int to) noex {
	cint		defnum = GNCACHE_DEFENT ;
	int		rs ;
	if ((rs = gncache_ctor(op)) >= 0) ylikely {
	    static cint		rsv = mkvars() ;
	    if ((rs = rsv) >= 0) ylikely {
	        if (nmax < 4) nmax = GNCACHE_DEFMAX ;
	        if (to < 1) to = GNCACHE_DEFTTL ;
	        if ((rs = cq_start(op->flp)) >= 0) ylikely {
	            if ((rs = vechand_start(op->rlp,defnum,0)) >= 0) ylikely {
	                op->nmax = nmax ;
	                op->ttl = to ;
	                op->ti_check = time(nullptr) ;
	                op->magval = GNCACHE_MAGIC ;
	            }
	            if (rs < 0) {
	                cq_finish(op->flp) ;
	            } /* end if (error) */
	        } /* end if (cq-start) */
	    } /* end if (mkvars) */
	    if (rs < 0) {
		gncache_dtor(op) ;
	    } /* end if (error) */
	} /* end if (gncache_ctor) */
	return rs ;
} /* end subroutine (gncache_start) */

int gncache_finish(GN *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = gncache_magic(op)) >= 0) ylikely {
	    vechand	*rlp = op->rlp ;
	    void	*vp{} ;
	    if (rlp) ylikely {
	        for (int i = 0 ; vechand_get(rlp,i,&vp) >= 0 ; i += 1) {
	            if (vp) {
	                rec	*rp = recp(vp) ;
	                {
	                    rs1 = record_finish(rp) ;
	                    if (rs >= 0) rs = rs1 ;
		        }
	                {
	                    rs1 = lm_free(rp) ;
	                    if (rs >= 0) rs = rs1 ;
		        }
	            } /* end if (non-null) */
	        } /* end for */
	    }
	    if (rlp) ylikely {
	        rs1 = vechand_finish(rlp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->flp) ylikely {
	        while (cq_rem(op->flp,&vp) >= 0) {
	            rs1 = lm_free(vp) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end while */
	    }
	    if (op->flp) ylikely {
	        rs1 = cq_finish(op->flp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = gncache_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (gncache_finish) */

int gncache_add(GN *op,gid_t gid,cchar *gn) noex {
	time_t		dt = time(nullptr) ;
	int		rs ;
	int		gl = 0 ;
	if ((rs = gncache_magic(op,gn)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if ((gid != gidend) && gn[0]) {
	       rec	*rp{} ;
	       if ((rs = gncache_searchgid(op,&rp,gid)) >= 0) {
	           gl = rs ;
	           rs = record_update(rp,dt,gn) ;
	       } else if (rs == SR_NOTFOUND) {
	           rs = gncache_newrec(op,dt,nullptr,gid,gn) ;
	           gl = rs ;
	       }
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? gl : rs ;
} /* end subroutine (gncache_add) */

int gncache_lookgid(GN *op,char *rbuf,int rlen,gid_t gid) noex {
	custime		dt = getustime ;
	int		rs ;
	int		rs1 ;
	int		gl = 0 ;
	if ((rs = gncache_magic(op,rbuf)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if ((gid != gidend) && (rlen > 0)) {
	        int	ct{} ;
	        rec	*rp{} ;
	        if ((rs = gncache_searchgid(op,&rp,gid)) >= 0) {
	            ct = ct_hit ;
	            rs = gncache_recaccess(op,rp,dt) ;
	            gl = rs ;
	        } else if (rs == SR_NOTFOUND) {
	            if (char *gbuf ; (rs = lm_gn(&gbuf)) >= 0) {
			cint	glen = rs ;
	                ct = ct_miss ;
	                if ((rs = getgroupname(gbuf,glen,gid)) >= 0) {
	                    rs = gncache_newrec(op,dt,&rp,gid,gbuf) ;
	                    gl = rs ;
	                }
		        rs1 = lm_free(gbuf) ;
			if (rs >= 0) rs = rs1 ;
		    } /* end if (m-a-f) */
	        } /* end if (search-gid) */
	        {
	            gncache_record(op,ct,rs) ;
	            if (rs == 0) rs = SR_NOTFOUND ;
	        }
	        if (rs >= 0) {
		    if ((rs = sncpy(rbuf,rlen,rp->gn)) >= 0) {
	                rs = gncache_maintenance(op,dt) ;
		    }
	        } /* end if */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? gl : rs ;
} /* end subroutine (gncache_lookgid) */

int gncache_getstats(GN *op,gncache_st *sp) noex {
	int		rs ;
	if ((rs = gncache_magic(op,sp)) >= 0) ylikely {
	    if ((rs = vechand_count(op->rlp)) >= 0) ylikely {
	        *sp = op->s ;
	        sp->nentries = rs ;
	    }
	} /* end if (magic) */
	return rs ;
} /* end subroutine (gncache_getstats) */

int gncache_check(GN *op,time_t dt) noex {
	int		rs ;
	int		f = false ;
	if ((rs = gncache_magic(op)) >= 0) ylikely {
	    if (dt == 0) dt = time(nullptr) ;
	    if ((dt - op->ti_check) >= TO_CHECK) {
	        f = true ;
	        op->ti_check = dt ;
	        rs = gncache_maintenance(op,dt) ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (gncache_check) */


/* private subroutines */

local int gncache_newrec(GN *op,time_t dt,rec **rpp,gid_t gid,cc *gn) noex {
	rec		*rp{} ;
	int		rs ;
	int		gl = 0 ;
	if ((rs = gncache_allocrec(op,&rp)) >= 0) ylikely {
	    if ((rs = record_start(rp,dt,gid,gn)) >= 0) ylikely {
	        gl = rs ;
	        rs = vechand_add(op->rlp,rp) ;
	        if (rs < 0) {
	            record_finish(rp) ;
		} /* end if (error) */
	    } /* end if (record-start) */
	    if (rs < 0) {
	        lm_free(rp) ;
	    } /* end if (error) */
	} /* end if */
	if (rpp) {
	    *rpp = (rs >= 0) ? rp : nullptr ;
	}
	return (rs >= 0) ? gl : rs ;
} /* end subroutine (gncache_newrec) */

local int gncache_recaccess(GN *op,rec *rp,time_t dt) noex {
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
} /* end subroutine (gncache_recaccess) */

local int gncache_searchgid(GN *op,rec **rpp,gid_t gid) noex {
	vechand		*rlp = op->rlp ;
	int		rs ;
	int		gl = 0 ;
	rec		*rp = nullptr ;
	void		*vp{} ;
	bool		f = false ;
	for (int i = 0 ; (rs = vechand_get(rlp,i,&vp)) >= 0 ; i += 1) {
	    if (vp) {
		rp = recp(vp) ;
	        f = (rp->gid == gid) ;
	    }
	    if (f) break ;
	} /* end for */
	if ((rs >= 0) && f && rp) {
	    gl = lenstr(rp->gn) ;
	}
	if (rpp) {
	    *rpp = ((rs >= 0) && f) ? rp : nullptr ;
	}
	return (rs >= 0) ? gl : rs ;
} /* end subroutine (gncache_searchgid) */

local int gncache_maintenance(GN *op,time_t dt) noex {
	vechand		*rlp = op->rlp ;
	time_t		ti_oldest = TIME_MAX ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		iold = -1 ; /* the oldest one */
	/* delete expired entries */
	void		*vp{} ;
	for (int i = 0 ; vechand_get(rlp,i,&vp) >= 0 ; i += 1) {
	    if (vp) {
		rec	*rp = recp(vp) ;
	        if ((dt - rp->ti_create) >= op->ttl) {
	            vechand_del(rlp,i) ;
	            record_finish(rp) ;
	            gncache_recfree(op,rp) ;
	        } else {
	            if (rp->ti_access < ti_oldest) {
	                ti_oldest = rp->ti_access ;
	                iold = i ;
	            }
	        } /* end if */
	    } /* end if (non-null) */
	} /* end for */
	/* delete entries (at least one) if we are too big */
	if ((rs >= 0) && (iold >= 0)) {
	    if ((rs = vechand_count(rlp)) > op->nmax) {
	        rs1 = vechand_get(op->rlp,iold,&vp) ;
	        if ((rs1 >= 0) && vp) {
		    rec		*rp = recp(vp) ;
	            vechand_del(op->rlp,iold) ;
	            record_finish(rp) ;
	            gncache_recfree(op,rp) ;
	        }
	    } /* end if (vechand_get) */
	} /* end if */
	return rs ;
} /* end subroutine (gncache_maintenance) */

local int gncache_allocrec(GN *op,rec **rpp) noex {
	int		rs ;
	if ((rs = cq_rem(op->flp,rpp)) == SR_NOTFOUND) {
	    cint	sz = sizeof(rec) ;
	    if (void *vp ; (rs = lm_mall(sz,&vp)) >= 0) {
	        *rpp = recp(vp) ;
	    } /* end if (memory-acquire) */
	} /* end if (cq_rem) */
	return rs ;
} /* end subroutine (gncache_allocrec) */

#ifdef	COMMENT
local int gncache_recdel(GN *op,rec *ep) noex {
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
} /* end subroutine (gncache_recdel) */
#endif /* COMMENT */

local int gncache_recfree(GN *op,rec *rp) noex {
	int		rs = SR_OK ;
	int		n = cq_count(op->flp) ;
	if (n < GNCACHE_MAXFREE) {
	    rs = cq_ins(op->flp,rp) ;
	    if (rs < 0) {
	        lm_free(rp) ;
	    } /* end if (error) */
	} else {
	    lm_free(rp) ;
	}
	return rs ;
} /* end subroutine (gncache_recfree) */

local int gncache_record(GN *op,int ct,int rs) noex {
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
} /* end subroutine (gncache_record) */

local int record_start(rec *rp,time_t dt,gid_t gid,cchar *gn) noex {
	int		rs = SR_FAULT ;
	int		gl = 0 ;
	if (rp && gn) ylikely {
	    rs = SR_INVALID ;
	    if (gn[0]) ylikely {
	        cint	gnl = var.groupnamelen ;
	        if (dt == 0) dt = time(nullptr) ;
	        memclear(rp) ;
	        rp->gid = gid ;
	        rp->ti_create = dt ;
	        rp->ti_access = dt ;
		if (char *cp ; (rs = lm_gn(&cp)) >= 0) ylikely {
		    rp->gn = cp ;
	            gl = intconv(strwcpy(rp->gn,gn,gnl) - rp->gn) ;
		} /* end if (memory-acquire) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? gl : rs ;
} /* end subroutine (record_start) */

local int record_finish(rec *rp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (rp) ylikely {
	    rs = SR_OK ;
	    rp->gid = -1 ;
	    if (rp->gn) ylikely {
	        rp->gn[0] = '\0' ;
		rs1 = lm_free(rp->gn) ;
		if (rs >= 0) rs = rs1 ;
		rp->gn = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (record_finish) */

local int record_old(rec *rp,time_t dt,int ttl) noex {
	int		rs = SR_FAULT ;
	int		f = false ;
	if (rp) ylikely {
	    rs = SR_OK ;
	    f = ((dt - rp->ti_create) >= ttl) ;
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (record_old) */

local int record_refresh(rec *rp,time_t dt) noex {
	int		rs ;
	int		rs1 ;
	int		gl = 0 ;
	if (char *gbuf ; (rs = lm_gn(&gbuf)) >= 0) ylikely {
	    cint	glen = rs ;
	    if ((rs = getgroupname(gbuf,glen,rp->gid)) >= 0) ylikely {
	        gl = rs ;
	        rs = record_update(rp,dt,gbuf) ;
	    }
	    rs1 = lm_free(gbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? gl : rs ;
} /* end subroutine (record_refresh) */

local int record_update(rec *rp,time_t dt,cchar *gn) noex {
	int		rs = SR_FAULT ;
	int		f_changed = false ;
	if (rp) ylikely {
	    rp->ti_create = dt ;
	    rp->ti_access = dt ;
	    f_changed = (strcmp(rp->gn,gn) != 0) ;
	    if (f_changed) {
		cint	gnl = var.groupnamelen ;
	        strwcpy(rp->gn,gn,gnl) ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? f_changed : rs ;
} /* end subroutine (record_update) */

local int record_access(rec *rp,time_t dt) noex {
	cint		gl = lenstr(rp->gn) ;
	rp->ti_access = dt ;
	return gl ;
} /* end subroutine (record_access) */

local int mkvars() noex {
	int		rs ;
	if ((rs = bufsizeget(bufsize_gn)) >= 0) ylikely {
	    var.groupnamelen = rs ;
	}
	return rs ;
} /* end subroutine (mkvars) */


