/* envs SUPPORT */
/* lang=C++20 */

/* environment-variable list container */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	Module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object just provides a hash list of environment-like
	variables (with a key and an associated value).

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstdlib>
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<usystem.h>
#include	<hdb.h>
#include	<vecstr.h>
#include	<nulstr.h>
#include	<localmisc.h>

#include	"envs.h"


/* local defines */

#define	ENVS_DB			hdb
#define	ENVS_ENT		struct envs_e
#define	ENVS_DEFENTS		10

#define	ENVS_DBSTART(op,n,hash,cmp)	hdb_start((op),(n),1,(hash),(cmp))
#define	ENVS_DBSTORE(op,k,v)		hdb_store((op),(k),(v))
#define	ENVS_DBGETREC(op,cp,kp,vp)	hdb_getrec((op),(cp),(kp),(vp))
#define	ENVS_DBNEXT(op,cp)		hdb_next((op),(cp))
#define	ENVS_DBENUM(op,cp,kp,vp)	hdb_enum((op),(cp),(kp),(vp))
#define	ENVS_DBCURBEGIN(op,cp)		hdb_curbegin((op),(cp))
#define	ENVS_DBCUREND(op,cp)		hdb_curend((op),(cp))
#define	ENVS_DBFETCH(op,k,cp,vp)	hdb_fetch((op),(k),(cp),(vp))
#define	ENVS_DBCOUNT(op)		hdb_count((op))
#define	ENVS_DBFINISH(op)		hdb_finish((op))

#define	ENVS_DBDAT			hdb_dat
#define	ENVS_DBCUR			hdb_cur


/* local namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct envs_e {
	cchar		*kp ;
	int		kl ;
	vecstr		elist ;
} ;

typedef envs_e		*entp ;


/* forward references */

static inline int envs_enter(envs *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    hdb		*p ;
	    rs = SR_NOMEM ;
	    if ((p = new(nothrow) hdb) != nullptr) {
		op->varp = p ;
		rs = SR_OK ;
	    }
	} /* end if (non-null) */
	return rs ;
}

template<typename ... Args>
static inline int envs_magic(envs *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == ENVS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}

static int	entry_start(ENVS_ENT *,cchar *,cchar *,int,cchar **) noex ;
static int	entry_count(ENVS_ENT *) noex ;
static int	entry_set(ENVS_ENT *,cchar *,int) noex ;
static int	entry_append(ENVS_ENT *,cchar *,int) noex ;
static int	entry_get(ENVS_ENT *,int,cchar **) noex ;
static int	entry_substr(ENVS_ENT *,cchar *,int) noex ;
static int	entry_finish(ENVS_ENT *) noex ;


/* local variables */


/* exported subroutines */

int envs_start(envs *op,int ne) noex {
	int		rs ;
	if ((rs = envs_enter(op)) >= 0) {
	    const nullptr_t	np{} ;
	    if ((rs = ENVS_DBSTART(op->varp,ne,np,np)) >= 0) {
		op->magic = ENVS_MAGIC ;
	    }
	}
	return rs ;
}
/* end subroutine (envs_start) */

