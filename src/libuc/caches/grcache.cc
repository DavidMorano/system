/* grcache SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* GROUP cache */
/* version %I% last-modified %G% */

#define	CF_SEARCHGID	0		/* compile in 'searchgid()' */
#define	CF_MAINTEXTRA	0		/* perform extra maintenance */

/* revision history:

	= 2004-01-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	grcache

	Description:
	This object provides a crude cache for GROUP-DB entries.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>		/* system-types */
#include	<sys/param.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strcmp(3c)| */
#include	<new>			/* |nothrow(3c++)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<string>		/* |string(3c++)| */
#include	<usystem.h>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<getax.h>
#include	<vechand.h>
#include	<cq.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"grcache.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	RECORD_MAGIC	0x98643163

#define	GE		ucentgr

#define	TO_CHECK	5

#ifndef	CF_MAINTEXTRA
#define	CF_MAINTEXTRA	0		/* perform extra maintenance */
#endif


/* imported namespaces */

using libuc::libmem ;			/* variable */
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

struct grcache_rec {
	cchar		*gn ;
	char		*grbuf ;
	ucentgr		gr ;
	time_t		ti_create ;		/* creation time */
	time_t		ti_access ;		/* access time (last) */
	gid_t		gid ;
	uint		magic ;
	int		wcount ;
	int		grl ;
} ;

typedef grcache_rec	rec ;
typedef grcache_rec *	recp ;


/* forward references */

