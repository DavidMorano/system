/* namecache SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* real-name cache (from UNIX® System PASSWD database) */
/* version %I% last-modified %G% */

#define	CF_FULLNAME	0		/* use fullname? */

/* revision history:

	= 2004-01-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	namecache

	Description:
	This object provides a crude cache for PASSWD-DB real-names.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strncmp(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU |getustime(3u)| */
#include	<uclibmem.h>		/* LIBUC */
#include	<getax.h>		/* LIBUC */
#include	<getpwx.h>		/* LIBUC */
#include	<realname.h>		/* LIBUC */
#include	<mkgecosname.h>		/* LIBUC */
#include	<strwcpy.h>		/* LIBUC */
#include	<strdcpy.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU |REALNAMELEN| */

#include	"namecache.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| + |lenstr(3u)| */

/* local defines */

#define	NC		namecache
#define	NC_ENT		namecache_ent
#define	NC_ST		namecache_st

#ifndef	GNAMELEN
#define	GNAMELEN	REALNAMELEN	/* GECOS name length */
#endif

#ifndef	CF_FULLNAME
#define	CF_FULLNAME	0		/* use fullname? */
#endif


/* imported namespaces */

using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct namecache_ent {
	cchar		*username ;
	cchar		*realname ;
	char		*a ;
	time_t		ti_init ;
	time_t		ti_access ;
	int		realnamelen ;
} ; /* end struct (namecache_ent) */


/* forward references */

template<typename ... Args>
local int namecache_ctor(namecache *op,Args ... args) noex {
    	NAMECACHE	*hop = op ;
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->dbp = new(nothrow) hdb) != np) ylikely {
		rs = SR_OK ;
	    } /* end if (new-hdb) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (namecache_ctor) */

