/* pwcache SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* system PASSWD database cache */
/* version %I% last-modified %G% */


/* revision history:

        = 2004-01-10, David A­D­ Morano
        This code was originally written.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	pwcache

	Description:
        This object provides a crude cache for PASSWD-DB entries.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include        <sys/types.h>		/* |uid_t| */
#include        <ctime>
#include	<cstddef>		/* |nullptr_t| */
#include        <cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>		/* |getustime(3u)| */
#include	<uclibmem.h>
#include        <ucgetpw.h>
#include        <strwcpy.h>
#include        <localmisc.h>

#include        "pwcache.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define PWE             ucentpw


/* imported namespaces */

using std::nothrow ;			/* constant */


/* local typedefs */

typedef ucentpw *	pwentp ;


/* external subroutines */


/* external variables */


/* local structures */

enum cts {
        ct_miss,
        ct_hit,
        ct_overlast
} ;

struct pwcache_rec : pq_ent {
        char            *un ;		/* allocated: username */
        char            *pwbuf ;	/* allocated: PW-buffer */
        ucentpw		pw ;
        time_t          ti_create ;
        time_t          ti_access ;
        uint            wcount ;
        int             pwl ;
} ;

typedef pwcache_rec	rec ;
typedef pwcache_rec *	recp ;

namespace {
    struct ureq {
	cchar		*un = nullptr ;
	uid_t		uid = 0 ;
	ureq(cchar *p) noex : un(p) { } ;
	ureq(uid_t u) noex : uid(u) { } ;
    } ; /* end if (ureq) */
}


/* forward references */

template<typename ... Args>
local inline int pwcache_ctor(pwcache *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    op->magval = 0 ;
	    op->s = {} ;
	    op->ti_check = 0 ;
	    op->wcount = 0 ;
	    op->ttl = 0 ;
	    op->nmax = 0 ;
	    if ((op->dbp = new(nothrow) hdb) != nullptr) ylikely {
	        if ((op->lrup = new(nothrow) pq) != nullptr) ylikely {
		    rs = SR_OK ;
		} /* end if (new-pq) */
	        if (rs < 0) {
		    delete op->dbp ;
		    op->dbp = nullptr ;
		} /* end if (error) */
	    } /* end if (new-hdb) */
	} /* end if (non-null) */
	return rs ;
}
/* end method (pwcache_ctor) */