template<typename ... Args>
static int grcache_ctor(grcache *op,Args ... args) noex {
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
/* end subroutine (grcache_ctor) */

static int grcache_dtor(grcache *op) noex {
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
/* end subroutine (grcache_dtor) */

template<typename ... Args>
static inline int grcache_magic(grcache *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == GRCACHE_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (grcache_magic) */

static int grcache_searchname(grcache *,rec **,cchar *) noex ;
static int grcache_mkrec(grcache *,time_t,rec **,cchar *) noex ;
static int grcache_getrec(grcache *,time_t,rec **) noex ;
static int grcache_allocrec(grcache *,rec **) noex ;
static int grcache_recstart(grcache *,time_t,rec *,cchar *) noex ;
static int grcache_recrear(grcache *,rec *) noex ;
static int grcache_recaccess(grcache *,time_t,rec *) noex ;
static int grcache_recdel(grcache *,int,rec *) noex ;
static int grcache_recfree(grcache *,rec *) noex ;
static int grcache_maintenance(grcache *,time_t) noex ;
static int grcache_record(grcache *,int,int) noex ;

#if	CF_SEARCHGID
static int grcache_searchgid(grcache *,rec **,gid_t) noex ;
#endif /* CF_SEARCHGID */

static int record_start(rec *,time_t,int,cchar *) noex ;
static int record_access(rec *,time_t) noex ;
static int record_refresh(rec *,time_t,int) noex ;
static int record_old(rec *,time_t,int) noex ;
static int record_finish(rec *) noex ;


/* local variables */

constexpr time_t	timemax = TIME_MAX ;

constexpr gid_t		gidend = -1 ;

constexpr bool		f_maintextra = CF_MAINTEXTRA ;


/* exported variables */


/* exported subroutines */

int grcache_start(grcache *op,int nmax,int ttl) noex {
	cint		defnum = GRCACHE_DEFENTS ;
	int		rs ;
	if ((rs = grcache_ctor(op)) >= 0) {
	    if (nmax < 4) nmax = GRCACHE_DEFMAX ;
	    if (ttl < 1) ttl = GRCACHE_DEFTTL ;
	    if ((rs = cq_start(op->flp)) >= 0) {
	        if ((rs = vechand_start(op->rlp,defnum,0)) >= 0) {
		    op->nmax = nmax ;
		    op->ttl = ttl ;
		    op->ti_check = time(nullptr) ;
		    op->magic = GRCACHE_MAGIC ;
	        }
	        if (rs < 0) {
		    cq_finish(op->flp) ;
	        }
	    } /* end if (cq-start) */
	    if (rs < 0) {
		grcache_dtor(op) ;
	    }
	} /* end if (grcache_start) */
	return rs ;
}
/* end subroutine (grcache_start) */

int grcache_finish(grcache *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = grcache_magic(op)) >= 0) {
	    vechand	*rlp = op->rlp ;
	    void	*vp{} ;
	    if (rlp) {
	        for (int i = 0 ; vechand_get(rlp,i,&vp) >= 0 ; i += 1) {
	            if (vp) {
	                rec	*rp = recp(vp) ;
			{
	                    rs1 = record_finish(rp) ;
			    if (rs >= 0) rs = rs1 ;
			}
			{
	                    rs1 = libmem.free(rp) ;
			    if (rs >= 0) rs = rs1 ;
			}
	            }
	        } /* end for */
	    }
	    if (rlp) {
	        rs1 = vechand_finish(rlp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->flp) {
	        while (cq_rem(op->flp,&vp) >= 0) {
	            rs1 = libmem.free(vp) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end while */
	    }
	    if (op->flp) {
	        rs1 = cq_finish(op->flp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = grcache_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (grcache_finish) */

int grcache_lookname(grcache *op,GE *grp,char *grbuf,int grlen,cc *gn) noex {
	time_t		dt = time(nullptr) ;
	int		rs ;
	if ((rs = grcache_magic(op,grp,grbuf,gn)) >= 0) {
	    rs = SR_INVALID ;
	    if (gn[0]) {
		rec	*rp{} ;
	        int	ct ;
	        op->s.total += 1 ;
	        if ((rs = grcache_searchname(op,&rp,gn)) >= 0) {
	            ct = ct_hit ;
	            rs = grcache_recaccess(op,dt,rp) ;
	        } else if (rs == SR_NOTFOUND) {
	            ct = ct_miss ;
	            rs = grcache_mkrec(op,dt,&rp,gn) ;
	        } /* end if */
	        grcache_record(op,ct,rs) ;
	        if (rs > 0) {
	            if ((rs = grp->load(grbuf,grlen,&rp->gr)) >= 0) {
	                if_constexpr (f_maintextra) {
	                    if ((dt - op->ti_check) >= TO_CHECK) {
	                        op->ti_check = dt ;
	                        grcache_maintenance(op,dt) ;
	                    }
	                } /* end if_constexpr (f_maintextra) */
		    }
	        } else if (rs == 0) {
	            rs = SR_NOTFOUND ;
	        } /* end if */
	        if (rs <= 0) {
	            memclear(grp) ;
	        }
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (grcache_lookname) */

#ifdef	COMMENT
int grcache_lookgid(grcache *op,GE *grp,char *grbuf,int grlen,gid_t gid) noex {
	time_t		dt = time(nullptr) ;
	int		rs = SR_OK ;
	int		grl = 0 ;
	if ((rs = grcache_magic(op,grp,grbuf)) >= 0) {
	    rs = SR_INVALID ;
	    if (gid != gidend) {
	        rec	*rp{} ;
	        op->s.total += 1 ;
	        if ((rs = grcache_searchgid(op,&rp,gid)) >= 0) {
	            rs = grcache_recaccess(op,dt,rp) ;
	            grl = rs ;
	        } else if (rs == SR_NOTFOUND) {
	            char	groupname[GROUPNAMELEN + 1] ;
	            if ((rs = getgroupname(groupname,GROUPNAMELEN,gid)) >= 0) {
	                rs = grcache_newrec(op,dt,&rp,gid,groupname) ;
	                grl = rs ;
	            }
	        } /* end if */
	        if (rs > 0) {
	            if (grp) {
		        rs = grp->load(grbuf,grlen,&rp->gr) ;
	            }
	            if (rs >= 0) {
	                grcache_maintenance(op,dt) ;
	            }
	        } /* end if */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? grl : rs ;
}
/* end subroutine (grcache_lookgid) */
#endif /* COMMENT */

int grcache_getstats(grcache *op,grcache_st *sp) noex {
	int		rs ;
	if ((rs = grcache_magic(op,sp)) >= 0) {
	    if ((rs = vechand_count(op->rlp)) >= 0) {
	        *sp = op->s ;
	        sp->nentries = rs ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (grcache_getstats) */

int grcache_check(grcache *op,time_t dt) noex {
	int		rs ;
	int		f = false ;
	if ((rs = grcache_magic(op)) >= 0) {
	    if (dt == 0) dt = time(nullptr) ;
	    if ((dt - op->ti_check) >= TO_CHECK) {
	        f = true ;
	        op->ti_check = dt ;
	        rs = grcache_maintenance(op,dt) ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (grcache_check) */


/* private subroutines */

static int grcache_mkrec(grcache *op,time_t dt,rec **epp,cc *gn) noex {
	int		rs ;
	int		grl = 0 ;
	*epp = nullptr ;
	if ((rs = vechand_count(op->rlp)) >= 0) {
	    cint	n = rs ;
	    if (n >= op->nmax) {
	        rs = grcache_getrec(op,dt,epp) ;
	    } else {
	        rs = grcache_allocrec(op,epp) ;
	    } /* end if */
	    if (rs >= 0) {
	        rs = grcache_recstart(op,dt,*epp,gn) ;
		grl = rs ;
	    }
	} /* end if */
	return (rs >= 0) ? grl : rs ;
}
/* end subroutine (grcache_mkrec) */

static int grcache_recstart(grcache *op,time_t dt,rec *ep,cc *gn) noex {
	cint		wc = op->wcount++ ;
	int		rs ;
	int		grl = 0 ;
	if ((rs = record_start(ep,dt,wc,gn)) >= 0) {
	    grl = rs ;
	    rs = vechand_add(op->rlp,ep) ;
	    if (rs < 0) {
	        record_finish(ep) ;
	    }
	} /* end if (record-start) */
	return (rs >= 0) ? grl : rs ;
}
/* end subroutine (grcache_recstart) */

static int grcache_recaccess(grcache *op,time_t dt,rec *ep) noex {
	int		rs ;
	if ((rs = grcache_recrear(op,ep)) >= 0) {
	    if ((rs = record_old(ep,dt,op->ttl)) > 0) {
		int	wc = op->wcount++ ;
	        op->s.refreshes += 1 ;
	        rs = record_refresh(ep,dt,wc) ;
	    } else {
	        rs = record_access(ep,dt) ;
	    }
	} /* end if */
	return rs ;
}
/* end subroutine (grcache_recaccess) */

static int grcache_recrear(grcache *op,rec *ep) noex {
	int		rs = SR_FAULT ;
	if (op && ep) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (grcache_recrear) */

static int grcache_recdel(grcache *op,int ri,rec *ep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (ri >= 0) {
	    rs1 = vechand_del(op->rlp,ri) ;
	} else {
	    if ((rs1 = vechand_ent(op->rlp,ep)) >= 0) {
	        rs1 = vechand_del(op->rlp,rs1) ;
	    }
	}
	if (rs >= 0) rs = rs1 ;
	{
	    rs1 = record_finish(ep) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (grcache_recdel) */

static int grcache_searchname(grcache *op,rec **rpp,cchar *gn) noex {
	vechand		*rlp = op->rlp ;
	int		rs ;
	rec		*rp = nullptr ;
	void		*vp{} ;
	bool		f = false ;
	for (int i = 0 ; (rs = vechand_get(rlp,i,&vp)) >= 0 ; i += 1) {
	    if (vp) {
		rp = recp(vp) ;
	        if (rp->gn[0] == gn[0]) {
		    f = (strcmp(rp->gn,gn) == 0) ;
	        }
	    }
	    if (f) break ;
	} /* end for */
	if (rpp) {
	    *rpp = (f) ? rp : nullptr ;
	}
	return rs ;
}
/* end subroutine (grcache_searchname) */

#if	CF_SEARCHGID
static int grcache_searchgid(grcache *op,rec **rpp,gid_t gid) noex {
	vechand		*rlp = op->rlp ;
	int		rs ;
	rec		*rp = nullptr ;
	void		*vp{} ;
	bool		f = false ;
	for (int i = 0 ; (rs = vechand_get(op->rlp,i,vp)) >= 0 ; i += 1) {
	    rp = recp(vp) ;
	    if (vp) {
	        f = (rp->gid == gid) ;
	    }
	    if (f) break ;
	} /* end for */
	if (rpp) {
	    *rpp = (f) ? rp : nullptr ;
	}
	return rs ;
}
/* end subroutine (grcache_searchgid) */
#endif /* CF_SEARCHGID */

static int grcache_getrec(grcache *op,time_t dt,rec **rpp) noex {
	rec		*rp = nullptr ;
	vechand		*rlp = op->rlp ;
	time_t		ti_oldest = timemax ;
	int		rs = SR_OK ;
	int		iold = -1 ;
	int		f_exp = false ;
	void		*vp{} ;
	for (int i = 0 ; vechand_get(rlp,i,&vp) >= 0 ; i += 1) {
	    rp = recp(vp) ;
	    if (vp) {
	        if (record_old(rp,dt,op->ttl) > 0) {
	            grcache_recdel(op,i,rp) ;
	            grcache_recfree(op,rp) ;
		    f_exp = true ;
	        } else {
	            if (rp->ti_access < ti_oldest) {
	                ti_oldest = rp->ti_access ;
	                iold = i ;
	            }
	        }
	    }
	    if (rs < 0) break ;
	} /* end for */
	if (rs >= 0) {
	    if (f_exp || (iold < 0)) {
		rs = grcache_allocrec(op,&rp) ;
	    } else {
	        if ((rs = vechand_get(rlp,iold,&vp)) >= 0) {
		    rp = recp(vp) ;
		    vechand_del(rlp,iold) ;
		    record_finish(rp) ;
		}
	    }
	} /* end if (ok) */
	if (rpp) {
	    *rpp = (rs >= 0) ? rp : nullptr ;
	}
	return rs ;
}
/* end subroutine (grcache_getrec) */

static int grcache_maintenance(grcache *op,time_t dt) noex {
	rec		*rp = nullptr ;
	time_t		ti_oldest = timemax ;
	int		rs = SR_OK ;
	int		iold = -1 ;
/* delete expired entries */
	void		*vp{} ;
	for (int i = 0 ; vechand_get(op->rlp,i,&vp) >= 0 ; i += 1) {
	    rp = recp(vp) ;
	    if (vp) {
	        if ((dt - rp->ti_create) >= op->ttl) {
	            grcache_recdel(op,i,rp) ;
	            grcache_recfree(op,rp) ;
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
	    if ((rs = vechand_count(op->rlp)) > op->nmax) {
	        if ((rs = vechand_get(op->rlp,iold,&vp)) >= 0) {
		    rp = recp(vp) ;
	            if (rp) {
	                grcache_recdel(op,iold,rp) ;
	                grcache_recfree(op,rp) ;
		    } /* end if (have) */
	        } /* end if (get) */
	    } /* enbd if (count) */
	} /* end if (iold) */
	return rs ;
}
/* end subroutine (grcache_maintenance) */

static int grcache_allocrec(grcache *op,rec **rpp) noex {
	int		rs ;
	if ((rs = cq_rem(op->flp,rpp)) == SR_NOTFOUND) {
	    cint	sz = szof(rec) ;
	    if (void *vp ; (rs = libmem.mall(sz,&vp)) >= 0) {
	        *rpp = recp(vp) ;
	    } /* end if (memory-allocation) */
	} /* end if (cq_rem) */
	return rs ;
}
/* end subroutine (grcache_allocrec) */

static int grcache_recfree(grcache *op,rec *rp) noex {
	int		rs ;
	if ((rs = cq_count(op->flp)) >= 0) {
	    cint	n = rs ;
	    if (n < GRCACHE_MAXFREE) {
	        rs = cq_ins(op->flp,rp) ;
	    } else {
	        libmem.free(rp) ;
	    }
	}
	return rs ;
}
/* end subroutine (grcache_recfree) */

static int grcache_record(grcache *op,int ct,int rs) noex {
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
/* end subroutine (grcache_record) */

static int record_start(rec *rp,time_t dt,int wc,cchar *gn) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		grl = 0 ;
	if (rp && gn) {
	    rs = SR_INVALID ;
	    if (gn[0]) {
		cint	rsn = SR_NOTFOUND ;
	        if (dt == 0) dt = time(nullptr) ;
	        memclear(rp) ;
	        if (char *grbuf ; (rs = malloc_gr(&grbuf)) >= 0) {
	            cint	grlen = rs ;
	            if (ucentgr gr ; (rs = gr.getnam(grbuf,grlen,gn)) >= 0) {
	                cint	sz = (rs+1) ;
	                grl = rs ; /* indicates entry found */
	                if (void *vp ; (rs = libmem.mall(sz,&vp)) >= 0) {
			    ucentgr	*grp = &rp->gr ;
		            char	*nbuf = charp(vp) ;
		            if ((rs = grp->load(nbuf,grl,&gr)) >= 0) {
	                        rp->grbuf = nbuf ;
	    	                rp->grl = grl ;
		            }
		            if (rs < 0) {
				libmem.free(grbuf) ;
			    }
	                } /* end if (memory-allocation) */
	            } else if (rs == rsn) {
	                rp->grl = 0 ; /* optional */
	                grl = 0 ; /* indicates an empty (not-found) entry */
		    }
	            if (rs >= 0) {
			if (cchar *cp ; (rs = libmem.strw(gn,-1,&cp)) >= 0) {
			    rp->gn = cp ;
	                    rp->ti_create = dt ;
	                    rp->ti_access = dt ;
	                    rp->wcount = wc ;
	                    rp->magic = RECORD_MAGIC ;
			} /* end if (memory-allocation) */
	            } /* end if (ok) */
	            rs1 = libmem.free(grbuf) ; /* free first one up at top */
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? grl : rs ;
}
/* end subroutine (record_start) */

static int record_finish(rec *rp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (rp) {
	    rs = SR_NOTOPEN ;
	    if (rp->magic == RECORD_MAGIC) {
		rs = SR_OK ;
		if (rp->gn) {
		    void *vp = voidp(rp->gn) ;
	            rs1 = libmem.free(vp) ;
	            if (rs >= 0) rs = rs1 ;
	            rp->gn = nullptr ;
		}
	        if (rp->grbuf) {
	            rs1 = libmem.free(rp->grbuf) ;
	            if (rs >= 0) rs = rs1 ;
	            rp->grbuf = nullptr ;
	        }
	        rp->grl = 0 ;
	        rp->gid = -1 ;
	        rp->magic = 0 ;
	    } /* end if (was open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (record_finish) */

static int record_access(rec *ep,time_t dt) noex {
	int		rs = SR_FAULT ;
	int		grl ;
	if (ep) {
	    rs = SR_NOTFOUND ;
	    if (ep->magic == RECORD_MAGIC) {
		rs = SR_OK ;
	        ep->ti_access = dt ;
	        grl  = ep->grl ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? grl : rs ;
}
/* end subroutine (record_access) */

static int record_reload(rec *ep,int grl,ucentgr *ngrp) noex {
	int		rs = SR_OK ;
        void    	*vp{} ;
        if (ep->grbuf) {
            rs = libmem.rall(ep->grbuf,(grl + 1),&vp) ;
        } else {
            rs = libmem.mall((grl + 1),&vp) ;
        }
        if (rs >= 0) {
            char        *grbuf = charp(vp) ; /* new variable */
	    ucentgr	*grp = &ep->gr ;
            ep->grbuf = charp(vp) ;
            ep->grl = grl ;
            rs = grp->load(grbuf,grl,ngrp) ;
            if (rs < 0) {
                libmem.free(vp) ;
            }
        } /* end if (ok) */
	return rs ;
}
/* end subroutine (record_reload) */

static int record_refresh(rec *ep,time_t dt,int wc) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		grl = 0 ;
	if (ep) {
	    rs = SR_NOTFOUND ;
	    if (ep->magic == RECORD_MAGIC) {
	        if (char *grbuf ; (rs = malloc_gr(&grbuf)) >= 0) {
		    ucentgr	gr ;
		    cint	rsn = SR_NOTFOUND ;
	            cint	grlen = rs ;
	            if ((rs = getgr_name(&gr,grbuf,grlen,ep->gn)) >= 0) {
	                grl = rs ; /* <- indicates entry found */
			rs = record_reload(ep,grl,&gr) ;
	            } else if (rs == rsn) {
	                if (ep->grbuf) {
		            libmem.free(ep->grbuf) ;
		            ep->grbuf = nullptr ;
	                }
	                ep->grl = 0 ; /* signal whatever? */
	                grl = 0 ; /* indicates an empty (not-found) entry */
		    } /* end if */
	            if (rs >= 0) {
	                ep->ti_create = dt ;
	                ep->ti_access = dt ;
	                ep->wcount = wc ;
	            } /* end if (ok) */
	            rs1 = libmem.free(grbuf) ; /* free first one up top */
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? grl : rs ;
}
/* end subroutine (record_refresh) */

static int record_old(rec *ep,time_t dt,int ttl) noex {
	int		rs = SR_FAULT ;
	int		f_old = false ;
	if (ep) {
	    rs = SR_NOTFOUND ;
	    if (ep->magic == RECORD_MAGIC) {
		rs = SR_OK ;
		f_old = ((dt - ep->ti_create) >= ttl) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? f_old : rs ;
}
/* end subroutine (record_old) */


