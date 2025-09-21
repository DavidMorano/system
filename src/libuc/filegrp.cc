/* filegrp SUPPORT */
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

  	Object:
	filegrp

	Description:
	This object provides a crude cache for GROUP-DB entries.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* system types */
#include	<ctime>			/* |time_t| */
#include	<cstddef>		/* |nullptr_t(3c++)| */
#include	<cstdlib>
#include	<usystem.h>
#include	<getgroupname.h>
#include	<bufsizevar.hh>
#include	<vechand.h>
#include	<cq.h>
#include	<snx.h>
#include	<strwcpy.h>
#include	<libmallocxx.h>
#include	<localmisc.h>

#include	"filegrp.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| + |getlenstr(3u)| */

/* local defines */

#define	FG		filegrp
#define	FG_ST		filegrp_st
#define	FG_REC		filegrp_rec

#define	TO_CHECK	5


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

struct filegrp_rec {
	time_t		ti_create ;		/* creation time */
	time_t		ti_access ;		/* last access time */
	char		*gn ;			/* <- allocated */
	gid_t		gid ;
} ;


/* forward references */

template<typename ... Args>
static inline int filegrp_ctor(filegrp *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_NOMEM ;
	    op->ti_check = 0 ;
	    op->magic = 0 ;
	    op->ttl = 0 ;
	    op->nmax = 0 ;
	    op->s = {} ;
 	    if ((op->flp = new(nothrow) cq) != nullptr) {
 	        if ((op->alp = new(nothrow) vechand) != nullptr) {
		    rs = SR_OK ;
	        } /* end if (new-vechand) */
		if (rs < 0) {
		    delete op->flp ;
		    op->flp = nullptr ;
		}
	    } /* end if (new-cq) */
	} /* end if (non-null) */
	return rs ;
}