int envs_finish(envs *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = envs_magic(op)) >= 0) {
	    ENVS_DBCUR		cur ;
	    ENVS_DBDAT		key, val ;
	    ENVS_DBCURBEGIN(op->varp,&cur) ;
	    while (ENVS_DBENUM(op->varp,&cur,&key,&val) >= 0) {
	        ENVS_ENT	*ep = (ENVS_ENT *) val.buf ;
		if (ep) {
		    {
			rs1 = entry_finish(ep) ;
			if (rs >= 0) rs = rs1 ;
		    }
		    {
			rs1 = uc_free(ep) ;
			if (rs >= 0) rs = rs1 ;
		    }
		}
	    } /* end while */
	    {
	        rs1 = ENVS_DBCUREND(op->varp,&cur) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    if (op->varp) {
		rs1 = ENVS_DBFINISH(op->varp) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		delete op->varp ;
		op->varp = nullptr ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (envs_finish) */

int envs_store(envs *op,cchar *kp,int fa,cchar *vp,int vl) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if ((rs = envs_magic(op,kp,vp)) >= 0) {
	    ENVS_DBDAT	key, val ;
	    ENVS_ENT	*ep{} ;
	    key.buf = kp ;
	    key.len = strlen(kp) ;
	    val.buf = nullptr ;
	    val.len = -1 ;
	    if ((rs1 = ENVS_DBFETCH(op->varp,key,nullptr,&val)) >= 0) {
	        ep = (ENVS_ENT *) val.buf ;
	        if (fa) {
		    rs = entry_append(ep,vp,vl) ;
	        } else {
		    rs = entry_set(ep,vp,vl) ;
	        }
	    } else if (rs1 == SR_NOTFOUND) {
	        cint	esize = sizeof(ENVS_ENT) ;
	        int		knl ;
	        cchar	*knp ;
	        if ((rs = uc_malloc(esize,&ep)) >= 0) {
		    if ((rs = entry_start(ep,kp,vp,vl,&knp)) >= 0) {
		        knl = rs ;
		        key.buf = knp ;
		        key.len = knl ;
		        val.buf = ep ;
		        val.len = sizeof(ENVS_ENT) ;
		        rs = ENVS_DBSTORE(op->varp,key,val) ;
		        if (rs < 0) {
			    entry_finish(ep) ;
		        }
		    } /* end if */
		    if (rs < 0) {
		        uc_free(ep) ;
		    }
	        } /* end if (allocated entry) */
	    } else {
	        rs = rs1 ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (envs_add) */

int envs_count(envs *op) noex {
	int		rs ;
	if ((rs = envs_magic(op)) >= 0) {
	    rs = ENVS_DBCOUNT(op->varp) ;
	}
	return rs ;
}
/* end subroutine (envs_count) */

int envs_present(envs *op,cchar *kp,int kl) noex {
	int		rs ;
	if ((rs = envs_magic(op,kp)) >= 0) {
	    ENVS_DBDAT	key, val ;
	    if (kl < 0) kl = strlen(kp) ;
	    key.buf = kp ;
	    key.len = kl ;
	    if ((rs = ENVS_DBFETCH(op->varp,key,nullptr,&val)) >= 0) {
	        ENVS_ENT	*ep = (ENVS_ENT *) val.buf ;
	        rs = entry_count(ep) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (envs_present) */

int envs_substr(envs *op,cchar *kp,int kl,cchar *sp,int sl) noex {
	int		rs ;
	if ((rs = envs_magic(op,kp,sp)) >= 0) {
	    hdb		*vlp = op->varp ;
	    ENVS_DBDAT	key, val ;
	    if (kl < 0) kl = strlen(kp) ;
	    if (sl < 0) sl = strlen(sp) ;
	    key.buf = kp ;
	    key.len = kl ;
	    if ((rs = ENVS_DBFETCH(vlp,key,nullptr,&val)) >= 0) {
	        ENVS_ENT	*ep = (ENVS_ENT *) val.buf ;
	        rs = entry_substr(ep,sp,sl) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (envs_substr) */

int envs_curbegin(envs *op,ENVS_CUR *curp) noex {
	int		rs ;
	if ((rs = envs_magic(op,curp)) >= 0) {
	    cint	osz = sizeof(hdb_cur) ;
	    hdb_cur	*ocp{} ;
	    curp->i = -1 ;
	    if ((rs = uc_malloc(osz,&ocp)) >= 0) {
		curp->curp = ocp ;
	        rs = ENVS_DBCURBEGIN(op->varp,curp->curp) ;
		if (rs < 0) {
		    uc_free(curp->curp) ;
		    curp->curp = nullptr ;
		}
	    } /* end if (m-a) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (envs_curbegin) */

int envs_curend(envs *op,ENVS_CUR *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = envs_magic(op,curp)) >= 0) {
	    {
	        rs1 = ENVS_DBCUREND(op->varp,curp->curp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = uc_free(curp->curp) ;
		if (rs >= 0) rs = rs1 ;
		curp->curp = nullptr ;
	    }
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (envs_curend) */

int envs_enumkey(envs *op,ENVS_CUR *curp,cchar **kpp) noex {
	int		rs ;
	int		kl = 0 ;
	if ((rs = envs_magic(op,curp)) >= 0) {
	    ENVS_DBDAT	key, val ;
	    ENVS_DB	*dlp = op->varp ;
	    if (kpp) *kpp = nullptr ;
	    if ((rs = ENVS_DBENUM(dlp,curp->curp,&key,&val)) >= 0) {
	        cchar	*kp = ccharp(key.buf) ;
	        kl = strlen(kp) ;
	        if (kpp) *kpp = kp ;
	    } /* end if */
	} /* end if (magic) */
	return (rs >= 0) ? kl : rs ;
}
/* end subroutine (envs_enumkey) */

int envs_enum(envs *op,ENVS_CUR *curp,cchar **kpp,cchar **vpp) noex {
	int		rs ;
	int		kl = 0 ;
	if ((rs = envs_magic(op,curp)) >= 0) {
	    ENVS_DBDAT	key, val ;
	    ENVS_ENT	*ep ;
	    cchar	*kp{}, *vp{} ;
	    if (kpp) *kpp = nullptr ;
	    if (vpp) *vpp = nullptr ;
	    while (rs >= 0) {
	       int		i ;
	       if (curp->i < 0) {
	            rs = ENVS_DBNEXT(op->varp,curp->curp) ;
	        }
	        if (rs >= 0) {
	            rs = ENVS_DBGETREC(op->varp,curp->curp,&key,&val) ;
	        }
	        if (rs < 0) break ;
	        kp = ccharp(key.buf) ;
	        kl = strlen(kp) ;
	        if (kpp) *kpp = kp ;
	        ep = (ENVS_ENT *) val.buf ;
	        i = (curp->i >= 0) ? (curp->i + 1) : 0 ;
	        if ((rs = entry_get(ep,i,&vp)) >= 0) {
		    curp->i = i ;
		    if (vpp) *vpp = vp ;
		    break ;
	        } else if (rs == SR_NOTFOUND) {
		    curp->i = -1 ;
	        } /* end if */
	    } /* end while */
	} /* end if (magic) */
	return (rs >= 0) ? kl : rs ;
}
/* end subroutine (envs_enum) */

int envs_fetch(envs *op,cc *kp,int kl,ENVS_CUR *curp,cc **rpp) noex {
	int		rs ;
	int		vl = 0 ;
	if ((rs = envs_magic(op,kp,curp)) >= 0) {
	    ENVS_DBDAT	key, val ;
	    ENVS_ENT	*ep ;
	    int		i = 0 ;
	    if (kl < 0) kl = strlen(kp) ;
	    if (rpp) *rpp = nullptr ;
	    if (curp) {
	        i = (curp->i >= 0) ? (curp->i + 1) : 0 ;
	    }
	    key.buf = kp ;
	    key.len = kl ;
	    if ((rs = ENVS_DBFETCH(op->varp,key,nullptr,&val)) >= 0) {
	        cchar	*vp{} ;
	        ep = (ENVS_ENT *) val.buf ;
	        if ((rs = entry_get(ep,i,&vp)) >= 0) {
		    vl = rs ;
		    if (curp) curp->i = i ;
		    if (rpp) *rpp = vp ;
	        }
	    } /* end if */
	} /* end if (magic) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (envs_fetch) */

int envs_delname(envs *op,cchar *kp,int kl) noex {
	int		rs ;
	if ((rs = envs_magic(op,kp)) >= 0) {
	    ENVS_DBDAT	key, val ;
	    if (kl < 0) kl = strlen(kp) ;
	    key.buf = kp ;
	    key.len = kl ;
	    rs = ENVS_DBFETCH(op->varp,key,nullptr,&val) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (envs_delname) */


/* private subroutines */

static int entry_start(ENVS_ENT *ep,cc *kp,cc *vn,int vnlen,cc**rpp) noex {
	int		rs = SR_FAULT ;
	int		kl = 0 ;
	if (ep && kp) {
	    rs = SR_INVALID ;
	    if (kp[0]) {
	        cchar		*cp{} ;
	        if ((rs = uc_mallocstrw(kp,-1,&cp)) >= 0) {
	            cint	n = ENVS_DEFENTS ;
	            int		vo = 0 ;
	            ep->kl = rs ;
	            ep->kp = cp ;
	            vo |= (VECSTR_OCOMPACT | VECSTR_OORDERED) ;
	            vo |= VECSTR_OREUSE ;
	            if ((rs = vecstr_start(&ep->elist,n,vo)) >= 0) {
		        if (vn == nullptr) vn = (ep->kp + ep->kl) ;
		        if ((rs = vecstr_add(&ep->elist,vn,vnlen)) >= 0) {
		            kl = ep->kl ;
		            if (rpp) *rpp = ep->kp ;
		        }
		        if (rs < 0) {
		            vecstr_finish(&ep->elist) ;
			}
	            } /* end if (vecstr_start) */
	            if (rs < 0) {
		        uc_free(ep->kp) ;
		        ep->kp = nullptr ;
	            }
	        } /* end if (memory-allocation) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? kl : rs ;
}
/* end subroutine (entry_start) */

static int entry_finish(ENVS_ENT *ep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (ep) {
	    rs = SR_OK ;
	    if (ep->kp) {
		rs1 = uc_free(ep->kp) ;
		if (rs >= 0) rs = rs1 ;
		ep->kp = nullptr ;
	    }
	    {
		rs1 = vecstr_finish(&ep->elist) ;
		if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entry_finish) */

static int entry_count(ENVS_ENT *ep) noex {
	return vecstr_count(&ep->elist) ;
}
/* end subroutine (entry_count) */

static int entry_set(ENVS_ENT *ep,cchar *vp,int vl) noex {
	int		rs ;
	if ((rs = vecstr_delall(&ep->elist)) >= 0) {
	    rs = vecstr_add(&ep->elist,vp,vl) ;
	}
	return rs ;
}
/* end subroutine (entry_set) */

static int entry_append(ENVS_ENT *ep,cchar *vp,int vl) noex {
	return vecstr_add(&ep->elist,vp,vl) ;
}
/* end subroutine (entry_append) */

static int entry_get(ENVS_ENT *ep,int i,cchar **rpp) noex {
	int		rs ;
	int		vl = 0 ;
	cchar		*rp{} ;
	if ((rs = vecstr_get(&ep->elist,i,&rp)) >= 0) {
	    vl = strlen(rp) ;
	    if (*rpp) *rpp = rp ;
	}
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (entry_get) */

static int entry_substr(ENVS_ENT *ep,cchar *sp,int sl) noex {
	nulstr		ns ;
	int		rs ;
	int		rs1 ;
	int		f = false ;
	cchar		*ss ;
	if ((rs = nulstr_start(&ns,sp,sl,&ss)) >= 0) {
	    vecstr	*clp = &ep->elist ;
	    cchar	*cp{} ;
	    for (int i = 0 ; (rs = vecstr_get(clp,i,&cp)) >= 0 ; i += 1) {
		if (cp) {
		    f = (strstr(cp,ss) != nullptr) ;
		    if (f) break ;
		}
	    } /* end for */
	    if (rs == SR_NOTFOUND) rs = SR_OK ;
	    rs1 = nulstr_finish(&ns) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (nulstr) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (entry_substr) */


