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
#include	<sys/types.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strncmp(3c)| */
#include	<usystem.h>
#include	<getbufsize.h>
#include	<getax.h>
#include	<getpwx.h>
#include	<mallocxx.h>
#include	<realname.h>
#include	<mkgecosname.h>
#include	<strwcpy.h>
#include	<strdcpy.h>
#include	<localmisc.h>		/* |REALNAMELEN| */

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

using libuc::libmem ;			/* variable */
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
static int namecache_ctor(namecache *op,Args ... args) noex {
    	NAMECACHE	*hop = op ;
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->dbp = new(nothrow) hdb) != np) {
		rs = SR_OK ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (namecache_ctor) */

static int namecache_dtor(namecache *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->dbp) {
		delete op->dbp ;
		op->dbp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (namecache_dtor) */

template<typename ... Args>
static int namecache_magic(namecache *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == NAMECACHE_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (namecache_magic) */

static int	namecache_newent(NC *,NC_ENT **,cchar *,cchar *,int) noex ;
static int	namecache_repent(NC *,NC_ENT **,cchar *,cchar *,int) noex ;
static int	namecache_entfins(NC *) noex ;

static int	entry_start(NC_ENT *,cchar *,cchar *,int) noex ;
static int	entry_update(NC_ENT *,cchar *,int) noex ;
static int	entry_finish(NC_ENT *) noex ;
static int	entry_loadnames(NC_ENT *,cchar *,cchar *,int) noex ;

static int	mkaname(char *,int,cchar *) noex ;


/* local variables */

constexpr bool		f_fullname = CF_FULLNAME ;


/* exported variables */


/* exported subroutines */

int namecache_start(NC *op,cchar *vname,int nmax,int ttl) noex {
	cnullptr	np{} ;
	int		rs ;
	if ((rs = namecache_ctor(op,vname)) >= 0) {
	    rs = SR_INVALID ;
	    if (vname[0]) {
	        if (nmax < 3) nmax = NAMECACHE_DEFMAX ;
	        if (ttl < 1) ttl = NAMECACHE_DEFTO ;
	        if (cchar *cp ; (rs = libmem.strw(vname,-1,&cp)) >= 0) {
	            cint	ne = NAMECACHE_DEFENTS ;
	            op->vname = cp ;
	            if ((rs = hdb_start(op->dbp,ne,1,np,np)) >= 0) {
	                op->nmax = nmax ;
	                op->ttl = ttl ;
	                op->magic = NAMECACHE_MAGIC ;
	            } /* end if (hdb-start) */
	            if (rs < 0) {
			void *vp = voidp(op->vname) ;
		        libmem.free(vp) ;
		        op->vname = nullptr ;
	            }
	        } /* end if (memory-allocation) */
	    } /* end if (valid) */
	    if (rs < 0) {
		namecache_dtor(op) ;
	    }
	} /* end if (namecache_ctor) */
	return rs ;
}
/* end subroutine (namecache_start) */

int namecache_finish(NC *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = namecache_magic(op)) >= 0) {
	    {
	        rs1 = namecache_entfins(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->dbp) {
	        rs1 = hdb_finish(op->dbp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->vname) {
		void *vp = voidp(op->vname) ;
	        rs1 = libmem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->vname = nullptr ;
	    }
	    {
	        rs1 = namecache_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (namecache_finish) */

int namecache_add(NC *op,cchar *un,cchar *rnp,int rnl) noex {
	int		rs ;
	if ((rs = namecache_magic(op,un,rnp)) >= 0) {
	    rs = SR_INVALID ;
	    if (un[0]) {
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
}
/* end subroutine (namecache_add) */

int namecache_lookup(NC *op,cchar *un,cchar **rpp) noex {
	int		rs ;
	int		rs1 ;
	int		rl = 0 ;
	if (rpp) *rpp = nullptr ;
	if ((rs = namecache_magic(op,un)) >= 0) {
	    custime	dt = getustime ;
	    rs = SR_INVALID ;
	    if (un[0]) {
	        cchar		*rp = nullptr ;
	        op->s.total += 1 ;
	        if (char *pwbuf{} ; (rs = malloc_pw(&pwbuf)) >= 0) {
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
	            rs1 = libmem.free(pwbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (memory-allocation) */
	        if (rpp) {
	            *rpp = (rs >= 0) ? rp : nullptr ;
	        }
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (namecache_lookup) */

int namecache_stats(NC *op,NC_ST *sp) noex {
	int		rs ;
	if ((rs = namecache_magic(op,sp)) >= 0) {
	    if ((rs = hdb_count(op->dbp)) >= 0) {
	        *sp = op->s ;
	        sp->nentries = rs ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (namecache_stats) */


/* private subroutines */

static int namecache_newent(NC *op,NC_ENT **epp,cc *un,cc *sp,int sl) noex {
	cint		msz = szof(NC_ENT) ;
	int		rs ;
	if (epp) *epp = nullptr ;
	if (NC_ENT *ep{} ; (rs = libmem.mall(msz,&ep)) >= 0) {
	    if ((rs = entry_start(ep,un,sp,sl)) >= 0) {
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
		}
	    } /* end if (entry-start) */
	    if (rs < 0) {
		libmem.free(ep) ;
	    }
	} /* end if (memory-allocation) */
	return rs ;
}
/* end subroutine (namecache_newent) */

static int namecache_repent(NC *op,NC_ENT **epp,cc *un,cc *sp,int sl) noex {
	NC_ENT		*ep = nullptr ; /* used-afterwards */
	hdb		*dbp = op->dbp ;
	int		rs ;
	int		rs1 ;
	if (epp) *epp = nullptr ;
	if (hdb_cur cur{} ; (rs = hdb_curbegin(dbp,&cur)) >= 0) {
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
}
/* end subroutine (namecache_repent) */

static int namecache_entfins(NC *op) noex {
	hdb		*elp = op->dbp ;
	int		rs = SR_OK ;
	int		rs1 ;
	if (hdb_cur cur{} ; (rs1 = hdb_curbegin(elp,&cur)) >= 0) {
	    hdb_dat	key ;
	    hdb_dat	val{} ;
	    while (hdb_curenum(elp,&cur,&key,&val) >= 0) {
	        NC_ENT	*ep = (NC_ENT *) val.buf ;
		{
	            rs1 = entry_finish(ep) ;
	            if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = libmem.free(ep) ;
	            if (rs >= 0) rs = rs1 ;
		}
	    } /* end while */
	    rs1 = hdb_curend(elp,&cur) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if */
	if (rs >= 0) rs = rs1 ;
	return rs ;
}
/* end subroutine (namecache_entfins) */

static int entry_start(NC_ENT *ep,cchar *up,cchar *rp,int rl) noex {
	custime		dt = getustime ;
	int		rs = SR_FAULT ;
	if (ep && up && rp) {
	    rs = SR_INVALID ;
	    memclear(ep) ;
	    if (up[0]) {
	        if (rl < 0) rl = lenstr(rp) ;
	        ep->ti_init = dt ;
	        ep->ti_access = dt ;
	        ep->realnamelen = rl ;
	        rs = entry_loadnames(ep,up,rp,rl) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entry_start) */

static int entry_finish(NC_ENT *ep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (ep) {
	    rs = SR_OK ;
	    if (ep->a) {
	        rs1 = libmem.free(ep->a) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->a = nullptr ;
	    }
	    ep->username = nullptr ;
	    ep->realname = nullptr ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entry_finish) */

static int entry_update(NC_ENT *ep,cchar *rp,int rl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		f_changed = false ;
	if (ep && rp) {
	    rs = SR_OK ;
	    if (rl < 0) rl = lenstr(rp) ;
	    f_changed = true ;
	    f_changed = f_changed && (strncmp(ep->realname,rp,rl) == 0) ;
	    f_changed = f_changed && (ep->realname[rl] == '\0') ;
	    if (f_changed) {
	        ep->ti_init = getustime ;
	        ep->realnamelen = rl ;
		if (char *ubuf{} ; (rs = malloc_un(&ubuf)) >= 0) {
		    cint	ulen = rs ;
		    {
		        strdcpy1(ubuf,ulen,ep->username) ;
	                rs = entry_loadnames(ep,ubuf,rp,rl) ;
		    }
		    rs1 = libmem.free(ubuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (changed) */
	} /* end if (non-null) */
	return (rs >= 0) ? f_changed : rs ;
}
/* end subroutine (entry_update) */

static int entry_loadnames(NC_ENT *ep,cchar *up,cchar *rp,int rl) noex {
	int		rs ;
	int		sz = 0 ;
	if (ep->a != nullptr) {
	    libmem.free(ep->a) ;
	    ep->a = nullptr ;
	}
	sz += (lenstr(up)+1) ;
	sz += (lenstr(rp,rl)+1) ;
	if (char *bp{} ; (rs = libmem.mall(sz,&bp)) >= 0) {
	    ep->a = bp ;
	    ep->username = bp ;
	    bp = (strwcpy(bp,up,-1)+1) ;
	    ep->realname = bp ;
	    bp = (strwcpy(bp,rp,rl)+1) ;
	} /* end if (memory-allocation) */
	return rs ;
}
/* end subroutine (entry_loadnames) */

/* make a real name from a GECOS name */
static int mkaname(char *nbuf,int nlen,cchar *gecos) noex {
	cint		glen = GNAMELEN ;
	int		rs ;
	int		rs1 ;
	int		rl = 0 ;
	char		gbuf[GNAMELEN+1] ;
	nbuf[0] = '\0' ;
	if ((rs = mkgecosname(gbuf,glen,gecos)) >= 0) {
	    cint	gl = rs ;
	    if (realname rn ; (rs = rn.start(gbuf,gl)) >= 0) {
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
}
/* end subroutine (mkaname) */