static inline int filegrp_dtor(filegrp *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->alp) {
		delete op->alp ;
		op->alp = nullptr ;
	    }
	    if (op->flp) {
		delete op->flp ;
		op->flp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (filegrp_dtor) */

static int filegrp_searchgid(FG *,FG_REC **,gid_t) noex ;
static int filegrp_newrec(FG *,time_t,FG_REC **,gid_t,cc *) noex ;
static int filegrp_recaccess(FG *,FG_REC *,time_t) noex ;
static int filegrp_allocrec(FG *,FG_REC **) noex ;
static int filegrp_recfree(FG *,FG_REC *) noex ;
static int filegrp_maintenance(FG *,time_t) noex ;
static int filegrp_record(FG *,int,int) noex ;

template<typename ... Args>
static inline int filegrp_magic(filegrp *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == FILEGRP_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}

static int record_start(FG_REC *,time_t,gid_t,cc *) noex ;
static int record_old(FG_REC *,time_t,int) noex ;
static int record_refresh(FG_REC *,time_t) noex ;
static int record_update(FG_REC *,time_t,cc *) noex ;
static int record_access(FG_REC *,time_t) noex ;
static int record_finish(FG_REC *) noex ;


/* local variables */

static constexpr gid_t	gidend = gid_t(-1) ;

static bufsizevar	gnl(getbufsize_gn) ;


/* exported variables */


/* exported subroutines */

int filegrp_start(FG *op,int nmax,int ttl) noex {
	cint		defnum = FILEGRP_DEFENTS ;
	int		rs ;
	if ((rs = filegrp_ctor(op)) >= 0) {
	    if (nmax < 4) nmax = FILEGRP_DEFMAX ;
	    if (ttl < 1) ttl = FILEGRP_DEFTTL ;
	    if ((rs = gnl) >= 0) {
	        if ((rs = cq_start(op->flp)) >= 0) {
	            if ((rs = vechand_start(op->alp,defnum,0)) >= 0) {
		        op->nmax = nmax ;
		        op->ttl = ttl ;
		        op->ti_check = time(nullptr) ;
		        op->magic = FILEGRP_MAGIC ;
	            }
	            if (rs < 0) {
		        cq_finish(op->flp) ;
		    }
	        } /* end if (cq_start) */
	    } /* end if (gnl) */
	    if (rs < 0) {
		filegrp_dtor(op) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (filegrp_start) */

int filegrp_finish(FG *op) noex {
	int		rs ;
	int		rs1{} ;		/* <- because possible compiler bug */
	if ((rs = filegrp_magic(op)) >= 0) {
            {
                vechand     *rlp = op->alp ;
                void        *vp{} ;
                for (int i = 0 ; vechand_get(rlp,i,&vp) >= 0 ; i += 1) {
                    if (vp) {
                        FG_REC *rp = (FG_REC *) vp ;
                        {
                            rs1 = record_finish(rp) ;
                            if (rs >= 0) rs = rs1 ;
                        }
                        {
                            rs1 = libmem.free(rp) ;
                            if (rs >= 0) rs = rs1 ;
                        }
                    }
                } /* end while */
                if (rs >= 0) rs = rs1 ;
            }
            if (op->alp) {
                rs1 = vechand_finish(op->alp) ;
                if (rs >= 0) rs = rs1 ;     /* <- compiler bug here */
            }
            {
                void        *vp{} ;
                while (cq_rem(op->flp,&vp) >= 0) {
                    rs1 = libmem.free(vp) ;
                    if (rs >= 0) rs = rs1 ;
                }
            }
	    if (op->flp) {
	        rs1 = cq_finish(op->flp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = filegrp_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (filegrp_finish) */

int filegrp_add(FG *op,gid_t gid,cc *gn) noex {
	custime		dt = time(nullptr) ;
	int		rs ;
	int		gl = 0 ;
	if ((rs = filegrp_magic(op,gn)) >= 0) {
		rs = SR_INVALID ;
	        if ((gid != gidend) && gn[0]) {
	            FG_REC		*rp{} ;
	            if ((rs = filegrp_searchgid(op,&rp,gid)) >= 0) {
	                gl = rs ;
	                rs = record_update(rp,dt,gn) ;
	            } else if (rs == SR_NOTFOUND) {
	                rs = filegrp_newrec(op,dt,nullptr,gid,gn) ;
	                gl = rs ;
	            }
	        } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? gl : rs ;
}
/* end subroutine (filegrp_add) */

int filegrp_lookgid(FG *op,char *rbuf,int rlen,gid_t gid) noex {
	custime		dt = time(nullptr) ;
	int		rs ;
	int		gl = 0 ;
	if ((rs = filegrp_magic(op)) >= 0) {
		rs = SR_INVALID ;
	        if (gid != gidend) {
	            FG_REC	*rp{} ;
	            int		ct{} ;
	            if ((rs = filegrp_searchgid(op,&rp,gid)) >= 0) {
	                ct = ct_hit ;
	                rs = filegrp_recaccess(op,rp,dt) ;
	                gl = rs ;
	            } else if (rs == SR_NOTFOUND) {
	                char	gn[gnl + 1] ;
	                ct = ct_miss ;
	                if ((rs = getgroupname(gn,gnl,gid)) >= 0) {
	                    rs = filegrp_newrec(op,dt,&rp,gid,gn) ;
	                    gl = rs ;
	                }
	            } /* end if (search-gid) */
	            filegrp_record(op,ct,rs) ;
	            if (rs == 0) rs = SR_NOTFOUND ;
	            if (rs >= 0) {
	                if (rbuf) {
			    rs = sncpy(rbuf,rlen,rp->gn) ;
			}
	                filegrp_maintenance(op,dt) ;
	            } /* end if */
	        } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? gl : rs ;
}
/* end subroutine (filegrp_lookgid) */

int filegrp_stats(FG *op,FG_ST *sp) noex {
	int		rs ;
	if ((rs = filegrp_magic(op,sp)) >= 0) {
	    if ((rs = vechand_count(op->alp)) >= 0) {
	        *sp = op->s ;
	        sp->nentries = rs ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (filegrp_stats) */

int filegrp_check(FG *op,time_t dt) noex {
	int		rs ;
	int		f = false ;
	if ((rs = filegrp_magic(op)) >= 0) {
	    if (dt == 0) dt = time(nullptr) ;
	    if ((dt - op->ti_check) >= TO_CHECK) {
	        f = true ;
	        op->ti_check = dt ;
	        rs = filegrp_maintenance(op,dt) ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (filegrp_check) */


/* private subroutines */

static int filegrp_newrec(FG *op,time_t dt,FG_REC **rpp,gid_t gid,cc *gn) noex {
	FG_REC		*rp{} ;
	int		rs ;
	int		gl = 0 ;
	if ((rs = filegrp_allocrec(op,&rp)) >= 0) {
	    if ((rs = record_start(rp,dt,gid,gn)) >= 0) {
		gl = rs ;
	        rs = vechand_add(op->alp,rp) ;
		if (rs < 0) {
		    record_finish(rp) ;
		}
	    } /* end if (record-start) */
	    if (rs < 0) {
		libmem.free(rp) ;
	    }
	} /* end if */
	if (rpp) *rpp = (rs >= 0) ? rp : nullptr ;
	return (rs >= 0) ? gl : rs ;
}
/* end subroutine (filegrp_newrec) */

static int filegrp_recaccess(FG *op,FG_REC *rp,time_t dt) noex {
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
/* end subroutine (filegrp_recaccess) */

static int filegrp_searchgid(FG *op,FG_REC **rpp,gid_t gid) noex {
	FG_REC		*rp = nullptr ;
	vechand		*rlp = op->alp ;
	int		rs = SR_OK ;
	int		gl = 0 ;
	void		*vp{} ;
	if (rpp) *rpp = nullptr ;
	for (int i = 0 ; (rs = vechand_get(rlp,i,&vp)) >= 0 ; i += 1) {
	    if (vp) {
		rp = (FG_REC *) vp ;
	        if (rp->gid == gid) break ;
	    }
	} /* end for */
	if ((rs >= 0) && rp) {
	   *rpp = rp ;
	    gl = lenstr(rp->gn) ;
	}
	return (rs >= 0) ? gl : rs ;
}
/* end subroutine (filegrp_searchgid) */

static int filegrp_maintenance(FG *op,time_t dt) noex {
	vechand		*rlp = op->alp ;
	FG_REC		*rp{} ;
	time_t		ti_oldest = 0 ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		iold = -1 ; /* the oldest one */
	void		*vp{} ;
	for (int i = 0 ; vechand_get(rlp,i,&vp) >= 0 ; i += 1) {
	    if (vp) {
	        if ((dt - rp->ti_create) >= op->ttl) {
		    rp = (FG_REC *) vp ;
	            vechand_del(rlp,i) ;
		    record_finish(rp) ;
	            filegrp_recfree(op,rp) ;
	        } else {
	            if (rp->ti_access < ti_oldest) {
	                ti_oldest = rp->ti_access ;
	                iold = i ;
	            }
	        }
	    }
	} /* end for */
	if ((rs >= 0) && (iold >= 0)) {
	    cint	n = vechand_count(op->alp) ;
	    if (n > op->nmax) {
	        rs1 = vechand_get(op->alp,iold,&vp) ;
	        if ((rs1 >= 0) && rp) {
		    rp = (FG_REC *) vp ;
	            vechand_del(rlp,iold) ;
		    record_finish(rp) ;
	            filegrp_recfree(op,rp) ;
	        }
	    }
	} /* end if */
	return rs ;
}
/* end subroutine (filegrp_maintenance) */

static int filegrp_allocrec(FG *op,FG_REC **rpp) noex {
	cint		sz = szof(FG_REC) ;
	int		rs ;
	if ((rs = cq_rem(op->flp,rpp)) == SR_NOTFOUND) {
	    if (void *vp ; (rs = libmem.mall(sz,&vp)) >= 0) {
	        *rpp = (FG_REC *) vp ;
	    }
	}
	return rs ;
}
/* end subroutine (filegrp_allocrec) */

static int filegrp_recfree(FG *op,FG_REC *rp) noex {
	int		rs = SR_FAULT ;
	int		n = 0 ;
	if (op && rp) {
	    cq		*fqp = op->flp ;
	    if ((rs = cq_count(fqp)) >= 0) {
	        n = rs ;
	        if (n < FILEGRP_MAXFREE) {
	            rs = cq_ins(fqp,rp) ;
	            if (rs < 0) {
		        libmem.free(rp) ;
		    }
	        } else {
	            libmem.free(rp) ;
	        }
	    } /* end if (cq_count) */
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (filegrp_recfree) */

static int filegrp_record(FG *op,int ct,int rs) noex {
	cbool		f_got = (rs > 0) ;
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
/* end subroutine (filegrp_record) */

static int record_start(FG_REC *rp,time_t dt,gid_t gid,cc *gn) noex {
	int		rs = SR_FAULT ;
	int		gl = 0 ;
	if (rp && gn) {
	    rs = SR_INVALID ;
	    memclear(rp) ;
	    if (dt == 0) dt = time(nullptr) ;
	    if (gn[0]) {
		if (char *gp ; (rs = libmalloc_gn(&gp)) >= 0) {
		    rp->gn = gp ;
	            rp->gid = gid ;
	            rp->ti_create = dt ;
	            rp->ti_access = dt ;
	            gl = intconv(strwcpy(rp->gn,gn,gnl) - rp->gn) ;
		} /* end if (m-a) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? gl : rs ;
}
/* end subroutine (record_start) */

static int record_finish(FG_REC *rp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (rp) {
	    rs = SR_OK ;
	    rp->gid = -1 ;
	    rp->gn[0] = '\0' ;
	    {
	        rs1 = libmem.free(rp->gn) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (record_finish) */

static int record_old(FG_REC *rp,time_t dt,int ttl) noex {
	int		rs = SR_FAULT ;
	if (rp) {
	    rs = ((dt - rp->ti_create) >= ttl) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (record_old) */

static int record_refresh(FG_REC *rp,time_t dt) noex {
	int		rs ;
	int		gl = 0 ;
	char		gn[gnl + 1] ;
	if ((rs = getgroupname(gn,gnl,rp->gid)) >= 0) {
	    gl = rs ;
	    rs = record_update(rp,dt,gn) ;
	}
	return (rs >= 0) ? gl : rs ;
}
/* end subroutine (record_refresh) */

static int record_update(FG_REC *rp,time_t dt,cc *gn) noex {
	int		rs = SR_FAULT ;
	int		f_changed = false ;
	if (rp && gn) {
	    rs = SR_OK ;
	    rp->ti_create = dt ;
	    rp->ti_access = dt ;
	    f_changed = (strcmp(rp->gn,gn) != 0) ;
	    if (f_changed) {
	        strwcpy(rp->gn,gn,gnl) ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? f_changed : rs ;
}
/* end subroutine (record_update) */

static int record_access(FG_REC *rp,time_t dt) noex {
    	int		rs = SR_FAULT ;
	int		gl ; /* return-value */
	if ((gl = lenstr(rp->gn)) >= 0) {
	    rs = SR_OK ;
	    rp->ti_access = dt ;
	} /* end if (getlenstr) */
	return (rs >= 0) ? gl : rs ;
}
/* end subroutine (record_access) */