local int namecache_dtor(namecache *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->dbp) ylikely {
		delete op->dbp ;
		op->dbp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (namecache_dtor) */

template<typename ... Args>
local int namecache_magic(namecache *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == NAMECACHE_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (namecache_magic) */

local int	namecache_newent(NC *,NC_ENT **,cchar *,cchar *,int) noex ;
local int	namecache_repent(NC *,NC_ENT **,cchar *,cchar *,int) noex ;
local int	namecache_entfins(NC *) noex ;

local int	entry_start(NC_ENT *,cchar *,cchar *,int) noex ;
local int	entry_update(NC_ENT *,cchar *,int) noex ;
local int	entry_finish(NC_ENT *) noex ;
local int	entry_loadnames(NC_ENT *,cchar *,cchar *,int) noex ;

local int	mkaname(char *,int,cchar *) noex ;


/* local variables */

constexpr bool		f_fullname = CF_FULLNAME ;


/* exported variables */


/* exported subroutines */

int namecache_start(NC *op,cchar *vname,int nmax,int ttl) noex {
	cnullptr	np{} ;
	int		rs ;
	if ((rs = namecache_ctor(op,vname)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (vname[0]) ylikely {
	        if (nmax < 3) nmax = NAMECACHE_DEFMAX ;
	        if (ttl < 1) ttl = NAMECACHE_DEFTO ;
	        if (cchar *cp ; (rs = lm_strw(vname,-1,&cp)) >= 0) ylikely {
	            cint	ne = NAMECACHE_DEFENTS ;
	            op->vname = cp ;
	            if ((rs = hdb_start(op->dbp,ne,1,np,np)) >= 0) {
	                op->nmax = nmax ;
	                op->ttl = ttl ;
	                op->magval = NAMECACHE_MAGIC ;
	            } /* end if (hdb-start) */
	            if (rs < 0) {
			void *vp = voidp(op->vname) ;
		        lm_free(vp) ;
		        op->vname = nullptr ;
	            } /* end if (error) */
	        } /* end if (memory-acquire) */
	    } /* end if (valid) */
	    if (rs < 0) {
		namecache_dtor(op) ;
	    } /* end if (error) */
	} /* end if (namecache_ctor) */
	return rs ;
} /* end subroutine (namecache_start) */

int namecache_finish(NC *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = namecache_magic(op)) >= 0) ylikely {
	    {
	        rs1 = namecache_entfins(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->dbp) ylikely {
	        rs1 = hdb_finish(op->dbp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->vname) ylikely {
		void *vp = voidp(op->vname) ;
	        rs1 = lm_free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->vname = nullptr ;
	    } /* end if (memory-release) */
	    {
	        rs1 = namecache_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (namecache_finish) */

int namecache_add(NC *op,cchar *un,cchar *rnp,int rnl) noex {
	int		rs ;
	if ((rs = namecache_magic(op,un,rnp)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (un[0]) ylikely {
	        hdb_dat		key ;
	        hdb_dat		val{} ;
	        key.buf = un ;
	        key.len = lenstr(un) ;
	        if ((rs = hdb_fetch(op->dbp,key,nullptr,&val)) >= 0) {
	            NC_ENT	*ep = (NC_ENT *) val.buf ;
	            rs = entry_update(ep,rnp,rnl) ;
	        } else if (rs == SR_NOTFOUND) {
	            if ((rs = hdb_count(op->dbp)) >= op->nmax) {
	                rs = namecache_repent(op,nullptr,un,rnp,rnl) ;
	            } else {
	                rs = namecache_newent(op,nullptr,un,rnp,rnl) ;
	            }
	        } /* end if (hdb-fetch) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
} /* end subroutine (namecache_add) */

int namecache_lookup(NC *op,cchar *un,cchar **rpp) noex {
	int		rs ;
	int		rs1 ;
	int		rl = 0 ;
	if (rpp) *rpp = nullptr ;
	if ((rs = namecache_magic(op,un)) >= 0) ylikely {
	    custime	dt = getustime ;
	    rs = SR_INVALID ;
	    if (un[0]) ylikely {
	        cchar		*rp = nullptr ;
	        op->s.total += 1 ;
	        if (char *pwbuf ; (rs = lm_pw(&pwbuf)) >= 0) ylikely {
	            ucentpw	pw{} ;
	            NC_ENT	*ep ;
	            hdb_dat	key ;
	            hdb_dat	val{} ;
		    cint	pwlen = rs ;
	            cint	rlen = REALNAMELEN ;
	            char	rbuf[REALNAMELEN + 1] ;
	            key.buf = un ;
	            key.len = lenstr(un) ;
	            if ((rs = hdb_fetch(op->dbp,key,nullptr,&val)) >= 0) {
	                ep = (NC_ENT *) val.buf ;
	                ep->ti_access = dt ;
	                if (dt > (ep->ti_init + op->ttl)) {
		            ep->ti_init = dt ;
	                    if ((rs = getpwx_name(&pw,pwbuf,pwlen,un)) >= 0) {
		                cchar	*gecos = pw.pw_gecos ;
	                        if ((rs = mkaname(rbuf,rlen,gecos)) >= 0) {
		                    rl = rs ;
			            rs = entry_update(ep,rbuf,rl) ;
		                }
		            } /* end if (get-pwname) */
	                } /* end if (expiration) */
	                rp = ep->realname ;
	                rl = ep->realnamelen ;
	                if (rs >= 0) {
		            if (rl > 0) {
		                op->s.phits += 1 ;
		            } else {
		                op->s.nhits += 1 ;
		            }
	                }
	            } else if (rs == SR_NOTFOUND) {
	                if ((rs = getpwx_name(&pw,pwbuf,pwlen,un)) >= 0) {
		            cchar	*gecos = pw.pw_gecos ;
	                    if ((rs = mkaname(rbuf,rlen,gecos)) >= 0) {
		                rl = rs ;
	                        if ((rs = hdb_count(op->dbp)) >= op->nmax) {
	                            rs = namecache_repent(op,&ep,un,rbuf,rl) ;
	                        } else if (rs >= 0) {
	                            rs = namecache_newent(op,&ep,un,rbuf,rl) ;
		                }
	                        if ((rs >= 0) && ep) {
		                    rp = ep->realname ;
		                    rl = ep->realnamelen ;
	                        }
		            } /* end if (real-name) */
	                } /* end if (getpw_name) */
	            } /* end if (hdb_fetch) */
	            rs1 = lm_free(pwbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (memory-acquire) */
	        if (rpp) {
	            *rpp = (rs >= 0) ? rp : nullptr ;
	        }
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (namecache_lookup) */

int namecache_stats(NC *op,NC_ST *sp) noex {
	int		rs ;
	if ((rs = namecache_magic(op,sp)) >= 0) ylikely {
	    if ((rs = hdb_count(op->dbp)) >= 0) ylikely {
	        *sp = op->s ;
	        sp->nentries = rs ;
	    }
	} /* end if (magic) */
	return rs ;
} /* end subroutine (namecache_stats) */


/* private subroutines */

local int namecache_newent(NC *op,NC_ENT **epp,cc *un,cc *sp,int sl) noex {
	cint		msz = szof(NC_ENT) ;
	int		rs ;
	if (epp) *epp = nullptr ;
	if (NC_ENT *ep{} ; (rs = lm_mall(msz,&ep)) >= 0) ylikely {
	    if ((rs = entry_start(ep,un,sp,sl)) >= 0) ylikely {
	        hdb_dat		key ;
	        hdb_dat		val{} ;
	        key.buf = ep->username ;
	        key.len = lenstr(ep->username) ;
	        val.buf = ep ;
	        val.len = msz ;
	        if ((rs = hdb_store(op->dbp,key,val)) >= 0) {
		    if (epp) *epp = ep ;
		} /* end if (hdb-store) */
		if (rs < 0) {
		    entry_finish(ep) ;
		} /* end if (error) */
	    } /* end if (entry-start) */
	    if (rs < 0) {
		lm_free(ep) ;
	    } /* end if (error) */
	} /* end if (memory-acquire) */
	return rs ;
} /* end subroutine (namecache_newent) */

local int namecache_repent(NC *op,NC_ENT **epp,cc *un,cc *sp,int sl) noex {
	NC_ENT		*ep = nullptr ; /* used-afterwards */
	hdb		*dbp = op->dbp ;
	int		rs ;
	int		rs1 ;
	if (epp) *epp = nullptr ;
	if (hdb_cur cur{} ; (rs = hdb_curbegin(dbp,&cur)) >= 0) ylikely {
	    hdb_dat	key ;
	    hdb_dat	val{} ;
	    while ((rs = hdb_curenum(dbp,&cur,&key,&val)) >= 0) {
	        NC_ENT	*tep = (NC_ENT *) val.buf ;
		if ((ep == nullptr) || (ep->ti_access < tep->ti_access)) {
		    ep = tep ;
		}
	    } /* end while */
	    rs1 = hdb_curend(dbp,&cur) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (cursor) */
	if ((rs >= 0) && ep) {
	    entry_finish(ep) ;
	    rs = entry_start(ep,un,sp,sl) ;
	} else {
	    rs = SR_NOANODE ;
	}
	if (epp) *epp = ep ;
	return rs ;
} /* end subroutine (namecache_repent) */

local int namecache_entfins(NC *op) noex {
	hdb		*elp = op->dbp ;
	int		rs = SR_OK ;
	int		rs1 ;
	if (hdb_cur cur{} ; (rs1 = hdb_curbegin(elp,&cur)) >= 0) ylikely {
	    hdb_dat	key ;
	    hdb_dat	val{} ;
	    while (hdb_curenum(elp,&cur,&key,&val) >= 0) {
	        NC_ENT	*ep = (NC_ENT *) val.buf ;
		{
	            rs1 = entry_finish(ep) ;
	            if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = lm_free(ep) ;
	            if (rs >= 0) rs = rs1 ;
		} /* end if (memory-release) */
	    } /* end while */
	    rs1 = hdb_curend(elp,&cur) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if */
	if (rs >= 0) rs = rs1 ;
	return rs ;
} /* end subroutine (namecache_entfins) */

local int entry_start(NC_ENT *ep,cchar *up,cchar *rp,int rl) noex {
	custime		dt = getustime ;
	int		rs = SR_FAULT ;
	if (ep && up && rp) ylikely {
	    rs = SR_INVALID ;
	    memclear(ep) ;
	    if (up[0]) ylikely {
	        if (rl < 0) rl = lenstr(rp) ;
	        ep->ti_init = dt ;
	        ep->ti_access = dt ;
	        ep->realnamelen = rl ;
	        rs = entry_loadnames(ep,up,rp,rl) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (entry_start) */

local int entry_finish(NC_ENT *ep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (ep) ylikely {
	    rs = SR_OK ;
	    if (ep->a) ylikely {
	        rs1 = lm_free(ep->a) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->a = nullptr ;
	    } /* end if (memory-release) */
	    ep->username = nullptr ;
	    ep->realname = nullptr ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (entry_finish) */

local int entry_update(NC_ENT *ep,cchar *rp,int rl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		f_changed = false ;
	if (ep && rp) ylikely {
	    rs = SR_OK ;
	    if (rl < 0) rl = lenstr(rp) ;
	    f_changed = true ;
	    f_changed = f_changed && (strncmp(ep->realname,rp,rl) == 0) ;
	    f_changed = f_changed && (ep->realname[rl] == '\0') ;
	    if (f_changed) {
	        ep->ti_init = getustime ;
	        ep->realnamelen = rl ;
		if (char *ubuf ; (rs = lm_un(&ubuf)) >= 0) ylikely {
		    cint	ulen = rs ;
		    {
		        strdcpy1(ubuf,ulen,ep->username) ;
	                rs = entry_loadnames(ep,ubuf,rp,rl) ;
		    }
		    rs1 = lm_free(ubuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (changed) */
	} /* end if (non-null) */
	return (rs >= 0) ? f_changed : rs ;
} /* end subroutine (entry_update) */

local int entry_loadnames(NC_ENT *ep,cchar *up,cchar *rp,int rl) noex {
	int		rs ;
	int		sz = 0 ;
	if (ep->a != nullptr) {
	    lm_free(ep->a) ;
	    ep->a = nullptr ;
	} /* end if (memory-release) */
	sz += (lenstr(up)+1) ;
	sz += (lenstr(rp,rl)+1) ;
	if (char *bp ; (rs = lm_mall(sz,&bp)) >= 0) ylikely {
	    ep->a = bp ;
	    ep->username = bp ;
	    bp = (strwcpy(bp,up,-1)+1) ;
	    ep->realname = bp ;
	    bp = (strwcpy(bp,rp,rl)+1) ;
	} /* end if (memory-acquire) */
	return rs ;
} /* end subroutine (entry_loadnames) */

/* make a real name from a GECOS name */
local int mkaname(char *nbuf,int nlen,cchar *gecos) noex {
	cint		glen = GNAMELEN ;
	int		rs ;
	int		rs1 ;
	int		rl = 0 ;
	char		gbuf[GNAMELEN+1] ;
	nbuf[0] = '\0' ;
	if ((rs = mkgecosname(gbuf,glen,gecos)) >= 0) ylikely {
	    cint	gl = rs ;
	    if (realname rn ; (rs = rn.start(gbuf,gl)) >= 0) ylikely {
		if_constexpr (f_fullname) {
	            rl = rn.fullname(nbuf,nlen) ;
		} else {
	            rl = rn.name(nbuf,nlen) ;
		} /* end if_constexpr (f_fullanem) */
	        rs1 = rn.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (realname) */
	} /* end if (mkgeoxname) */
	return (rs >= 0) ? rl : 0 ;
} /* end subroutine (mkaname) */