local inline int pwcache_dtor(pwcache *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->lrup) ylikely {
		delete op->lrup ;
		op->lrup = nullptr ;
	    }
	    if (op->dbp) ylikely {
		delete op->dbp ;
		op->dbp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end method (pwcache_dtor) */

template<typename ... Args>
local int pwcache_magic(pwcache *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == PWCACHE_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end method (pwcache_magic) */

local int      pwcache_mkrec(pwcache *,time_t,rec **,ureq *) noex ;
local int      pwcache_newrec(pwcache *,time_t,rec **,ureq *) noex ;
local int      pwcache_oldrec(pwcache *,rec *) noex ;
local int      pwcache_recstart(pwcache *,time_t,rec *,ureq *) noex ;
local int      pwcache_recdel(pwcache *,rec *) noex ;
local int      pwcache_recaccess(pwcache *,time_t,rec *) noex ;
local int      pwcache_recrear(pwcache *,rec *) noex ;
local int      pwcache_recfins(pwcache *) noex ;
local int      pwcache_record(pwcache *,int,int) noex ;
local int	pwcache_finduid(pwcache *,rec **,uid_t) noex ;

local int	record_start(rec *,time_t,int,ureq *) noex ;
local int	record_loadun(rec *,ucentpw *) noex ;
local int	record_access(rec *,time_t) noex ;
local int	record_refresh(rec *,time_t,int) noex ;
local int	record_old(rec *,time_t,int) noex ;
local int	record_finish(rec *) noex ;

local int	getpw(ucentpw *,char *,int,ureq *) noex ;
local int	getpw_name(ucentpw *,char *,int,cchar *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int pwcache_start(pwcache *op,int nmax,int ttl) noex {
	cnullptr	np{} ;
        int             rs  ;
	if ((rs = pwcache_ctor(op)) >= 0) ylikely {
            if (nmax < PWCACHE_DEFMAX) nmax = PWCACHE_DEFMAX ;
            if (ttl < PWCACHE_DEFTTL) ttl = PWCACHE_DEFTTL ;
            if ((rs = hdb_start(op->dbp,nmax,1,np,np)) >= 0) ylikely {
                if ((rs = pq_start(op->lrup)) >= 0) ylikely {
                    op->nmax = nmax ;
                    op->ttl = ttl ;
                    op->ti_check = getustime ;
                    op->magval = PWCACHE_MAGIC ;
                }
                if (rs < 0) {
                    hdb_finish(op->dbp) ;
		} /* end if (error) */
            } /* end if (hdb-start) */
	    if (rs < 0) {
		pwcache_dtor(op) ;
	    } /* end if (error) */
        } /* end if (non-null) */
        return rs ;
} /* end subroutine (pwcache_start) */

int pwcache_finish(pwcache *op) noex {
        int             rs ;
        int             rs1 ;
	if ((rs = pwcache_magic(op)) >= 0) ylikely {
            if (op->lrup) ylikely {
                rs1 = pq_finish(op->lrup) ; /* finish up the LRU queue */
                if (rs >= 0) rs = rs1 ;
            }
            {
                rs1 = pwcache_recfins(op) ; /* freeing all cache entries */
                if (rs >= 0) rs = rs1 ;
            }
            if (op->dbp) ylikely {
                rs1 = hdb_finish(op->dbp) ; /* free up everything else */
                if (rs >= 0) rs = rs1 ;
            }
	    {
		rs1 = pwcache_dtor(op) ;
                if (rs >= 0) rs = rs1 ;
	    }
            op->magval = 0 ;
	} /* end if (magic) */
        return rs ;
} /* end subroutine (pwcache_finish) */

int pwcache_lookup(pwcache *op,PWE *pwp,char *pwbuf,int pwlen,cchar *un) noex {
        custime    	dt = getustime ;
        int             rs ;
	if ((rs = pwcache_magic(op,pwp,pwbuf,un)) >= 0) ylikely {
            rs = SR_INVALID ;
            if (un[0]) ylikely {
                hdb_dat		key ;
                hdb_dat		val{} ;
                rec		*ep = nullptr ;
                int		ct = 0 ;
                op->s.total += 1 ;
                key.buf = un ;
                key.len = lenstr(un) ;
                if ((rs = hdb_fetch(op->dbp,key,nullptr,&val)) >= 0) {
                    ep = (rec *) val.buf ;
                    ct = ct_hit ;
                    rs = pwcache_recaccess(op,dt,ep) ;
                } else if (rs == SR_NOTFOUND) {
		    ureq	ro(un) ;
                    ct = ct_miss ;
                    rs = pwcache_mkrec(op,dt,&ep,&ro) ;
                } /* end if (hit or miss) */
                pwcache_record(op,ct,rs) ;
                if ((rs > 0) && ep) { /* not '>=' */
                    ucentpw         *entp = pwentp(pwp) ;
                    ucentpw         *opwp = pwentp(&ep->pw) ;
                    rs = entp->load(pwbuf,pwlen,opwp) ;
                } else if (rs == 0) {
                    rs = SR_NOTFOUND ;
                }
                if (rs <= 0) {		/* note '<=' */
                    memclear(pwp) ;
                }
            } /* end if (valid) */
	} /* end if (magic) */
        return rs ;
} /* end subroutine (pwcache_lookup) */

int pwcache_uid(pwcache *op,PWE *pwp,char *pwbuf,int pwlen,uid_t uid) noex {
        custime		dt = getustime ;
        int             rs ;
	if ((rs = pwcache_magic(op,pwp,pwbuf)) >= 0) ylikely {
            rec         *ep = nullptr ;
            int         ct = 0 ;
            op->s.total += 1 ;
            if ((rs = pwcache_finduid(op,&ep,uid)) >= 0) {
                ct = ct_hit ;
                rs = pwcache_recaccess(op,dt,ep) ;
            } else if (rs == SR_NOTFOUND) {
                ureq        ro(uid) ;
                ct = ct_miss ;
                rs = pwcache_mkrec(op,dt,&ep,&ro) ;
            } /* end if (hit or miss) */
            pwcache_record(op,ct,rs) ;
            if ((rs > 0) && ep) { /* not '>=' */
                ucentpw         *entp = pwentp(pwp) ;
                ucentpw         *opwp = pwentp(&ep->pw) ;
                rs = entp->load(pwbuf,pwlen,opwp) ;
            } else if (rs == 0) {
                rs = SR_NOTFOUND ;
            }
            if (rs <= 0) {		/* note '<=' */
                memclear(pwp) ;
            }
	} /* end if (magic) */
        return rs ;
} /* end subroutine (pwcache_uid) */

int pwcache_invalidate(pwcache *op,cchar *un) noex {
	int             rs ;
        int             rs1 ;
        int             f_found = false ;
	if ((rs = pwcache_magic(op,un)) >= 0) ylikely {
            rs = SR_INVALID ;
            if (un[0]) ylikely {
                hdb_dat   key ;
                hdb_dat   val{} ;
                key.buf = un ;
                key.len = lenstr(un) ;
                if ((rs = hdb_fetch(op->dbp,key,nullptr,&val)) >= 0) ylikely {
                    rec     *ep = (rec *) val.buf ;
                    f_found = true ;
                    if (op->lrup) ylikely {
                        pq_ent      *pep = (pq_ent *) ep ;
                        rs1 = pq_unlink(op->lrup,pep) ;
                        if (rs >= 0) rs = rs1 ;
                    }
                    if (op->dbp) ylikely {
                        rs1 = hdb_delkey(op->dbp,key) ;
                        if (rs >= 0) rs = rs1 ;
                    }
                    {
                        rs1 = record_finish(ep) ;
                        if (rs >= 0) rs = rs1 ;
                    }
                    {
                        rs1 = lm_free(ep) ;
                        if (rs >= 0) rs = rs1 ;
                    }
                } else if (rs == SR_NOTFOUND) {
                    rs = SR_OK ;
                }
            } /* end if (valid) */
	} /* end if (magic) */
        return (rs >= 0) ? f_found : rs ;
} /* end subroutine (pwcache_invalidate) */

int pwcache_check(pwcache *op,time_t dt) noex {
        int             rs ;
        int             rs1 ;
        int             f = false ;
	if ((rs = pwcache_magic(op)) >= 0) ylikely {
            if (hdb_cur cur ; (rs = hdb_curbegin(op->dbp,&cur)) >= 0) ylikely {
                hdb_dat       key{} ;
                hdb_dat       val{} ;
                if (dt == 0) dt = getustime ;
                while (rs >= 0) {
                    rs1 = hdb_curenum(op->dbp,&cur,&key,&val) ;
                    if (rs1 == SR_NOTFOUND) break ;
                    rs = rs1 ;
                    if (rs >= 0) ylikely {
                        rec         *ep = recp(val.buf) ;
                        if ((rs = record_old(ep,dt,op->ttl)) > 0) ylikely {
                            f = true ;
                            if ((rs = pwcache_recdel(op,ep)) >= 0) ylikely {
                                pq_ent      *pep = (pq_ent *) ep ;
                                rs = pq_unlink(op->lrup,pep) ;
                                lm_free(ep) ;
                            }
                        } /* end if (entry-old) */
                    } /* end if (ok) */
                } /* end while */
                rs1 = hdb_curend(op->dbp,&cur) ;
                if (rs >= 0) rs = rs1 ;
            } /* end if (hdb-cur) */
	} /* end if (magic) */
        return (rs >= 0) ? f : rs ;
} /* end subroutine (pwcache_check) */

int pwcache_getstat(pwcache *op,pwcache_st *sp) noex {
        int             rs ;
	if ((rs = pwcache_magic(op,sp)) >= 0) ylikely {
            if ((rs = hdb_count(op->dbp)) >= 0) ylikely {
                *sp = op->s ;
                sp->nentries = rs ;
            }
	} /* end if (magic) */
        return rs ;
} /* end subroutine (pwcache_getstat) */


/* private subroutines */

local int pwcache_mkrec(pwcache *op,time_t dt,rec **epp,ureq *rp) noex {
        int             rs ;
        int             rs1 ;
        int             pwl = 0 ;
        *epp = nullptr ;
        if ((rs = hdb_count(op->dbp)) >= 0) ylikely {
            pq_ent	*pep{} ;
            cint	n = rs ;
            if (n >= op->nmax) {
                if ((rs = pq_rem(op->lrup,&pep)) >= 0) ylikely {
                    rec		*ep = recp(pep) ;
                    if ((rs = pwcache_recdel(op,ep)) >= 0) ylikely {
                        if ((rs = pwcache_recstart(op,dt,ep,rp)) >= 0) {
                            pwl = rs ;
                	    if (epp) {
				*epp = (rec *) pep ;
			    }
			}
                    }
                    rs1 = pq_ins(op->lrup,pep) ;
                    if (rs >= 0) rs = rs1 ;
                } /* end if (removed entry) */
            } else {
                if ((rs = pwcache_newrec(op,dt,epp,rp)) >= 0) ylikely {
                    pwl = rs ;
                    if (*epp) {
                        pep = (pq_ent *) *epp ;
                        rs = pq_ins(op->lrup,pep) ;
                    }
		    if ((rs < 0) && *epp) {
			pwcache_oldrec(op,*epp) ;
		    } /* end if (error) */
                } /* end if (new-entry) */
            } /* end if */
        } /* end if (hdb_count) */
        return (rs >= 0) ? pwl : rs ;
} /* end subroutine (pwcache_mkrec) */

local int pwcache_newrec(pwcache *op,time_t dt,rec **epp,ureq *rp) noex {
        int             rs = SR_BUGCHECK ;
        if (epp) ylikely {
            cint	rsz = szof(rec) ;
            rec		*ep = nullptr ; /* used-afterwards */
            if ((rs = lm_mall(rsz,&ep)) >= 0) ylikely {
                rs = pwcache_recstart(op,dt,ep,rp) ;
                if (rs < 0) {
		    lm_free(ep) ;
		} /* end if (error) */
            } /* end if (memory-acquire) */
            *epp = (rs >= 0) ? ep : nullptr ;
	} /* end if (non-null) */
        return rs ;
} /* end subroutine (pwcache_newrec) */

local int pwcache_oldrec(pwcache *op,rec *ep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op && ep) ylikely {
	    {
                rs1 = record_finish(ep) ;
                if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = lm_free(ep) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	}
	return rs ;
} /* end subroutine (pwcache_newrec) */

local int pwcache_recstart(pwcache *op,time_t dt,rec *ep,ureq *rp) noex {
        cint		wc = op->wcount++ ;
        int             rs ;
        int             pwl = 0 ;
        if ((rs = record_start(ep,dt,wc,rp)) >= 0) ylikely {
            hdb_dat	key ;
            hdb_dat	val ;
            cint	rsz = szof(rec) ;
            pwl = rs ;
            key.buf = ep->un ;
            key.len = lenstr(ep->un) ;
            val.buf = ep ;
            val.len = rsz ;
            rs = hdb_store(op->dbp,key,val) ;
            if (rs < 0) {
                record_finish(ep) ;
	    } /* end if (error) */
        } /* end if (entry-start) */
        return (rs >= 0) ? pwl : rs ;
} /* end subroutine (pwcache_recstart) */

local int pwcache_recaccess(pwcache *op,time_t dt,rec *ep) noex {
        int             rs ;
        if ((rs = pwcache_recrear(op,ep)) >= 0) ylikely {
            if ((rs = record_old(ep,dt,op->ttl)) > 0) ylikely {
                cint     wc = op->wcount++ ;
                op->s.refreshes += 1 ;
                rs = record_refresh(ep,dt,wc) ;
            } else {
                rs = record_access(ep,dt) ;
            }
        } /* end if */
        return rs ;
} /* end subroutine (pwcache_recaccess) */

local int pwcache_recrear(pwcache *op,rec *ep) noex {
        pq_ent          *pcp = (pq_ent *) ep ;
        int             rs ;
        if (pq_ent *pep{} ; (rs = pq_gettail(op->lrup,&pep)) >= 0) ylikely {
            if (pcp != pep) {
                pep = (pq_ent *) ep ;
                if ((rs = pq_unlink(op->lrup,pep)) >= 0) {
                    rs = pq_ins(op->lrup,pep) ;
                    if (rs < 0) {
			rec	*rep = (rec *) pep ;
                        record_finish(rep) ;
                        lm_free(pep) ;
                    } /* end if (error) */
                }
            }
        } /* end if (pq-gettail) */
        return rs ;
} /* end subroutine (pwcache_recrear) */

local int pwcache_recdel(pwcache *op,rec *ep) noex {
        int             rs = SR_OK ;
        int             rs1 ;
	{
            hdb_dat	key ;
            key.buf = ep->un ;
            key.len = lenstr(ep->un) ;
            rs1 = hdb_delkey(op->dbp,key) ;
            if (rs >= 0) rs = rs1 ;
	}
	{
            rs1 = record_finish(ep) ;
            if (rs >= 0) rs = rs1 ;
	}
        return rs ;
} /* end subroutine (pwcache_recdel) */

local int pwcache_recfins(pwcache *op) noex {
        int             rs = SR_OK ;
        int             rs1 ;
        if (hdb_cur cur{} ; (rs1 = hdb_curbegin(op->dbp,&cur)) >= 0) ylikely {
            hdb_dat	key{} ;
            hdb_dat	val{} ;
            while (hdb_curenum(op->dbp,&cur,&key,&val) >= 0) {
        	rec	*ep = recp(val.buf) ;
		rs1 = pwcache_oldrec(op,ep) ;
                if (rs >= 0) rs = rs1 ;
            } /* end while */
            rs1 = hdb_curend(op->dbp,&cur) ;
            if (rs >= 0) rs = rs1 ;
        } /* end if */
        if (rs >= 0) rs = rs1 ;
        return rs ;
} /* end subroutine (pwcache_recfins) */

local int pwcache_record(pwcache *op,int ct,int rs) noex {
	if (rs >= 0) ylikely {
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
	} /* end if (ok) */
        return SR_OK ;
} /* end subroutine (pwcache_record) */

local int pwcache_finduid(pwcache *op,rec **rpp,uid_t uid) noex {
	hdb		*dbp = op->dbp ;
	int		rs ;
	int		rs1 ;
	int		ffound = false ;
        rec         	*ep = nullptr ;
	if (hdb_cur cur{} ; (rs = hdb_curbegin(dbp,&cur)) >= 0) ylikely {
	    hdb_dat   key{} ;
	    hdb_dat   val{} ;
            while ((rs = hdb_curenum(dbp,&cur,&key,&val)) >= 0) {
                ep = (rec *) val.buf ;
		{
		    ucentpw	*pwp = pwentp(&ep->pw) ;
		    ffound = (pwp->pw_uid == uid) ;
		    if (ffound) break ;
		}
	    } /* end while */
	    if (rs == SR_NOTFOUND) rs = SR_OK ;
	    rs1 = hdb_curend(dbp,&cur) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (hdb-cur) */
	if (rpp) {
	    *rpp = (ffound) ? ep : nullptr ;
	}
	return (rs >= 0) ? ffound : rs ;
} /* end subroutine (pwcache_finduid) */

local int record_start(rec *ep,time_t dt,int wc,ureq *rp) noex {
        int             rs = SR_FAULT ;
        int             rs1 ;
        int             pwl = 0 ;
        if (ep && rp) ylikely {
            memclear(ep) ; /* dangerous */
            if (char *pwbuf ; (rs = lm_pw(&pwbuf)) >= 0) ylikely {
                ucentpw		pw{} ;
                cint		pwlen = rs ;
                if ((rs = getpw(&pw,pwbuf,pwlen,rp)) >= 0) ylikely {
		    if ((rs = record_loadun(ep,&pw)) >= 0) ylikely {
                        pwl = rs ;
                        if (void *vp ; (rs = lm_mall((pwl+1),&vp)) >= 0) {
			    ucentpw	*pwp = pwentp(&ep->pw) ;
			    ucentpw	*opwp = pwentp(&pw) ;
                            char	*pwb = charp(vp) ;
                            if ((rs = pwp->load(pwb,pwl,opwp)) >= 0) {
                                ep->pwbuf = pwb ;
                                ep->pwl = pwl ;
                            }
                            if (rs < 0) {
				lm_free(vp) ;
			    } /* end if (error) */
                        } /* end if (memory-acquire - pwbuf) */
	    	    } /* end if (memory-acquire - un) */
                } else if (rs == SR_NOTFOUND) {
                    ep->pwl = 0 ; /* optional */
                    pwl = 0 ; /* indicates a not-found entry */
                } /* end if (getpw) */
                rs1 = lm_free(pwbuf) ;
                if (rs >= 0) rs = rs1 ;
            } /* end if (m-a-f) */
            if (rs >= 0) ylikely {
                ep->ti_create = dt ;
                ep->ti_access = dt ;
                ep->wcount = wc ;
            }
	    if (rs < 0) {
		record_finish(ep) ;
	    } /* end if (error) */
	} /* end if (non-null) */
        return (rs >= 0) ? pwl : rs ;
} /* end subroutine (record_start) */

local int record_loadun(rec *ep,ucentpw *pwp) noex {
	int		rs ;
	cchar		*un = pwp->pw_name ;
	if (cchar *cp ; (rs = lm_strw(un,-1,&cp)) >= 0) ylikely {
	    ep->un = cast_const<charp>(cp) ;
	}
	return rs ;
} /* end subroutine (record_loadun) */

local int record_finish(rec *ep) noex {
        int             rs = SR_FAULT ;
        int             rs1 ;
        if (ep) ylikely {
	    rs = SR_OK ;
            if (ep->pwbuf) ylikely {
                rs1 = lm_free(ep->pwbuf) ;
                if (rs >= 0) rs = rs1 ;
                ep->pwbuf = nullptr ;
            }
	    if (ep->un) ylikely {
                rs1 = lm_free(ep->un) ;
                if (rs >= 0) rs = rs1 ;
                ep->un = nullptr ;
	    }
            ep->pwl = 0 ;
	} /* end if (non-null) */
        return rs ;
} /* end subroutine (record_finish) */

local int record_access(rec *ep,time_t dt) noex {
        int             rs = SR_FAULT ;
        int             pwl = 0 ;
        if (ep) ylikely {
	    rs = SR_OK ;
            ep->ti_access = dt ;
            pwl  = ep->pwl ;
	} /* end if (non-null) */
        return (rs >= 0) ? pwl : rs ;
} /* end subroutine (record_access) */

local int record_refresh(rec *ep,time_t dt,int wc) noex {
        int             rs = SR_FAULT ;
        int             rs1 ;
        int             pwl = 0 ;
        if (ep) ylikely {
            if (char *pwbuf ; (rs = lm_pw(&pwbuf)) >= 0) ylikely {
                cint		pwlen = rs ;
                ucentpw		pw{} ;
                if ((rs = getpw_name(&pw,pwbuf,pwlen,ep->un)) >= 0) ylikely {
                    void    *vp{} ;
                    pwl = rs ;
                    ep->pwl = pwl ;
                    if (ep->pwbuf) {
                        rs = lm_rall(ep->pwbuf,(pwl+1),&vp) ;
                    } else {
                        rs = lm_mall((pwl+1),&vp) ;
                    }
                    if (rs >= 0) ylikely {
                        ucentpw     *pwp = pwentp(&ep->pw) ;
                        ucentpw     *opwp = pwentp(&pw) ;
                        char        *mbuf = charp(vp) ;
                        ep->pwbuf = charp(vp) ;
                        rs = pwp->load(mbuf,pwl,opwp) ;
                        if (rs < 0) {
			    lm_free(vp) ;
			} /* end if (error) */
                    } /* end if (ok) */
                } else if (rs == SR_NOTFOUND) {
                    if (ep->pwbuf != nullptr) {
                        lm_free(ep->pwbuf) ;
                        ep->pwbuf = nullptr ;
                    }
                    ep->pwl = 0 ;
                    pwl = 0 ; /* indicates an empty (not-found) entry */
                } /* end if (getpw_name) */
                rs1 = lm_free(pwbuf) ;
                if (rs >= 0) rs = rs1 ;
            } /* end if (m-a) */
            if (rs >= 0) ylikely {
                ep->ti_create = dt ;
                ep->ti_access = dt ;
                ep->wcount = wc ;
            }
	} /* end if (non-null) */
        return (rs >= 0) ? pwl : rs ;
} /* end subroutine (record_refresh) */

local int record_old(rec *ep,time_t dt,int ttl) noex {
	int		rs = SR_FAULT ;
        int             f_old = false ;
        if (ep) ylikely {
	    rs = SR_OK ;
            f_old = ((dt - ep->ti_create) >= ttl) ;
	}
        return (rs >= 0) ? f_old : rs ;
} /* end subroutine (record_old) */

local int getpw(ucentpw *pwp,char *pwbuf,int pwlen,ureq *rp) noex {
	ucentpw		*pp = pwentp(pwp) ;
	int		rs ;
	if (rp->un) {
	    rs = uc_getpwnam(pp,pwbuf,pwlen,rp->un) ;
	} else {
	    rs = uc_getpwuid(pp,pwbuf,pwlen,rp->uid) ;
	}
	return rs ;
} /* end subroutine */

local int getpw_name(ucentpw *pwp,char *pwbuf,int pwlen,cchar *un) noex {
	return uc_getpwnam(pwp,pwbuf,pwlen,un) ;
} /* end subroutine */


