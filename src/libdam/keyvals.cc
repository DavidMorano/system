/* keyvals SUPPORT */
/* charset=ISO8859-1 */
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
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<new>			/* C++STD |nothrow(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<vecobj.h>		/* LIBUC */
#include	<hdb.h>			/* LIBUC */
#include	<hash.h>		/* LIBUC |hash_elf(3dam)| */
#include	<localmisc.h>		/* LIB */

#include	"keyvals.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	KEYVALS_KEY	keyvals_key
#define	KEYVALS_ENT	keyvals_entry

#define	KV		keyvals
#define	KEY		keyvals_key
#define	CUR		keyvals_cur
#define	ENT		keyvals_entry


/* imported namespaces */

using libuc::mem ;			/* variable */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct keyvals_key {
	cchar		*kp ;
	int		kl ;
	int		count ;
} ; /* end struct */

typedef keyvals_key *	keyp ;

struct keyvals_entry {
	KEYVALS_KEY	*kep ;
	cchar		*vname ;
	int		vlen ;
	int		fi ;		/* file index */
	int		ki ;		/* key index */
} ; /* end struct */

typedef keyvals_entry *	entp ;


/* forward references */

template<typename ... Args>
local int keyvals_ctor(keyvals *op,Args ... args) noex {
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    op->magval = 0 ;
	    if ((op->keyp = new(nothrow) vecobj) != np) ylikely {
		if ((op->bykeyp = new(nothrow) hdb) != np) ylikely {
		    if ((op->bykeyvalp = new(nothrow) hdb) != np) ylikely {
			rs = SR_OK ;
		    } /* end if (new-hdb) */
		    if (rs < 0) {
		        delete op->bykeyp ;
		        op->bykeyp = nullptr ;
		    } /* end if (error) */
		} /* end if (new-hdb) */
		if (rs < 0) {
		    delete op->keyp ;
		    op->keyp = nullptr ;
		} /* end if (error) */
	    } /* end if (new-vecobj) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (keyvals_ctor) */

local int keyvals_dtor(keyvals *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->bykeyvalp) ylikely {
		delete op->bykeyvalp ;
		op->bykeyvalp = nullptr ;
	    }
	    if (op->bykeyp) ylikely {
		delete op->bykeyp ;
		op->bykeyp = nullptr ;
	    }
	    if (op->keyp) ylikely {
		delete op->keyp ;
		op->keyp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (keyvals_dtor) */

template<typename ... Args>
local inline int keyvals_magic(keyvals *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == KEYVALS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (keyvals_magic) */

local int	keyvals_keyadd(keyvals *,KEY *,KEY **) noex ;
local int	keyvals_keyget(keyvals *,cchar *,KEY **) noex ;
local int	keyvals_keyhave(keyvals *,cchar *,int,KEY **) noex ;
local int	keyvals_keydel(keyvals *,int) noex ;
local int	keyvals_enthave(keyvals *,ENT *) noex ;
local int	keyvals_entadd(keyvals *,ENT *) noex ;
local int	keyvals_finents(keyvals *) noex ;
local int	keyvals_finkeys(keyvals *) noex ;

local int	key_start(KEY *,cchar *,int = -1) noex ;
local int	key_increment(KEY *) noex ;
local int	key_decrement(KEY *) noex ;
local int	key_finish(KEY *) noex ;
local int	key_mat(KEY *,cchar *,int) noex ;

local int	entry_start(ENT *,int,int,KEY *,cchar *,int) noex ;
local int	entry_matkey(ENT *,cchar *,int) noex ;
local int	entry_finish(ENT *) noex ;

extern "C" {
    static uint	hashent(cvoid *,int) noex ;
} /* end extern (C) */

extern "C" {
    local int	vcmpkey(cvoid **,cvoid **) noex ;
    local int	cmpent(cvoid *,cvoid *,int) noex ;
} /* end extern (C) */

constexpr bool		f_comment = false ;


/* local variables */


/* exported variables */


/* exported subroutines */

int keyvals_start(keyvals *op,int ndef) noex {
	cnullptr	np{} ;
	int		rs ;
	if (ndef < KEYVALS_DEFENTS) {
	    ndef = KEYVALS_DEFENTS ;
	}
	if ((rs = keyvals_ctor(op)) >= 0) ylikely {
	    cint	sz = szof(KEYVALS_KEY) ;
	    cint	vn = (ndef / 10) ;
	    cint	vo = (vecobjm.stationary | vecobjm.reuse) ;
	    if ((rs = vecobj_start(op->keyp,sz,vn,vo)) >= 0) ylikely {
	        if ((rs = hdb_start(op->bykeyp,ndef,0,np,np)) >= 0) {
	            auto	hk = hashent ;
	            auto	cmp = cmpent ;
	            if ((rs = hdb_start(op->bykeyvalp,ndef,0,hk,cmp)) >= 0) {
	                op->magval = KEYVALS_MAGIC ;
	            }
		    if (rs < 0) {
	    	        hdb_finish(op->bykeyp) ;
		    } /* end if (error) */
	        } /* end if (hdb_start) */
	        if (rs < 0) {
		    vecobj_finish(op->keyp) ;
	        } /* end if (error) */
	    } /* end if (vecobj_start) */
	    if (rs < 0) {
		keyvals_dtor(op) ;
	    } /* end if (error) */
	} /* end if (keyvals_ctor) */
	return rs ;
} /* end subroutine (keyvals_start) */

int keyvals_finish(keyvals *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = keyvals_magic(op)) >= 0) ylikely {
	    {
	        rs1 = keyvals_finents(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = keyvals_finkeys(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->bykeyvalp) {
	        rs1 = hdb_finish(op->bykeyvalp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->bykeyp) {
	        rs1 = hdb_finish(op->bykeyp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->keyp) {
	        rs1 = vecobj_finish(op->keyp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = keyvals_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (keyvals_finish) */

int keyvals_add(keyvals *op,int fi,cc *kp,cc *vp,int vl) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		rs1 ;
	int		cadd = 0 ;
	if ((rs = keyvals_magic(op,kp,vp)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (*kp) {
	        if (vl < 0) vl = lenstr(vp) ;
	        if (KEY *kep{} ; (rs = keyvals_keyget(op,kp,&kep)) >= 0) {
	            KEYVALS_ENT		ve ;
	            cint		ki = rs ;
	            if ((rs = entry_start(&ve,fi,ki,kep,vp,vl)) >= 0) {
	                bool	f = true ;
	                if ((rs = keyvals_enthave(op,&ve)) == rsn) {
	            	    if ((rs = keyvals_entadd(op,&ve)) >= 0) {
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
} /* end subroutine (keyvals_add) */

int keyvals_already(keyvals *op,cc *kp,int kl) noex {
    	return keyvals_keyhave(op,kp,kl,nullptr) ;
} /* end subroutine (keyvals_already) */

int keyvals_count(keyvals *op) noex {
	int		rs ;
	if ((rs = keyvals_magic(op)) >= 0) ylikely {
	    rs = hdb_count(op->bykeyp) ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (keyvals_count) */

int keyvals_curbegin(keyvals *op,keyvals_cur *curp) noex {
	int		rs ;
	if ((rs = keyvals_magic(op,curp)) >= 0) ylikely {
	    cint	osz = szof(hdb_cur) ;
	    curp->i = -1 ;
	    if (void *vp{} ; (rs = mem.mall(osz,&vp)) >= 0) {
		curp->ecp = (hdb_cur *) vp ;
	        rs = hdb_curbegin(op->bykeyp,curp->ecp) ;
		if (rs < 0) {
		    mem.free(curp->ecp) ;
		    curp->ecp = nullptr ;
		} /* end if (error) */
	    } /* end if (memory-acquire) */
	} /* end if (magic) */
	return rs ;
} /* end subroutine (keyvals_curbegin) */

int keyvals_curend(keyvals *op,keyvals_cur *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = keyvals_magic(op,curp)) >= 0) ylikely {
	    rs = SR_FAULT ;
	    if (curp->ecp) ylikely {
		rs = SR_OK ;
	        curp->i = -1 ;
		if (op->bykeyp) ylikely {
	            rs1 = hdb_curend(op->bykeyp,curp->ecp) ;
		    if (rs >= 0) rs = rs1 ;
		}
		if (curp->ecp) ylikely {
		    rs1 = mem.free(curp->ecp) ;
		    if (rs >= 0) rs = rs1 ;
		}
		curp->ecp = nullptr ;
	    } /* end if (cursor-valid) */
	} /* end if (magic) */
	return rs ;
} /* end subroutine (keyvals_curend) */

int keyvals_curenumkey(keyvals *op,CUR *curp,cchar **kpp) noex {
	int		rs ;
	int		kl = 0 ;
	if ((rs = keyvals_magic(op,curp)) >= 0) ylikely {
	    KEY		*kep = nullptr ;
	    int		oi = (curp->i >= 0) ? (curp->i + 1) : 0 ;
	    cchar	*kp = nullptr ;
	    void	*vp{} ;
	    while ((rs = vecobj_get(op->keyp,oi,&vp)) >= 0) {
	        kep = keyp(vp) ;
	        if (kep) break ;
	        oi += 1 ;
	    } /* end while */
	    if ((rs >= 0) && kep) {
	        kp = kep->kp ;
	        kl = kep->kl ;
	        curp->i = oi ;
	    }
	    if (kpp) {
		*kpp = kp ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? kl : rs ;
} /* end subroutine (keyvals_curenumkey) */

int keyvals_curenum(keyvals *op,CUR *curp,cchar **kpp,cchar **vpp) noex {
	int		rs ;
	int		kl = 0 ;
	if ((rs = keyvals_magic(op,curp)) >= 0) ylikely {
	    rs = SR_BUGCHECK ;
	    if (curp->ecp) {
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
	        	ENT	*ep = (ENT *) val.buf ;
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
} /* end subroutine (keyvals_curenum) */

int keyvals_fetch(keyvals *op,cchar *kp,CUR *curp,cchar **vpp) noex {
	int		rs ;
	int		vl = 0 ;
	if ((rs = keyvals_magic(op,kp,curp)) >= 0) ylikely {
	    rs = SR_BUGCHECK ;
	    if (curp->ecp) {
	        hdb_dat		key ;
	        hdb_dat		val ;
	        int		kl ;
	        cchar		*valp = nullptr ;
	        if (kp[0] == '\0') kp = "default" ;
	        kl = lenstr(kp) ;
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
} /* end subroutine (keyvals_fetch) */

int keyvals_delset(keyvals *op,int fi) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = keyvals_magic(op)) >= 0) ylikely {
	    hdb_cur	cur ; /* used multiple blocks */
	    hdb_dat	key ; /* used multiple blocks */
	    hdb_dat	val ; /* used multiple blocks */
	    ENT		*ep ; /* used multiple blocks */
	    if ((rs = hdb_curbegin(op->bykeyp,&cur)) >= 0) {
	        while (hdb_curenum(op->bykeyp,&cur,&key,&val) >= 0) {
	            ep = entp(val.buf) ;
	            if ((ep->fi == fi) || (fi < 0)) {
		        c += 1 ;
	                hdb_curdel(op->bykeyp,&cur,0) ;
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
	                    hdb_curdel(op->bykeyvalp,&cur,0) ;
		            entry_finish(ep) ;
		            mem.free(ep) ;
	                } /* end if (key-match) */
	            } /* end while (looping through entries) */
	            rs1 = hdb_curend(op->bykeyvalp,&cur) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (cursor) */
	    } /* end if (ok) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (keyvals_delset) */

int keyvals_delkey(keyvals *op,cchar *kp,int kl) noex {
    	cnullptr	np{} ;
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = keyvals_magic(op,kp)) >= 0) ylikely {
	    if ((rs = keyvals_keyhave(op,kp,kl,np)) >= 0) ylikely {
	        hdb		*bykeyp = op->bykeyp ;
	        hdb_cur		cur ; /* used in multiple blocks (below) */
	        hdb_dat		key ; /* used multiple blocks */
	        hdb_dat		val ; /* used multiple blocks */
	        cint		ki = rs ;
	        /* delete all keyvals w/ this key */
	        if ((rs = hdb_curbegin(bykeyp,&cur)) >= 0) {
	            key.buf = kp ;
	            key.len = lenstr(kp) ;
	            while ((rs1 = hdb_fetch(bykeyp,key,&cur,&val)) >= 0) {
		        c += 1 ;
	                hdb_curdel(bykeyp,&cur,0) ;
	            } /* end while */
		    if ((rs >= 0) && (rs1 != rsn)) rs = rs1 ;
	            rs1 = hdb_curend(bykeyp,&cur) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (cursor) */
	        if (rs >= 0) {
	            hdb	*kvp = op->bykeyvalp ;
	            if ((rs = hdb_curbegin(kvp,&cur)) >= 0) {
	                while ((rs1 = hdb_curenum(kvp,&cur,&key,&val)) >= 0) {
	                    KEYVALS_ENT	*ep = entp(val.buf) ;
	                    if ((rs = entry_matkey(ep,kp,kl)) >= 0) {
	                        hdb_curdel(kvp,&cur,0) ;
		                entry_finish(ep) ;
		                mem.free(ep) ;
			    } else if (rs == rsn) {
			        rs = SR_OK ;
	                    } /* end if (key-match) */
	                } /* end while (looping through entries) */
		        if ((rs >= 0) && (rs1 != rsn)) rs = rs1 ;
	                rs1 = hdb_curend(kvp,&cur) ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (cursor) */
	        } /* end if (ok) */
	        /* delete the key from the key-store */
	        if ((rs >= 0) && (ki >= 0)) {
	            rs = vecobj_del(op->keyp,ki) ;
	        }
	    } /* end if (vecobj_keyhave) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (keyvals_delkey) */


/* private subroutines */

local int keyvals_keyadd(keyvals *op,KEY *kep,KEY **rpp) noex {
    	vecobj		*klp = op->keyp ;
	int		rs ;
	int		ki = INT_MAX ;
	if ((rs = klp->add(kep)) >= 0) {
	    ki = rs ;
	    if (void *vp{} ; (rs = klp->get(ki,&vp)) >= 0) {
		*rpp = keyp(vp) ;
	    }
	}
	return (rs >= 0) ? ki : rs ;
} /* end subroutine (keyvals_keyadd) */

local int keyvals_finkeys(keyvals *op) noex {
    	vecobj		*klp = op->keyp ;
	int		rs = SR_OK ;
	int		rs1 ;
	void		*vp{} ;
	for (int i = 0 ; klp->get(i,&vp) >= 0 ; i += 1) {
	    KEY		*kep = keyp(vp) ;
	    if (vp) {
	        rs1 = key_finish(kep) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */
	return rs ;
} /* end subroutine (keyvals_finkeys) */

local int keyvals_keyget(keyvals *op,cchar *keybuf,KEY **kpp) noex {
	KEYVALS_KEY	*kep = nullptr ;
	int		rs ;
	int		ki = 0 ;
	if (KEYVALS_KEY ke ; (rs = key_start(&ke,keybuf)) >= 0) {
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
	    if (f) {
		key_finish(&ke) ;
	    }
	} /* end if (needed to enter new key) */
	if (kpp) {
	    *kpp = (rs >= 0) ? kep : nullptr ;
	}
	return (rs >= 0) ? ki : rs ;
} /* end subroutine (keyvals_keyget) */

local int keyvals_keyhave(keyvals *op,cchar *kp,int kl,KEY **kpp) noex {
	KEYVALS_KEY	*kep = nullptr ;
	int		rs ;
	int		rs1 ;
	int		ki = 0 ;
	if (KEYVALS_KEY ke ; (rs = key_start(&ke,kp,kl)) >= 0) {
    	    vecobj	*klp = op->keyp ;
	    if (void *vp{} ; (rs = klp->search(&ke,vcmpkey,&vp)) >= 0) {
		kep = keyp(vp) ;
	        ki = rs ;
	    } 
	    rs1 = key_finish(&ke) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (key) */
	if (kpp) {
	    *kpp = (rs >= 0) ? kep : nullptr ;
	}
	return (rs >= 0) ? ki : rs ;
} /* end subroutine (keyvals_keyhave) */

local int keyvals_keydel(keyvals *op,int ki) noex {
	return vecobj_del(op->keyp,ki) ;
} /* end subroutine (keyvals_keydel) */

local int keyvals_enthave(keyvals *op,ENT *nep) noex {
	hdb_dat		key ;
	hdb_dat		val ; /* dummy */
	key.buf = nep ;
	key.len = szof(KEYVALS_ENT) ;
	return hdb_fetch(op->bykeyvalp,key,nullptr,&val) ;
} /* end subroutine (keyvals_enthave) */

/* add an entry to entry list */
local int keyvals_entadd(keyvals *op,ENT *nep) noex {
	int		rs ;
	int		sz = szof(KEYVALS_ENT) ;
	if (void *vp{} ; (rs = mem.mall(sz,&vp)) >= 0) {
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
	        val.len = szof(KEYVALS_ENT) ;
	    }
	    if ((rs = hdb_store(op->bykeyp,key,val)) >= 0) {
		key.buf = ep ;
		key.len = szof(KEYVALS_ENT) ;
	        rs = hdb_store(op->bykeyvalp,key,val) ;
		if (rs < 0) {
	    	    hdb_cur	cur ;
	    	    hdb_curbegin(op->bykeyp,&cur) ;
	    	    {
	        	if (hdb_fetch(op->bykeyp,key,&cur,&val) >= 0) {
	            	    hdb_curdel(op->bykeyp,&cur,0) ;
			}
	    	    }
	    	    hdb_curend(op->bykeyp,&cur) ;
		} /* end if (error) */
	    } /* end if (hdb_store) */
	    if (rs < 0) {
		mem.free(ep) ;
	    } /* end if (error) */
	} /* end if (memory-acquire) */
	return rs ;
} /* end subroutine (keyvals_entadd) */

local int keyvals_finents(keyvals *op) noex {
	hdb		*elp = op->bykeyvalp ;
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		rs1 ;
	int		rs2 ;
	if (hdb_cur cur ; (rs = hdb_curbegin(elp,&cur)) >= 0) {
	    hdb_dat	key ;
	    hdb_dat	val ;
	    while ((rs2 = hdb_curenum(elp,&cur,&key,&val)) >= 0) {
	        ENT	*ep = entp(val.buf) ;
		{
	            rs1 = entry_finish(ep) ;
		    if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = mem.free(ep) ;
		    if (rs >= 0) rs = rs1 ;
		}
	    } /* end while */
	    if ((rs >= 0) && (rs2 != rsn)) rs = rs2 ;
	    rs1 = hdb_curend(elp,&cur) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (cursor) */
	return rs ;
} /* end subroutine (keyvals_finents) */

void keyvals::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("keyvals",rs,"fini-finish") ;
	}
} /* end method (keyvals::dtor) */

keyvals::operator int () noex {
    	int		rs = SR_NOTOPEN ;
	if (keyp) {
	    rs = keyp->count ;
	}
	return rs ;
} /* end method (keyvals::operator) */

int keyvals_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case keyvalsmem_start:
	        rs = keyvals_start(op,a) ;
	        break ;
	    case keyvalsmem_count:
	        rs = keyvals_count(op) ;
	        break ;
	    case keyvalsmem_delset:
	        rs = keyvals_delset(op,a) ;
	        break ;
	    case keyvalsmem_finish:
	        rs = keyvals_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (keyvals_co::operator) */

local int key_start(KEY *kep,cchar *ksp,int ksl) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ;
	if (kep && ksp) ylikely {
	    rs = memclear(kep) ;
	    if (ksp[0]) ylikely {
	        if (ksl < 0) ksl = lenstr(ksp) ;
	        if (cchar *sp{} ; (rs = mem.strw(ksp,ksl,&sp)) >= 0) {
		    kep->kp = sp ;
	            kep->kl = rs ;
		    rl = rs ;
	        } /* end if (memory-acquire) */
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (key_start) */

local int key_increment(KEY *kep) noex {
	int		rs = SR_FAULT ;
	if (kep) ylikely {
	    rs = SR_OK ;
	    kep->count += 1 ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (key_increment) */

local int key_decrement(KEY *kep) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (kep) ylikely {
	    rs = SR_OK ;
	    if (kep->count > 0) {
	        kep->count -= 1 ;
	    }
	    if_constexpr (f_comment) {
	        if ((kep->count == 0) && kep->kp) {
	            voidp vp = voidp(kep->kp) ;
	            mem.free(vp) ;
	            kep->kp = nullptr ;
	            kep->kl = 0 ;
	        } /* end if (memory-release) */
	    } /* end if_constexpr (f_comment) */
	    c = kep->count ;
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (key_decrement) */

local int key_finish(KEY *kep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (kep->kp) {
	    voidp vp = voidp(kep->kp) ;
	    rs1 = mem.free(vp) ;
	    if (rs >= 0) rs = rs1 ;
	    kep->kp = nullptr ;
	} /* end if (memory-release) */
	kep->kl = 0 ;
	kep->count = 0 ;
	return rs ;
} /* end subroutine (key_finish) */

local int key_mat(KEY *kep,cchar *kp,int kl) noex {
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
} /* end subroutine (key_mat) */

local int entry_start(ENT *ep,int fi,int ki,KEY *kep,cc *vp,int vl) noex {
	int		rs = SR_FAULT ;
	int		kl = 0 ;
	if (ep && kep && vp) ylikely {
	    memclear(ep) ;
	    kl = kep->kl ;
	    ep->fi = fi ;
	    ep->ki = ki ;
	    ep->kep = kep ;
	    ep->vlen = vl ;
	    if (cchar *cp{} ; (rs = mem.strw(vp,vl,&cp)) >= 0) {
	        ep->vname = cp ;
	        key_increment(kep) ;
	    } /* end if (memory-acquire) */
	} /* end if (non-null) */
	return (rs >= 0) ? kl : rs ;
} /* end subroutine (entry_start) */

local int entry_finish(ENT *ep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rc = 0 ;
	if (ep) ylikely {
	    rs = SR_BUGCHECK ;
	    if (ep->vname && ep->kep) {
		if ((rs = key_decrement(ep->kep)) >= 0) {
		    cint	nkeys = rs ;
	            if (ep->vname) {
	                voidp vp = voidp(ep->vname) ;
	                rs1 = mem.free(vp) ;
	                if (rs >= 0) rs = rs1 ;
	                ep->vname = nullptr ;
	            } /* end if (memory-release) */
		    rc = (nkeys == 0) ? ep->ki : INT_MAX ;
		} /* end if (key-decrement) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rc : rs ;
} /* end subroutine (entry_finish) */

local int entry_matkey(ENT *ep,cchar *kp,int kl) noex {
	int		rs ;
	int		ki = 0 ;
	if ((rs = key_mat(ep->kep,kp,kl)) > 0) {
	    ki = ep->ki ;
	} else {
	    rs = SR_NOTFOUND ;
	}
	return (rs >= 0) ? ki : rs ;
} /* end subroutine (entry_matkey) */

local unsigned int hashente(const ENT *ep,int len) noex {
	const KEY	*kep = ep->kep ;
	uint		hv = 0 ;
	(void) len ;
	hv += hash_elf(kep->kp,kep->kl) ;
	hv += hash_elf(ep->vname,-1) ;
	hv += (ep->fi << 4) ;
	return hv ;
} /* end subroutine (hashente) */

local unsigned int hashent(cvoid *vp,int len) noex {
	const ENT	*kep = entp(vp) ;
	return hashente(kep,len) ;
} /* end subroutine (hashent) */

local int cmpkeye(const KEY *k1p,const KEY *k2p) noex {
	int		rc ;
	cchar		*c1p = k1p->kp ;
	cchar		*c2p = k2p->kp ;
	if ((rc = (c1p[0] - c2p[0])) == 0) {
	    rc = strcmp(c1p,c2p) ;
	}
	return rc ;
} /* end subroutine (cmpkeye) */

local int vcmpkey(cvoid **v1pp,cvoid **v2pp) noex {
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
} /* end subroutine (vcmpkey) */

local int cmpente(const ENT *e1p,const ENT *e2p,int) noex {
	int		rc = (e1p->fi - e2p->fi) ;
	if (rc == 0) {
	    const KEY	*k1p = e1p->kep ;
	    const KEY	*k2p = e2p->kep ;
	    if ((rc = strcmp(k1p->kp,k2p->kp)) == 0) {
	        rc = strcmp(e1p->vname,e2p->vname) ;
	    }
	}
	return rc ;
} /* end subroutine (cmpente) */

local int cmpent(cvoid *v1p,cvoid *v2p,int len) noex {
	const ENT	*e1p = entp(v1p) ;
	const ENT	*e2p = entp(v2p) ;
	return cmpente(e1p,e2p,len) ;
} /* end subroutine (cmpent) */


