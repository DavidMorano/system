/* pwcache SUPPORT */
/* lang=C++20 */

/* PASSWD cache */
/* version %I% last-modified %G% */


/* revision history:

        = 2004-01-10, David A­D­ Morano
        This code was originally written.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

        This object provides a crude cache for PASSWD-DB entries.

*******************************************************************************/

#include        <envstandards.h>	/* place first to configure */
#include        <sys/types.h>
#include        <sys/param.h>
#include        <cstdlib>
#include        <cstring>		/* <- for |strlen(3c)| */
#include        <ctime>
#include        <usystem.h>
#include        <usupport.h>
#include        <getbufsize.h>
#include        <ucentpw.h>
#include        <getax.h>
#include        <strwcpy.h>
#include        <localmisc.h>

#include        "pwcache.h"


/* local defines */

#ifndef PWCACHE_REC
#define PWCACHE_REC     struct pwcache_r
#endif

#define REC             PWCACHE_REC
#define PWE             PASSWD


/* local namespaces */

using std::nothrow ;			/* type */


/* local typedefs */

typedef ucentpw *	pwentp ;


/* external subroutines */


/* local structures */

enum cts {
        ct_miss,
        ct_hit,
        ct_overlast
} ;

struct pwcache_r {
        pq_ent          linkage ;
        char            *pwbuf ;
        PASSWD          pw ;
        time_t          ti_create ;
        time_t          ti_access ;
        uint            wcount ;
        int             pwl ;
        char            un[USERNAMELEN+1] ;
} ;


/* forward references */

template<typename ... Args>
static inline int pwcache_ctor(pwcache *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_NOMEM ;
	    op->magic = 0 ;
	    op->s = {} ;
	    op->ti_check = 0 ;
	    op->wcount = 0 ;
	    op->ttl = 0 ;
	    op->max = 0 ;
	    if ((op->dbp = new(nothrow) hdb) != nullptr) {
	        if ((op->lrup = new(nothrow) pq) != nullptr) {
		    rs = SR_OK ;
		} /* end if (new-qq) */
	        if (rs < 0) {
		    delete op->dbp ;
		    op->dbp = nullptr ;
		}
	    } /* end if (new-hdb) */
	} /* end if (non-null) */
	return rs ;
}
/* end method (pwcache_ctor) */

