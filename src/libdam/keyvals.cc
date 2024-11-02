/* keyvals SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* key-values (in-memory only) operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-01, David A­D­ Morano
	This object was originally written.

	- 2004-05-25, David A­D­ Morano
	This subroutine was adopted for use as a general key-value
	file reader.

*/

/* Copyright © 1998,2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	keyvals

	Description:
	This object manages in-memory key-value pairs.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t(3c++)| */
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<vecobj.h>
#include	<hdb.h>
#include	<hash.h>
#include	<localmisc.h>

#include	"keyvals.h"


/* local defines */

#define	KEYVALS_KEY	keyvals_key
#define	KEYVALS_ENT	keyvals_entry

#define	KV		keyvals
#define	KEY		keyvals_key
#define	CUR		keyvals_cur
#define	ENT		keyvals_entry


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct keyvals_key {
	cchar		*kp ;
	int		kl ;
	int		count ;
} ;

typedef keyvals_key *	keyp ;

struct keyvals_entry {
	KEYVALS_KEY	*kep ;
	cchar		*vname ;
	int		vlen ;
	int		fi ;		/* file index */
	int		ki ;		/* key index */
} ;

typedef keyvals_entry *		entp ;


/* forward references */

template<typename ... Args>
static int keyvals_ctor(keyvals *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    op->magic = 0 ;
	    if ((op->keyp = new(nothrow) vecobj) != np) {
		if ((op->bykeyp = new(nothrow) hdb) != np) {
		    if ((op->bykeyvalp = new(nothrow) hdb) != np) {
			rs = SR_OK ;
		    } /* end if (new-hdb) */
		    if (rs < 0) {
		        delete op->bykeyp ;
		        op->bykeyp = nullptr ;
		    }
		} /* end if (new-hdb) */
		if (rs < 0) {
		    delete op->keyp ;
		    op->keyp = nullptr ;
		}
	    } /* end if (new-vecobj) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (keyvals_ctor) */

static int keyvals_dtor(keyvals *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->bykeyvalp) {
		delete op->bykeyvalp ;
		op->bykeyvalp = nullptr ;
	    }
	    if (op->bykeyp) {
		delete op->bykeyp ;
		op->bykeyp = nullptr ;
	    }
	    if (op->keyp) {
		delete op->keyp ;
		op->keyp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (keyvals_dtor) */

template<typename ... Args>
static inline int keyvals_magic(keyvals *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == KEYVALS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (keyvals_magic) */

static int	keyvals_keyadd(keyvals *,KEY *,KEY **) noex ;
static int	keyvals_keyfins(keyvals *) noex ;
static int	keyvals_keyget(keyvals *,cchar *,KEY **) noex ;
static int	keyvals_keydel(keyvals *,int) noex ;
static int	keyvals_already(keyvals *,ENT *) noex ;
static int	keyvals_addentry(keyvals *,ENT *) noex ;
static int	keyvals_entfins(keyvals *) noex ;

static int	key_start(KEY *,cchar *) noex ;
static int	key_increment(KEY *) noex ;
static int	key_decrement(KEY *) noex ;
static int	key_finish(KEY *) noex ;
static int	key_mat(KEY *,cchar *,int) noex ;

static int	entry_start(ENT *,int,int,KEY *,cchar *,int) noex ;
static int	entry_matkey(ENT *,cchar *,int) noex ;
static int	entry_finish(ENT *) noex ;

#ifdef	COMMENT
typedef uint		(*hdbhash_f)(cvoid *,int) noex ;
typedef int		(*hdbcmp_f)(cvoid *,cvoid *,int) noex ;
#endif /* COMMENT */

extern "C" {
    static uint	hashent(cvoid *,int) noex ;
    static int	cmpent(cvoid *,cvoid *,int) noex ;
}

static uint	hashente(const ENT *,int) noex ;

extern "C" {
    static int	vcmpkey(cvoid **,cvoid **) noex ;
}

static int	cmpente(const ENT *,const ENT *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int keyvals_start(keyvals *op,int ndef) noex {
	int		rs ;
	if (ndef < KEYVALS_DEFENTS) ndef = KEYVALS_DEFENTS ;
	if ((rs = keyvals_ctor(op)) >= 0) {
	    cnullptr	np{} ;
	    cint	sz = sizeof(KEYVALS_KEY) ;
	    cint	vn = (ndef / 10) ;
	    cint	vo = (VECOBJ_OSTATIONARY | VECOBJ_OREUSE) ;
	    if ((rs = vecobj_start(op->keyp,sz,vn,vo)) >= 0) {
	        if ((rs = hdb_start(op->bykeyp,ndef,0,np,np)) >= 0) {
	            auto	hk = hashent ;
	            auto	vcmp = cmpent ;
	            if ((rs = hdb_start(op->bykeyvalp,ndef,0,hk,vcmp)) >= 0) {
	                op->magic = KEYVALS_MAGIC ;
	            }
		    if (rs < 0) {
	    	        hdb_finish(op->bykeyp) ;
		    }
	        } /* end if (hdb_start) */
	        if (rs < 0) {
		    vecobj_finish(op->keyp) ;
	        }
	    } /* end if (vecobj_start) */
	    if (rs < 0) {
		keyvals_dtor(op) ;
	    }
	} /* end if (keyvals_ctor) */
	return rs ;
}
/* end subroutine (keyvals_start) */

int keyvals_finish(keyvals *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = keyvals_magic(op)) >= 0) {
	    {
	        rs1 = keyvals_entfins(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = keyvals_keyfins(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = hdb_finish(op->bykeyvalp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = hdb_finish(op->bykeyp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = vecobj_finish(op->keyp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs = keyvals_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (keyvals_finish) */

/* add a key-value pair */
int keyvals_add(keyvals *op,int fi,cc *kp,cc *vp,int vl) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		rs1 ;
	int		cadd = 0 ;
	if ((rs = keyvals_magic(op,kp,vp)) >= 0) {
	    rs = SR_INVALID ;
	    if (*kp) {
	        KEY	*kep = nullptr ;
	        if (vl < 0) vl = strlen(vp) ;
	        if ((rs = keyvals_keyget(op,kp,&kep)) >= 0) {
	            KEYVALS_ENT	ve ;
	            cint	ki = rs ;
	            bool	f = true ;
	            if ((rs = entry_start(&ve,fi,ki,kep,vp,vl)) >= 0) {
	                if ((rs = keyvals_already(op,&ve)) == rsn) {
	            	    if ((rs = keyvals_addentry(op,&ve)) >= 0) {
	                         f = false ;
	                         cadd += 1 ;
	                     }
	                } /* end if (new entry) */
	                if (f) {
	                    rs1 = entry_finish(&ve) ;
		            if (rs1 != INT_MAX) {
			        rs1 = keyvals_keydel(op,rs1) ;
			        if (rs >= 0) rs = rs1 ;
		            }
		        } /* end if (true) */
	            } /* end if (entry initialized) */
	        } /* end if (keyvals_keyget) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? cadd : rs ;
}
/* end subroutine (keyvals_add) */

int keyvals_count(keyvals *op) noex {
	int		rs ;
	if ((rs = keyvals_magic(op)) >= 0) {
	    rs = hdb_count(op->bykeyp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (keyvals_count) */

int keyvals_curbegin(keyvals *op,keyvals_cur *curp) noex {
	int		rs ;
	if ((rs = keyvals_magic(op,curp)) >= 0) {
	    cint	osz = sizeof(hdb_cur) ;
	    curp->i = -1 ;
	    if (void *vp{} ; (rs = uc_malloc(osz,&vp)) >= 0) {
		curp->ecp = (hdb_cur *) vp ;
	        rs = hdb_curbegin(op->bykeyp,curp->ecp) ;
		if (rs < 0) {
		    uc_free(curp->ecp) ;
		    curp->ecp = nullptr ;
		}
	    } /* end if (memory-allocation) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (keyvals_curbegin) */

int keyvals_curend(keyvals *op,keyvals_cur *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = keyvals_magic(op,curp)) >= 0) {
	    rs = SR_FAULT ;
	    if (curp->ecp) {
	        curp->i = -1 ;
		{
	            rs1 = hdb_curend(op->bykeyp,curp->ecp) ;
		    if (rs >= 0) rs = rs1 ;
		}
		{
		    rs1 = uc_free(curp->ecp) ;
		    if (rs >= 0) rs = rs1 ;
		}
		curp->ecp = nullptr ;
	    } /* end if (cursor-valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (keyvals_curend) */

int keyvals_curenumkey(keyvals *op,CUR *curp,cchar **kpp) noex {
	int		rs ;
	int		kl = 0 ;
	if ((rs = keyvals_magic(op,curp)) >= 0) {
	    KEY		*kep = nullptr ;
	    int		oi = (curp->i >= 0) ? (curp->i + 1) : 0 ;
	    cchar	*kp = nullptr ;
	    void	*vp{} ;
	    while ((rs = vecobj_get(op->keyp,oi,&vp)) >= 0) {
	        KEY	*kep = keyp(vp) ;
	        if (kep) break ;
	        oi += 1 ;
	    } /* end while */
	    if (rs >= 0) {
	        kp = kep->kp ;
	        kl = kep->kl ;
	        curp->i = oi ;
	    }
	    if (kpp) {
		*kpp = kp ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? kl : rs ;
}
/* end subroutine (keyvals_curenumkey) */

int keyvals_curenum(keyvals *op,CUR *curp,cchar **kpp,cchar **vpp) noex {
	int		rs ;
	int		kl = 0 ;
	if ((rs = keyvals_magic(op,curp)) >= 0) {
	    int		rs = SR_BUGCHECK ;
	    if (curp->ecp) {
	        ENT		*ep{} ;
	        hdb_dat		key ;
	        hdb_dat		val ;
	        cchar		*kp = nullptr ;
	        cchar		*valp = nullptr ;
	        if ((rs = hdb_curenum(op->bykeyp,curp->ecp,&key,&val)) >= 0) {
	            {
	                kp = charp(key.buf) ;
	                kl = key.len ;
	            }
	            {
	                ep = (ENT *) val.buf ;
	                valp = ep->vname ;
	            }
	        } /* end if (had an entry) */
	        if (kpp) {
	            *kpp = kp ;
	        }
	        if (vpp) {
	            *vpp = valp ;
	        }
	    } /* end if (cursor-valid) */
	} /* end if (magic) */
	return (rs >= 0) ? kl : rs ;
}
/* end subroutine (keyvals_curenum) */

int keyvals_fetch(keyvals *op,cchar *kp,CUR *curp,cchar **vpp) noex {
	int		rs ;
	int		vl = 0 ;
	if ((rs = keyvals_magic(op,kp,curp)) >= 0) {
	    int		rs = SR_BUGCHECK ;
	    if (curp->ecp) {
	        hdb_dat		key ;
	        hdb_dat		val ;
	        int		kl ;
	        cchar		*valp = nullptr ;
	        if (kp[0] == '\0') kp = "default" ;
	        kl = strlen(kp) ;
	        key.buf = kp ;
	        key.len = kl ;
	        if ((rs = hdb_fetch(op->bykeyp,key,curp->ecp,&val)) >= 0) {
	            ENT		*ep = entp(val.buf) ;
	            valp = ep->vname ;
	            vl = ep->vlen ;
	        } /* end if (had an entry) */
	        if (vpp) {
	            *vpp = valp ;
	        }
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (keyvals_fetch) */

int keyvals_delset(keyvals *op,int fi) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = keyvals_magic(op)) >= 0) {
	    hdb_cur	cur ;
	    hdb_dat	key ;
	    hdb_dat	val ;
	    ENT		*ep ;
	    if ((rs = hdb_curbegin(op->bykeyp,&cur)) >= 0) {
	        while (hdb_curenum(op->bykeyp,&cur,&key,&val) >= 0) {
	            ep = entp(val.buf) ;
	            if ((ep->fi == fi) || (fi < 0)) {
		        c += 1 ;
	                hdb_delcur(op->bykeyp,&cur,0) ;
	            } /* end if (found matching entry) */
	        } /* end while (looping through entries) */
	        rs1 = hdb_curend(op->bykeyp,&cur) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (cursor) */
	    if (rs >= 0) {
	        if ((rs = hdb_curbegin(op->bykeyvalp,&cur)) >= 0) {
	            while (hdb_curenum(op->bykeyvalp,&cur,&key,&val) >= 0) {
	                ep = entp(val.buf) ;
	                if ((ep->fi == fi) || (fi < 0)) {
	                    hdb_delcur(op->bykeyvalp,&cur,0) ;
		            entry_finish(ep) ;
		            uc_free(ep) ;
	                } /* end if (key-match) */
	            } /* end while (looping through entries) */
	            rs1 = hdb_curend(op->bykeyvalp,&cur) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (cursor) */
	    } /* end if (ok) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (keyvals_delset) */

int keyvals_delkey(keyvals *op,cchar *kp,int kl) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = keyvals_magic(op,kp)) >= 0) {
	    hdb		*bykeyp = op->bykeyp ;
	    hdb_cur	cur ;
	    hdb_dat	key ;
	    hdb_dat	val ;
	    int		ki = 0 ;
	    /* delete all keyvals w/ this key */
	    if ((rs = hdb_curbegin(bykeyp,&cur)) >= 0) {
	        key.buf = kp ;
	        key.len = strlen(kp) ;
	        while ((rs1 = hdb_fetch(bykeyp,key,&cur,&val)) >= 0) {
		    c += 1 ;
	            hdb_delcur(bykeyp,&cur,0) ;
	        } /* end while */
		if (rs1 != rsn) rs = rs1 ;
	        hdb_curend(bykeyp,&cur) ;
	    } /* end if (cursor) */
	    if (rs >= 0) {
	        hdb	*bykeyvalp = op->bykeyvalp ;
	        if ((rs = hdb_curbegin(bykeyvalp,&cur)) >= 0) {
	            KEYVALS_ENT		*ep ;
	            while ((rs1 = hdb_curenum(bykeyvalp,&cur,&key,&val)) >= 0) {
	                ep = entp(val.buf) ;
	                if ((rs = entry_matkey(ep,kp,kl)) >= 0) {
	                    hdb_delcur(bykeyvalp,&cur,0) ;
		            entry_finish(ep) ;
		            uc_free(ep) ;
			} else if (rs == rsn) {
			    rs = SR_OK ;
	                } /* end if (key-match) */
	            } /* end while (looping through entries) */
		    if (rs1 != rsn) rs = rs1 ;
	            hdb_curend(bykeyvalp,&cur) ;
	        } /* end if (cursor) */
	    } /* end if (ok) */
	    /* delete the key from the key-store */
	    if ((rs >= 0) && (ki >= 0)) {
	        rs = vecobj_del(op->keyp,ki) ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (keyvals_delkey) */


/* private subroutines */

static int keyvals_keyadd(keyvals *op,KEY *kep,KEY **rpp) noex {
	int		rs ;
	int		ki = INT_MAX ;
	if ((rs = vecobj_add(op->keyp,kep)) >= 0) {
	    ki = rs ;
	    if (void *vp{} ; (rs = vecobj_get(op->keyp,ki,&vp)) >= 0) {
		*rpp = keyp(vp) ;
	    }
	}
	return (rs >= 0) ? ki : rs ;
}
/* end subroutine (keyvals_keyadd) */

static int keyvals_keyfins(keyvals *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	void		*vp{} ;
	for (int i = 0 ; vecobj_get(op->keyp,i,&vp) >= 0 ; i += 1) {
	    if (vp) {
		KEY	*kep = keyp(vp) ;
	        rs1 = key_finish(kep) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */
	return rs ;
}
/* end subroutine (keyvals_keyfins) */

static int keyvals_keyget(keyvals *op,cchar *keybuf,KEY **kpp) noex {
	KEYVALS_KEY	ke ;
	KEYVALS_KEY	*kep = nullptr ;
	int		rs ;
	int		ki = 0 ;
	if ((rs = key_start(&ke,keybuf)) >= 0) {
	    bool	f = true ;
	    void	*vp{} ;
	    if ((rs = vecobj_search(op->keyp,&ke,vcmpkey,&vp)) >= 0) {
		kep = keyp(vp) ;
	        ki = rs ;
	    } else if (rs == SR_NOTFOUND) {
		if ((rs = keyvals_keyadd(op,&ke,&kep)) >= 0) {
		    f = false ;
	    	    ki = rs ;
		}
	    } 
	    if (f) key_finish(&ke) ;
	} /* end if (needed to enter new key) */
	if (kpp) {
	    *kpp = (rs >= 0) ? kep : nullptr ;
	}
	return (rs >= 0) ? ki : rs ;
}
/* end subroutine (keyvals_keyget) */

static int keyvals_keydel(keyvals *op,int ki) noex {
	return vecobj_del(op->keyp,ki) ;
}
/* end subroutine (keyvals_keydel) */

static int keyvals_already(keyvals *op,ENT *nep) noex {
	hdb_dat		key ;
	hdb_dat		val ;
	int		rs ;
	key.buf = nep ;
	key.len = sizeof(KEYVALS_ENT) ;
	rs = hdb_fetch(op->bykeyvalp,key,nullptr,&val) ;
	return rs ;
}
/* end subroutine (keyvals_already) */

/* add an entry to entry list */
static int keyvals_addentry(keyvals *op,ENT *nep) noex {
	int		rs ;
	int		rs1 ;
	int		sz = sizeof(KEYVALS_ENT) ;
	if (void *vp{} ; (rs = uc_malloc(sz,&vp)) >= 0) {
	    ENT		*ep = entp(vp) ;
	    KEY		*kep ;
	    hdb_dat	key ;
	    hdb_dat	val ;
	    *ep = *nep ; /* copy */
	    kep = ep->kep ;
	    {
	        key.buf = kep->kp ;
	        key.len = kep->kl ;
	    }
	    {
	        val.buf = ep ;
	        val.len = sizeof(KEYVALS_ENT) ;
	    }
	    if ((rs = hdb_store(op->bykeyp,key,val)) >= 0) {
		key.buf = ep ;
		key.len = sizeof(KEYVALS_ENT) ;
	        rs = hdb_store(op->bykeyvalp,key,val) ;
		if (rs < 0) {
	    	    hdb_cur	cur ;
	    	    hdb_curbegin(op->bykeyp,&cur) ;
	    	    {
	        	if ((rs1 = hdb_fetch(op->bykeyp,key,&cur,&val)) >= 0) {
	            	    hdb_delcur(op->bykeyp,&cur,0) ;
			}
	    	    }
	    	    hdb_curend(op->bykeyp,&cur) ;
		} /* end if (error) */
	    } /* end if (hdb_store) */
	    if (rs < 0) {
		uc_free(ep) ;
	    }
	} /* end if (memory-allocation) */
	return rs ;
}
/* end subroutine (keyvals_addentry) */

static int keyvals_entfins(keyvals *op) noex {
	hdb		*elp = op->bykeyvalp ;
	hdb_cur		cur ;
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		rs1 ;
	int		rs2 ;
	if ((rs = hdb_curbegin(elp,&cur)) >= 0) {
	    hdb_dat	key ;
	    hdb_dat	val ;
	    while ((rs2 = hdb_curenum(elp,&cur,&key,&val)) >= 0) {
	        ENT	*ep = entp(val.buf) ;
		{
	            rs1 = entry_finish(ep) ;
		    if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = uc_free(ep) ;
		    if (rs >= 0) rs = rs1 ;
		}
	    } /* end while */
	    if ((rs >= 0) && (rs2 != rsn)) rs = rs2 ;
	    rs1 = hdb_curend(elp,&cur) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (cursor) */
	return rs ;
}
/* end subroutine (keyvals_entfins) */

static int key_start(KEY *kep,cchar *kname) noex {
	int		rs = SR_FAULT ;
	int		kl = 0 ;
	if (kep && kname) {
	    rs = memclear(kep) ;
	    if (kname) {
	        kl = strlen(kname) ;
	        if (cchar *kp{} ; (rs = uc_mallocstrw(kname,kl,&kp)) >= 0) {
		    kep->kp = kp ;
	            kep->kl = kl ;
	        }
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? kl : rs ;
}
/* end subroutine (key_start) */

static int key_increment(KEY *kep) noex {
	int		rs = SR_FAULT ;
	if (kep) {
	    rs = SR_OK ;
	    kep->count += 1 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (key_increment) */

static int key_decrement(KEY *kep) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (kep) {
	    rs = SR_OK ;
	    if (kep->count > 0) {
	        kep->count -= 1 ;
	    }
#ifdef	COMMENT
	    if ((kep->count == 0) && (kep->kp != nullptr)) {
	        uc_free(kep->kp) ;
	        kep->kp = nullptr ;
	        kep->kl = 0 ;
	    }
#endif /* COMMENT */
	    c = kep->count ;
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (key_decrement) */

static int key_finish(KEY *kep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (kep->kp) {
	    rs1 = uc_free(kep->kp) ;
	    if (rs >= 0) rs = rs1 ;
	    kep->kp = nullptr ;
	}
	kep->kl = 0 ;
	kep->count = 0 ;
	return rs ;
}
/* end subroutine (key_finish) */

static int key_mat(KEY *kep,cchar *kp,int kl) noex {
	int		f = false ;
	if (kl >= 0) {
	    f = (kep->kl == kl) ;
	    if (f) {
	 	f = (strncmp(kep->kp,kp,kl) == 0) ;
	    }
	} else {
	 	f = (strcmp(kep->kp,kp) == 0) ;
	} /* end if */
	return f ;
}
/* end subroutine (key_mat) */

static int entry_start(ENT *ep,int fi,int ki,KEY *kep,cc *vp,int vl) noex {
	int		rs = SR_FAULT ;
	int		kl = 0 ;
	if (ep && kep && vp) {
	    memclear(ep) ;
	    kl = kep->kl ;
	    ep->fi = fi ;
	    ep->ki = ki ;
	    ep->kep = kep ;
	    ep->vlen = vl ;
	    if (cchar *cp{} ; (rs = uc_mallocstrw(vp,vl,&cp)) >= 0) {
	        ep->vname = cp ;
	        key_increment(kep) ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? kl : rs ;
}
/* end subroutine (entry_start) */

static int entry_finish(ENT *ep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rc = 0 ;
	if (ep) {
	    rs = SR_BUGCHECK ;
	    if (ep->vname && ep->kep) {
		cint	nkeys = key_decrement(ep->kep) ;
		rs = SR_OK ;
	        if (ep->vname) {
	            rs1 = uc_free(ep->vname) ;
	            if (rs >= 0) rs = rs1 ;
	            ep->vname = nullptr ;
	        }
		rc = (nkeys == 0) ? ep->ki : INT_MAX ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (entry_finish) */

static int entry_matkey(ENT *ep,cchar *kp,int kl) noex {
	int		rs ;
	int		ki = 0 ;
	if ((rs = key_mat(ep->kep,kp,kl)) > 0) {
	    ki = ep->ki ;
	} else {
	    rs = SR_NOTFOUND ;
	}
	return (rs >= 0) ? ki : rs ;
}
/* end subroutine (entry_matkey) */

static unsigned int hashente(const ENT *ep,int len) noex {
	const KEY	*kep = ep->kep ;
	uint		hv = 0 ;
	(void) len ;
	hv += hash_elf(kep->kp,kep->kl) ;
	hv += hash_elf(ep->vname,-1) ;
	hv += (ep->fi << 4) ;
	return hv ;
}
/* end subroutine (hashente) */

static unsigned int hashent(cvoid *vp,int len) noex {
	const ENT	*kep = entp(vp) ;
	return hashente(kep,len) ;
}
/* end subroutine (hashent) */

static int cmpkeye(const KEY *k1p,const KEY *k2p) noex {
	int		rc ;
	cchar		*c1p = k1p->kp ;
	cchar		*c2p = k2p->kp ;
	if ((rc = (c1p[0] - c2p[0])) == 0) {
	    rc = strcmp(c1p,c2p) ;
	}
	return rc ;
}
/* end subroutine (cmpkeye) */

static int vcmpkey(cvoid **v1pp,cvoid **v2pp) noex {
	const KEY	**k1pp = (const KEY **) v1pp ;
	const KEY	**k2pp = (const KEY **) v2pp ;
	int		rc = 0 ;
	{
	    const KEY	*k1p = *k1pp ;
	    const KEY	*k2p = *k2pp ;
	    if (k1p || k2p) {
		rc = +1 ;
		if (k1p) {
		    rc = -1 ;
		    if (k2p) {
	    	        rc = cmpkeye(k1p,k2p) ;
		    }
		}
	    }
	} /* end block */
	return rc ;
}
/* end subroutine (vcmpkey) */

static int cmpente(const ENT *e1p,const ENT *e2p,int) noex {
	int		rc = (e1p->fi - e2p->fi) ;
	if (rc == 0) {
	    const KEY	*k1p = e1p->kep ;
	    const KEY	*k2p = e2p->kep ;
	    if ((rc = strcmp(k1p->kp,k2p->kp)) == 0) {
	        rc = strcmp(e1p->vname,e2p->vname) ;
	    }
	}
	return rc ;
}
/* end subroutine (cmpente) */

static int cmpent(cvoid *v1p,cvoid *v2p,int len) noex {
	const ENT	*e1p = entp(v1p) ;
	const ENT	*e2p = entp(v2p) ;
	return cmpente(e1p,e2p,len) ;
}
/* end subroutine (cmpent) */


