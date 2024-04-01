/* namecache SUPPORT */
/* lang=C++20 */

/* real-name cache (from UNIX® System PASSWD database) */
/* version %I% last-modified %G% */

#define	CF_FULLNAME	0		/* use fullname? */
#define	CF_PWCACHE	1		/* use |ugetpw(3uc)| */

/* revision history:

	= 2004-01-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object provides a crude cache for PASSWD-DB real-names.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<ctime>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<getax.h>
#include	<ucpwcache.h>
#include	<realname.h>
#include	<mkgecosname.h>
#include	<strwcpy.h>
#include	<strdcpy.h>
#include	<localmisc.h>		/* |READNAMELEN| */

#include	"namecache.h"


/* local defines */

#define	NC		namecache
#define	NC_ENT		namecache_ent
#define	NC_ST		namecache_st

#ifndef	GNAMELEN
#define	GNAMELEN	REALNAMELEN	/* GECOS name length */
#endif

#if	CF_PWCACHE
#define	GETPW_NAME	ucpwcache_name
#else
#define	GETPW_NAME	getpw_name
#endif /* CF_PWCACHE */

#ifndef	CF_FULLNAME
#define	CF_FULLNAME	0		/* use fullname? */
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
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
} ;


/* forward references */

template<typename ... Args>
static int namecache_ctor(namecache *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(op) ; /* dangerous */
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

int namecache_start(NC *op,cchar *varname,int nmax,int ttl) noex {
	int		rs ;
	if ((rs = namecache_ctor(op,varname)) >= 0) {
	    rs = SR_INVALID ;
	    if (varname[0]) {
	        cchar	*cp{} ;
	        if (nmax < 3) nmax = NAMECACHE_DEFMAX ;
	        if (ttl < 1) ttl = NAMECACHE_DEFTO ;
	        if ((rs = uc_mallocstrw(varname,-1,&cp)) >= 0) {
		    cnullptr	np{} ;
	            cint	ne = NAMECACHE_DEFENTS ;
	            op->varname = cp ;
	            if ((rs = hdb_start(op->dbp,ne,1,np,np)) >= 0) {
	                op->nmax = nmax ;
	                op->ttl = ttl ;
	                op->magic = NAMECACHE_MAGIC ;
	            } /* end if (hdb-start) */
	            if (rs < 0) {
		        uc_free(op->varname) ;
		        op->varname = nullptr ;
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
	    if (op->varname) {
	        rs1 = uc_free(op->varname) ;
	        if (rs >= 0) rs = rs1 ;
	        op->varname = nullptr ;
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
	        hdb_dat		val ;
	        key.buf = un ;
	        key.len = strlen(un) ;
	        if ((rs = hdb_fetch(op->dbp,key,nullptr,&val)) >= 0) {
	            NC_ENT	*ep ;
	            ep = (NC_ENT *) val.buf ;
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
	    const time_t	dt = time(nullptr) ;
	    rs = SR_INVALID ;
	    if (un[0]) {
	        ucentpw		pw ;
	        cchar		*rp = nullptr ;
	        char		*pwbuf{} ;
	        op->s.total += 1 ;
	        if ((rs = malloc_pw(&pwbuf)) >= 0) {
	            hdb_dat	key, val ;
	            NC_ENT	*ep ;
		    cint	pwlen = rs ;
	            cint	rlen = REALNAMELEN ;
	            char	rbuf[REALNAMELEN + 1] ;
	            key.buf = un ;
	            key.len = strlen(un) ;
	            if ((rs = hdb_fetch(op->dbp,key,nullptr,&val)) >= 0) {
	                ep = (NC_ENT *) val.buf ;
	                ep->ti_access = dt ;
	                if (dt > (ep->ti_init + op->ttl)) {
		            ep->ti_init = dt ;
	                    if ((rs = GETPW_NAME(&pw,pwbuf,pwlen,un)) >= 0) {
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
	                if ((rs = GETPW_NAME(&pw,pwbuf,pwlen,un)) >= 0) {
		            cchar	*gecos = pw.pw_gecos ;
	                    if ((rs = mkaname(rbuf,rlen,gecos)) >= 0) {
		                rl = rs ;
	                        if ((rs = hdb_count(op->dbp)) >= op->nmax) {
	                            rs = namecache_repent(op,&ep,un,rbuf,rl) ;
	                        } else if (rs >= 0) {
	                            rs = namecache_newent(op,&ep,un,rbuf,rl) ;
		                }
	                        if ((rs >= 0) && (ep != nullptr)) {
		                    rp = ep->realname ;
		                    rl = ep->realnamelen ;
	                        }
		            } /* end if (real-name) */
	                } /* end if (getpw_name) */
	            } /* end if (hdb_fetch) */
	            rs1 = uc_free(pwbuf) ;
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
	NC_ENT		*ep{} ;
	cint		msize = sizeof(NC_ENT) ;
	int		rs ;
	if (epp) *epp = nullptr ;
	if ((rs = uc_malloc(msize,&ep)) >= 0) {
	    if ((rs = entry_start(ep,un,sp,sl)) >= 0) {
	        hdb_dat		key ;
	        hdb_dat		val ;
	        key.buf = ep->username ;
	        key.len = strlen(ep->username) ;
	        val.buf = ep ;
	        val.len = msize ;
	        if ((rs = hdb_store(op->dbp,key,val)) >= 0) {
		    if (epp) *epp = ep ;
		} /* end if (hdb-store) */
		if (rs < 0) {
		    entry_finish(ep) ;
		}
	    } /* end if (entry-start) */
	    if (rs < 0) {
		uc_free(ep) ;
	    }
	} /* end if (memory-allocation) */
	return rs ;
}
/* end subroutine (namecache_newent) */

static int namecache_repent(NC *op,NC_ENT **epp,cc *un,cc *sp,int sl) noex {
	NC_ENT		*ep = nullptr ;
	hdb		*dbp = op->dbp ;
	hdb_cur		cur ;
	hdb_dat		key ;
	hdb_dat		val ;
	int		rs ;
	int		rs1 ;
	if (epp) *epp = nullptr ;
	if ((rs = hdb_curbegin(dbp,&cur)) >= 0) {
	    while ((rs = hdb_enum(dbp,&cur,&key,&val)) >= 0) {
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
	hdb_cur		cur ;
	hdb_dat		key ;
	hdb_dat		val ;
	int		rs = SR_OK ;
	int		rs1 ;
	if ((rs1 = hdb_curbegin(elp,&cur)) >= 0) {
	    while (hdb_enum(elp,&cur,&key,&val) >= 0) {
	        NC_ENT	*ep = (NC_ENT *) val.buf ;
		{
	            rs1 = entry_finish(ep) ;
	            if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = uc_free(ep) ;
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
	const time_t	dt = time(nullptr) ;
	int		rs = SR_FAULT ;
	if (ep && up && rp) {
	    rs = SR_INVALID ;
	    memclear(ep) ;
	    if (up[0]) {
	        if (rl < 0) rl = strlen(rp) ;
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
	        rs1 = uc_free(ep->a) ;
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
	int		f_changed = true ;
	if (ep && rp) {
	    rs = SR_OK ;
	    if (rl < 0) rl = strlen(rp) ;
	    f_changed = f_changed && (strncmp(ep->realname,rp,rl) == 0) ;
	    f_changed = f_changed && (ep->realname[rl] == '\0') ;
	    if (f_changed) {
		char	*ubuf{} ;
	        ep->ti_init = time(nullptr) ;
	        ep->realnamelen = rl ;
	        if ((rs = malloc_un(&ubuf)) >= 0) {
		    cint	ulen = rs ;
		    {
		        strdcpy1(ubuf,ulen,ep->username) ;
	                rs = entry_loadnames(ep,ubuf,rp,rl) ;
		    }
		    rs1 = uc_free(ubuf) ;
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
	char		*bp ;
	if (ep->a != nullptr) {
	    uc_free(ep->a) ;
	    ep->a = nullptr ;
	}
	sz += (strlen(up)+1) ;
	sz += (strnlen(rp,rl)+1) ;
	if ((rs = uc_malloc(sz,&bp)) >= 0) {
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
	    realname	rn ;
	    cint	gl = rs ;
	    if ((rs = realname_start(&rn,gbuf,gl)) >= 0) {
		if constexpr (f_fullname) {
	            rl = realname_fullname(&rn,nbuf,nlen) ;
		} else {
	            rl = realname_name(&rn,nbuf,nlen) ;
		} /* end if-constexpr (f_fullanem) */
	        rs1 = realname_finish(&rn) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (realname) */
	} /* end if (mkgeoxname) */
	return (rs >= 0) ? rl : 0 ;
}
/* end subroutine (mkaname) */