static inline int pwcache_dtor(pwcache *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->lrup) {
		delete op->lrup ;
		op->lrup = nullptr ;
	    }
	    if (op->dbp) {
		delete op->dbp ;
		op->dbp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end method (pwcache_dtor) */

template<typename ... Args>
static int pwcache_magic(pwcache *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == PWCACHE_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end method (pwcache_magic) */

static int      pwcache_mkrec(pwcache *,time_t,REC **,cchar *) noex ;
static int      pwcache_newrec(pwcache *,time_t,REC **,cchar *) noex ;
static int      pwcache_recstart(pwcache *,time_t,REC *,cchar *) noex ;
static int      pwcache_recdel(pwcache *,REC *) noex ;
static int      pwcache_recaccess(pwcache *,time_t,REC *) noex ;
static int      pwcache_recrear(pwcache *,REC *) noex ;
static int      pwcache_recfins(pwcache *) noex ;
static int      pwcache_record(pwcache *,int,int) noex ;

static int record_start(REC *,time_t,int,cchar *) noex ;
static int record_access(REC *,time_t) noex ;
static int record_refresh(REC *,time_t,int) noex ;
static int record_old(REC *,time_t,int) noex ;
static int record_finish(REC *) noex ;


/* local variables */


/* exported subroutines */

int pwcache_start(pwcache *op,int max,int ttl) noex {
        int             rs  ;
	if ((rs = pwcache_ctor(op)) >= 0) {
            if (max < PWCACHE_DEFMAX) max = PWCACHE_DEFMAX ;
            if (ttl < PWCACHE_DEFTTL) ttl = PWCACHE_DEFTTL ;
            if ((rs = hdb_start(op->dbp,max,1,nullptr,nullptr)) >= 0) {
                if ((rs = pq_start(op->lrup)) >= 0) {
                    op->max = max ;
                    op->ttl = ttl ;
                    op->ti_check = time(nullptr) ;
                    op->magic = PWCACHE_MAGIC ;
                }
                if (rs < 0) {
                    hdb_finish(op->dbp) ;
		}
            } /* end if (hdb-start) */
	    if (rs < 0) {
		pwcache_dtor(op) ;
	    }
        } /* end if (non-null) */
        return rs ;
}
/* end subroutine (pwcache_start) */

int pwcache_finish(pwcache *op) noex {
        int             rs ;
        int             rs1 ;
	if ((rs = pwcache_magic(op)) >= 0) {
            {
                rs1 = pq_finish(op->lrup) ; /* finish up the LRU queue */
                if (rs >= 0) rs = rs1 ;
            }
            {
                rs1 = pwcache_recfins(op) ; /* freeing all cache entries */
                if (rs >= 0) rs = rs1 ;
            }
            {
                rs1 = hdb_finish(op->dbp) ; /* free up everything else */
                if (rs >= 0) rs = rs1 ;
            }
	    {
		rs1 = pwcache_dtor(op) ;
                if (rs >= 0) rs = rs1 ;
	    }
            op->magic = 0 ;
	} /* end if (magic) */
        return rs ;
}
/* end subroutine (pwcache_finish) */

int pwcache_lookup(pwcache *op,PWE *pwp,char *pwbuf,int pwlen,cchar *un) noex {
        const time_t    dt = time(nullptr) ;
        int             rs ;
	if ((rs = pwcache_magic(op,pwp,pwbuf,un)) >= 0) {
                rs = SR_INVALID ;
                if (un[0]) {
                    hdb_datum	key{}, val{} ;
       		    REC		*ep = nullptr ;
        	    int		ct = 0 ;
                    op->s.total += 1 ;
                    key.buf = un ;
                    key.len = strlen(un) ;
                    if ((rs = hdb_fetch(op->dbp,key,nullptr,&val)) >= 0) {
        		ep = (REC *) val.buf ;
                        ct = ct_hit ;
                        rs = pwcache_recaccess(op,dt,ep) ;
                    } else if (rs == SR_NOTFOUND) {
                        ct = ct_miss ;
                        rs = pwcache_mkrec(op,dt,&ep,un) ;
                    } /* end if (hit or miss) */
                    pwcache_record(op,ct,rs) ;
                    if (rs > 0) { /* not '>=' */
			ucentpw		*entp = pwentp(pwp) ;
			ucentpw		*opwp = pwentp(&ep->pw) ;
                        rs = entp->load(pwbuf,pwlen,opwp) ;
                    } else if (rs == 0) {
                        rs = SR_NOTFOUND ;
                    }
                    if (rs <= 0) {
                        memclear(pwp,sizeof(PASSWD)) ;
                    }
                } /* end if (valid) */
	} /* end if (magic) */
        return rs ;
}
/* end subroutine (pwcache_lookup) */

int pwcache_invalidate(pwcache *op,cchar *un) noex {
        int             rs ;
        int             rs1 ;
        int             f_found = false ;
	if ((rs = pwcache_magic(op,un)) >= 0) {
                rs = SR_INVALID ;
                if (un[0]) {
                    hdb_datum   key{}, val{} ;
                    key.buf = un ;
                    key.len = strlen(un) ;
                    if ((rs = hdb_fetch(op->dbp,key,nullptr,&val)) >= 0) {
        		REC    	*ep = (REC *) val.buf ;
                        f_found = true ;
                        {
                            pq_ent      *pep = (pq_ent *) ep ;
                            rs1 = pq_unlink(op->lrup,pep) ;
                            if (rs >= 0) rs = rs1 ;
                        }
                        {
                            rs1 = hdb_delkey(op->dbp,key) ;
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
                } /* end if (valid) */
	} /* end if (magic) */
        return (rs >= 0) ? f_found : rs ;
}
/* end subroutine (pwcache_invalidate) */

int pwcache_check(pwcache *op,time_t dt) noex {
        int             rs ;
        int             rs1 ;
        int             f = false ;
	if ((rs = pwcache_magic(op)) >= 0) {
                hdb_cur         cur{} ;
                hdb_datum       key{}, val{} ;
                if ((rs = hdb_curbegin(op->dbp,&cur)) >= 0) {
                    if (dt == 0) dt = time(nullptr) ;
                    while (rs >= 0) {
                        rs1 = hdb_enum(op->dbp,&cur,&key,&val) ;
                        if (rs1 == SR_NOTFOUND) break ;
                        rs = rs1 ;
                        if (rs >= 0) {
        	            REC		*ep = (REC *) val.buf ;
                            if ((rs = record_old(ep,dt,op->ttl)) > 0) {
                                f = true ;
                                if ((rs = pwcache_recdel(op,ep)) >= 0) {
                                    pq_ent      *pep = (pq_ent *) ep ;
                                    rs = pq_unlink(op->lrup,pep) ;
                                    uc_free(ep) ;
                                }
                            } /* end if (entry-old) */
                        }
                    } /* end while */
                    rs1 = hdb_curend(op->dbp,&cur) ;
	            if (rs >= 0) rs = rs1 ;
                } /* end if */
	} /* end if (magic) */
        return (rs >= 0) ? f : rs ;
}
/* end subroutine (pwcache_check) */

int pwcache_getstats(pwcache *op,pwcache_stats *sp) noex {
        int             rs ;
	if ((rs = pwcache_magic(op,sp)) >= 0) {
                if ((rs = hdb_count(op->dbp)) >= 0) {
                    *sp = op->s ;
                    sp->nentries = rs ;
                }
	} /* end if (magic) */
        return rs ;
}
/* end subroutine (pwcache_getstats) */


/* private subroutines */

static int pwcache_mkrec(pwcache *op,time_t dt,REC **epp,cchar *un) noex {
        pq_ent          *pep ;
        int             rs ;
        int             rs1 ;
        int             pwl = 0 ;
        *epp = nullptr ;
        if ((rs = hdb_count(op->dbp)) >= 0) {
            cint		n = rs ;
            if (n >= op->max) {
                if ((rs = pq_rem(op->lrup,&pep)) >= 0) {
                    REC		*ep = (REC *) pep ;
                    if ((rs = pwcache_recdel(op,ep)) >= 0) {
                        rs = pwcache_recstart(op,dt,ep,un) ;
                        pwl = rs ;
                    }
                    rs1 = pq_ins(op->lrup,pep) ;
                    if (rs >= 0) rs = rs1 ;
                } /* end if (removed entry) */
                if (rs >= 0) *epp = (REC *) pep ;
            } else {
                if ((rs = pwcache_newrec(op,dt,epp,un)) >= 0) {
                    pwl = rs ;
                    if (*epp != nullptr) {
                        pep = (pq_ent *) *epp ;
                        rs = pq_ins(op->lrup,pep) ;
                    }
                } /* end if (new-entry) */
            } /* end if */
        } /* end if */
        return (rs >= 0) ? pwl : rs ;
}
/* end subroutine (pwcache_mkrec) */

static int pwcache_newrec(pwcache *op,time_t dt,REC **epp,cchar *un) noex {
        REC		*ep{} ;
        int             rs = SR_BUGCHECK ;
        if (epp) {
            cint	size = sizeof(REC) ;
            if ((rs = uc_malloc(size,&ep)) >= 0) {
                rs = pwcache_recstart(op,dt,ep,un) ;
                if (rs < 0) uc_free(ep) ;
            }
	} /* end if (non-null) */
        *epp = (rs >= 0) ? ep : nullptr ;
        return rs ;
}
/* end subroutine (pwcache_newrec) */

static int pwcache_recstart(pwcache *op,time_t dt,REC *ep,cchar *un) noex {
        cint		wc = op->wcount++ ;
        int             rs ;
        int             pwl = 0 ;
        if ((rs = record_start(ep,dt,wc,un)) >= 0) {
            hdb_datum   key{}, val{} ;
            cint   	size = sizeof(REC) ;
            pwl = rs ;
            key.buf = ep->un ;
            key.len = strlen(ep->un) ;
            val.buf = ep ;
            val.len = size ;
            rs = hdb_store(op->dbp,key,val) ;
            if (rs < 0)
                record_finish(ep) ;
        } /* end if (entry-start) */
        return (rs >= 0) ? pwl : rs ;
}
/* end subroutine (pwcache_recstart) */

static int pwcache_recaccess(pwcache *op,time_t dt,REC *ep) noex {
        int             rs ;
        if ((rs = pwcache_recrear(op,ep)) >= 0) {
            if ((rs = record_old(ep,dt,op->ttl)) > 0) {
                int     wc = op->wcount++ ;
                op->s.refreshes += 1 ;
                rs = record_refresh(ep,dt,wc) ;
            } else {
                rs = record_access(ep,dt) ;
            }
        } /* end if */
        return rs ;
}
/* end subroutine (pwcache_recaccess) */

static int pwcache_recrear(pwcache *op,REC *ep) noex {
        pq_ent          *pcp = (pq_ent *) ep ;
        pq_ent          *pep{} ;
        int             rs ;
        if ((rs = pq_gettail(op->lrup,&pep)) >= 0) {
            if (pcp != pep) {
                pep = (pq_ent *) ep ;
                if ((rs = pq_unlink(op->lrup,pep)) >= 0) {
                    rs = pq_ins(op->lrup,pep) ;
                    if (rs < 0) {
                        REC     *ep = (REC *) pep ;
                        record_finish(ep) ;
                        uc_free(pep) ;
                    }
                }
            }
        } /* end if (pq-gettail) */
        return rs ;
}
/* end subroutine (pwcache_recrear) */

static int pwcache_recdel(pwcache *op,REC *ep) noex {
        int             rs = SR_OK ;
        int             rs1 ;
	{
            hdb_datum	key{} ;
            key.buf = ep->un ;
            key.len = strlen(ep->un) ;
            rs1 = hdb_delkey(op->dbp,key) ;
            if (rs >= 0) rs = rs1 ;
	}
	{
            rs1 = record_finish(ep) ;
            if (rs >= 0) rs = rs1 ;
	}
        return rs ;
}
/* end subroutine (pwcache_recdel) */

static int pwcache_recfins(pwcache *op) noex {
        hdb_cur         cur ;
        hdb_datum       key, val ;
        REC             *ep ;
        int             rs = SR_OK ;
        int             rs1 ;
        if ((rs1 = hdb_curbegin(op->dbp,&cur)) >= 0) {
            while (hdb_enum(op->dbp,&cur,&key,&val) >= 0) {
                ep = (REC *) val.buf ;
                {
                    rs1 = record_finish(ep) ;
                    if (rs >= 0) rs = rs1 ;
                }
                {
                    rs1 = uc_free(ep) ;
                    if (rs >= 0) rs = rs1 ;
                }
            } /* end while */
            hdb_curend(op->dbp,&cur) ;
        } /* end if */
        if (rs >= 0) rs = rs1 ;
        return rs ;
}
/* end subroutine (pwcache_recfins) */

static int pwcache_record(pwcache *op,int ct,int rs) noex {
	if (rs >= 0) {
            bool	f_got = (rs > 0) ;
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
	}
        return SR_OK ;
}
/* end subroutine (pwcache_record) */

static int record_start(REC *ep,time_t dt,int wc,cchar *un) noex {
        int             rs = SR_FAULT ;
        int             rs1 ;
        int             pwl = 0 ;
        if (ep && un) {
	    rs = SR_INVALID ;
            memclear(ep,sizeof(REC)) ;
            if (un[0]) {
                strwcpy(ep->un,un,USERNAMELEN) ;
                if ((rs = getbufsize(getbufsize_pw)) >= 0) {
                    PASSWD      pw{} ;
                    cint        pwlen = rs ;
                    char        *pwbuf = nullptr ;
                    if ((rs = uc_malloc((pwlen+1),&pwbuf)) >= 0) {
                        if ((rs1 = getpw_name(&pw,pwbuf,pwlen,un)) >= 0) {
                            void	*vp{} ;
                            pwl = rs1 ;
                            if ((rs = uc_malloc((pwl+1),&vp)) >= 0) {
				ucentpw	*pwp = pwentp(&ep->pw) ;
				ucentpw	*opwp = pwentp(&pw) ;
                                char    *pwbuf = (char *) vp ;
                                if ((rs = pwp->load(pwbuf,pwl,opwp)) >= 0) {
                                    ep->pwbuf = pwbuf ;
                                    ep->pwl = pwl ;
                                }
                                if (rs < 0) uc_free(vp) ;
                            } /* end if (memory-allocation) */
                        } else if (rs1 == SR_NOTFOUND) {
                            ep->pwl = 0 ; /* optional */
                            pwl = 0 ; /* indicates an empty (not-found) entry */
                        } else {
                            rs = rs1 ;
                        }
                        rs1 = uc_free(pwbuf) ;
                        if (rs >= 0) rs = rs1 ;
                    } /* end if (m-a) */
                } /* end if (getbufsize) */
                if (rs >= 0) {
                    ep->ti_create = dt ;
                    ep->ti_access = dt ;
                    ep->wcount = wc ;
                }
	    } /* end if (valid) */
	} /* end if (non-null) */
        return (rs >= 0) ? pwl : rs ;
}
/* end subroutine (record_start) */

static int record_finish(REC *ep) noex {
        int             rs = SR_FAULT ;
        int             rs1 ;
        if (ep) {
	    rs = SR_OK ;
            if (ep->pwbuf) {
                rs1 = uc_free(ep->pwbuf) ;
                if (rs >= 0) rs = rs1 ;
                ep->pwbuf = nullptr ;
            }
            ep->pwl = 0 ;
            ep->un[0] = '\0' ;
	} /* end if (non-null) */
        return rs ;
}
/* end subroutine (record_finish) */

static int record_access(REC *ep,time_t dt) noex {
        int             rs = SR_FAULT ;
        int             pwl = 0 ;
        if (ep) {
            ep->ti_access = dt ;
            pwl  = ep->pwl ;
	} /* end if (non-null) */
        return (rs >= 0) ? pwl : rs ;
}
/* end subroutine (record_access) */

static int record_refresh(REC *ep,time_t dt,int wc) noex {
        int             rs = SR_FAULT ;
        int             rs1 ;
        int             pwl = 0 ;
        if (ep) {
            if ((rs = getbufsize(getbufsize_pw)) >= 0) {
                PASSWD 	pw ;
                cint	pwlen = rs ;
                char	*pwbuf{} ;
                if ((rs = uc_malloc((pwlen+1),&pwbuf)) >= 0) {
                    if ((rs1 = getpw_name(&pw,pwbuf,pwlen,ep->un)) >= 0) {
                        void	*vp{} ;
                        pwl = rs1 ;
                        ep->pwl = pwl ;
                        if (ep->pwbuf) {
                            rs = uc_realloc(ep->pwbuf,(pwl+1),&vp) ;
                        } else {
                            rs = uc_malloc((pwl+1),&vp) ;
                        }
                        if (rs >= 0) {
			    ucentpw	*pwp = pwentp(&ep->pw) ;
			    ucentpw	*opwp = pwentp(&pw) ;
                            char	*pwbuf = (char *) vp ;
                            ep->pwbuf = (char *) vp ;
                            rs = pwp->load(pwbuf,pwl,opwp) ;
                            if (rs < 0) uc_free(vp) ;
                        }
                    } else if (rs1 == SR_NOTFOUND) {
                        if (ep->pwbuf != nullptr) {
                            uc_free(ep->pwbuf) ;
                            ep->pwbuf = nullptr ;
                        }
                        ep->pwl = 0 ;
                        pwl = 0 ; /* indicates an empty (not-found) entry */
                    } else {
                        rs = rs1 ;
                    }
                    rs1 = uc_free(pwbuf) ;
                    if (rs >= 0) rs = rs1 ;
                } /* end if (m-a) */
            } /* end if (getbufsize) */
            if (rs >= 0) {
                ep->ti_create = dt ;
                ep->ti_access = dt ;
                ep->wcount = wc ;
            }
	} /* end if (non-null) */
        return (rs >= 0) ? pwl : rs ;
}
/* end subroutine (record_refresh) */

static int record_old(REC *ep,time_t dt,int ttl) noex {
	int		rs = SR_FAULT ;
        int             f_old = false ;
        if (ep) {
            f_old = ((dt - ep->ti_create) >= ttl) ;
	}
        return (rs >= 0) ? f_old : rs ;
}
/* end subroutine (record_old) */


