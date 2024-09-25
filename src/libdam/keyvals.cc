/* keyvals SUPPORT */
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

struct keyvals_entry {
	KEYVALS_KEY	*kep ;
	cchar		*vname ;
	int		vlen ;
	int		fi ;		/* file index */
	int		ki ;		/* key index */
} ;


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

extern "C" {
    static uint	hashkeyval(ENT *,int) noex ;
}

extern "C" {
    static int	vcmpkey(cvoid **,cvoid **) noex ;
    static int	vcmpentry(ENT *,KEYVALS_ENT *,int) noex ;
}


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
	    cint	vo = = (VECOBJ_OSTATIONARY | VECOBJ_OREUSE) ;
	    if ((rs = vecobj_start(op->keyp,sz,vn,vo)) >= 0) {
	        uint	(*hk)(ENT *,int) = hashkeyval ;
	        int		(*vkv)() = vcmpentry ;
	        if ((rs = hdb_start(op->bykeyvalp,ndef,0,hk,vkv)) >= 0) {
	            if ((rs = hdb_start(op->keyp,ndef,0,np,np)) >= 0) {
	                op->magic = KEYVALS_MAGIC ;
	            }
		    if (rs < 0) {
	    	        hdb_finish(op->bykeyvalp) ;
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
	        rs1 = hdb_finish(op->keyp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = hdb_finish(op->bykeyvalp) ;
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
	    rs = hdb_count(op->keyp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (keyvals_count) */

int keyvals_curbegin(keyvals *op,keyvals_cur *curp) noex {
	int		rs ;
	if ((rs = keyvals_magic(op,curp)) >= 0) {
	    cint	osz = sizeof(hdb_cur) ;
	    void	*vp{} ;
	    curp->i = -1 ;
	    if ((rs = uc_malloc(osz,&vp)) >= 0) {
		curp->ecp = (hdb_cur *) vp ;
	        rs = hdb_curbegin(op->keyp,curp->ecp) ;
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
	            rs1 = hdb_curend(op->keyp,curp->ecp) ;
		    id (rs >= 0) rs = rs1 ;
		}
		{
		    rs1 = uc_free(curp->ecp) ;
		    id (rs >= 0) rs = rs1 ;
		}
		curp->ecp = nullptr ;
	    } /* end if (non-null) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (keyvals_curend) */

int keyvals_curenumkey(keyvals *op,CUR *curp,cchar **kpp) noex {
	KEY		*kep = nullptr ;
	int		rs = SR_OK ;
	int		oi ;
	int		kl = 0 ;
	cchar		*kp = nullptr ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magic != KEYVALS_MAGIC) return SR_NOTOPEN ;

	oi = (curp->i >= 0) ? (curp->i + 1) : 0 ;

/* CONSTCOND */

	while (true) {
	    rs = vecobj_get(op->keyp,oi,&kep) ;
	    if (rs < 0) break ;
	    if (kep != nullptr) break ;
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

	return (rs >= 0) ? kl : rs ;
}
/* end subroutine (keyvals_curenumkey) */

int keyvals_curenum(keyvals *op,CUR *curp,cchar **kpp,cchar **vpp) noex {
	ENT		*ep ;
	hdb_dat		key ;
	hdb_dat		val ;
	hdb_cur		cur ;
	int		rs ;
	int		kl = 0 ;
	cchar		*kp = nullptr ;
	cchar		*vp = nullptr ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magic != KEYVALS_MAGIC) return SR_NOTOPEN ;

	cur = curp->ec ;
	if ((rs = hdb_enum(op->keyp,&cur,&key,&val)) >= 0) {
	    {
	        kp = charp(key.buf) ;
	        kl = key.len ;
	    }
	    {
	        ep = (ENT *) val.buf ;
	        vp = ep->vname ;
	    }
	    curp->ec = cur ;
	} /* end if (had an entry) */

	if (kpp) {
	    *kpp = kp ;
	}

	if (vpp) {
	    *vpp = vp ;
	}

	return (rs >= 0) ? kl : rs ;
}
/* end subroutine (keyvals_curenum) */


int keyvals_fetch(op,keybuf,curp,vpp)
KEYVALS		*op ;
cchar		keybuf[] ;
KEYVALS_CUR	*curp ;
cchar		**vpp ;
{
	KEYVALS_ENT	*ep ;
	hdb_dat	key, val ;
	hdb_cur		cur ;
	int		rs ;
	int		kl ;
	int		vl = 0 ;
	cchar		*kp ;
	cchar		*vp = nullptr ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (keybuf == nullptr) return SR_FAULT ;

	if (op->magic != KEYVALS_MAGIC) return SR_NOTOPEN ;

	kp = (cchar	 *) keybuf ;
	if (keybuf[0] == '\0') kp = "default" ;
	kl = strlen(kp) ;

	key.buf = kp ;
	key.len = kl ;
	cur = curp->ec ;
	if ((rs = hdb_fetch(op->keyp,key,&cur,&val)) >= 0) {
	    ep = (ENT *) val.buf ;
	    vp = ep->vname ;
	    vl = ep->vlen ;
	    curp->ec = cur ;
	} /* end if (had an entry) */

	if (vpp) {
	    *vpp = vp ;
	}

	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (keyvals_fetch) */


/* delete up all entries associated w/ a file */
int keyvals_delset(op,fi)
KEYVALS		*op ;
int		fi ;
{
	hdb_cur		cur ;
	hdb_dat	key, val ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != KEYVALS_MAGIC) return SR_NOTOPEN ;

/* delete all keyvals w/ this file */

	if ((rs = hdb_curbegin(op->keyp,&cur)) >= 0) {
	    KEYVALS_ENT	*ep ;
	    while (hdb_enum(op->keyp,&cur,&key,&val) >= 0) {
	        ep = (ENT *) val.buf ;
	        if ((ep->fi == fi) || (fi < 0)) {
		    c += 1 ;
	            hdb_delcur(op->keyp,&cur,0) ;
	        } /* end if (found matching entry) */
	    } /* end while (looping through entries) */
	    hdb_curend(op->keyp,&cur) ;
	} /* end if (cursor) */

	if (rs >= 0) {
	    if ((rs = hdb_curbegin(op->bykeyvalp,&cur)) >= 0) {
	        KEYVALS_ENT	*ep ;
	        while (hdb_enum(op->bykeyvalp,&cur,&key,&val) >= 0) {
	            ep = (ENT *) val.buf ;
	            if ((ep->fi == fi) || (fi < 0)) {
	                hdb_delcur(op->bykeyvalp,&cur,0) ;
		        entry_finish(ep) ;
		        uc_free(ep) ;
	            } /* end if (key-match) */
	        } /* end while (looping through entries) */
	        hdb_curend(op->bykeyvalp,&cur) ;
	    } /* end if (cursor) */
	} /* end if */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (keyvals_delset) */


int keyvals_delkey(op,kp,kl)
KEYVALS		*op ;
cchar		*kp ;
int		kl ;
{
	hdb_cur		cur ;
	hdb_dat	key, val ;
	int		rs ;
	int		rs1 ;
	int		ki = 0 ;
	int		c = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (kp == nullptr) return SR_FAULT ;

	if (op->magic != KEYVALS_MAGIC) return SR_NOTOPEN ;

/* delete all keyvals w/ this key */

	if ((rs = hdb_curbegin(op->keyp,&cur)) >= 0) {
	    key.buf = kp ;
	    key.len = strlen(kp) ;
	    while (hdb_fetch(op->keyp,key,&cur,&val) >= 0) {
		c += 1 ;
	        hdb_delcur(op->keyp,&cur,0) ;
	    } /* end while */
	    hdb_curend(op->keyp,&cur) ;
	} /* end if (cursor) */

	if (rs >= 0) {
	    if ((rs = hdb_curbegin(op->bykeyvalp,&cur)) >= 0) {
	        KEYVALS_ENT	*ep ;
	        while (hdb_enum(op->bykeyvalp,&cur,&key,&val) >= 0) {
	            ep = (ENT *) val.buf ;
	            if (entry_matkey(ep,kp,kl) >= 0) {
	                hdb_delcur(op->bykeyvalp,&cur,0) ;
		        entry_finish(ep) ;
		        uc_free(ep) ;
	            } /* end if (key-match) */
	        } /* end while (looping through entries) */
	        hdb_curend(op->bykeyvalp,&cur) ;
	    } /* end if (cursor) */
	} /* end if */

/* delete the key from the key-store */

	if ((rs >= 0) && (ki >= 0)) {
	    rs = vecobj_del(op->keyp,ki) ;
	}

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (keyvals_delkey) */


/* private subroutines */


static int keyvals_keyadd(op,kep,rpp)
KEYVALS		*op ;
KEYVALS_KEY	*kep ;
KEYVALS_KEY	**rpp ;
{
	int		rs ;
	int		ki = INT_MAX ;

	if ((rs = vecobj_add(op->keyp,kep)) >= 0) {
	    ki = rs ;
	    rs = vecobj_get(op->keyp,ki,rpp) ;
	}

	return (rs >= 0) ? ki : rs ;
}
/* end subroutine (keyvals_keyadd) */


static int keyvals_keyfins(op)
KEYVALS		*op ;
{
	KEYVALS_KEY	*kep ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		i ;

	for (i = 0 ; vecobj_get(op->keyp,i,&kep) >= 0 ; i += 1) {
	    if (kep == nullptr) continue ;
	    rs1 = key_finish(kep) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end for */

	return rs ;
}
/* end subroutine (keyvals_keyfins) */


/* get a pointer to the current key (make it as necessary) */
static int keyvals_keyget(op,keybuf,kpp)
KEYVALS		*op ;
cchar		keybuf[] ;
KEYVALS_KEY	**kpp ;
{
	KEYVALS_KEY	ke ;
	KEYVALS_KEY	*kep = nullptr ;
	int		rs, rs1 ;
	int		ki = 0 ;

	if ((rs = key_start(&ke,keybuf)) >= 0) {
	    bool	f = true ;
	    rs = vecobj_search(op->keyp,&ke,vcmpkey,&kep) ;
	    ki = rs ;
	    if (rs == SR_NOTFOUND) {
		if ((rs = keyvals_keyadd(op,&ke,&kep)) >= 0) {
		    f = false ;
	    	    ki = rs ;
		}
	    } 
	    if (f) key_finish(&ke) ;
	} /* end if (needed to enter new key) */

	if (kpp != nullptr) {
	    *kpp = (rs >= 0) ? kep : nullptr ;
	}

	return (rs >= 0) ? ki : rs ;
}
/* end subroutine (keyvals_keyget) */


static int keyvals_keydel(op,ki)
KEYVALS		*op ;
int		ki ;
{
	int		rs ;

	rs = vecobj_del(op->keyp,ki) ;

	return rs ;
}
/* end subroutine (keyvals_keydel) */


/* do we have this entry already */
static int keyvals_already(op,nep)
KEYVALS		*op ;
KEYVALS_ENT	*nep ;
{
	hdb_dat	key, val ;
	int		rs ;

	key.buf = nep ;
	key.len = sizeof(KEYVALS_ENT) ;
	rs = hdb_fetch(op->bykeyvalp,key,nullptr,&val) ;

	return rs ;
}
/* end subroutine (keyvals_already) */


/* add an entry to entry list */
static int keyvals_addentry(op,nep)
KEYVALS		*op ;
KEYVALS_ENT	*nep ;
{
	KEYVALS_ENT	*ep ;
	int		rs ;
	int		sz = sizeof(KEYVALS_ENT) ;
	if ((rs = uc_malloc(sz,&ep)) >= 0) {
	    KEYVALS_KEY	*kep ;
	    hdb_dat	key, val ;
	    *ep = *nep ; /* copy */
	    kep = ep->kep ;
		key.buf = kep->kp ;
		key.len = kep->kl ;
	    val.buf = ep ;
	    val.len = sizeof(KEYVALS_ENT) ;
	    if ((rs = hdb_store(op->keyp,key,val)) >= 0) {
		key.buf = ep ;
		key.len = sizeof(KEYVALS_ENT) ;
	        rs = hdb_store(op->bykeyvalp,key,val) ;
		if (rs < 0) {
	    	    hdb_cur	cur ;
	    	    int		rs1 ;
	    	    hdb_curbegin(op->keyp,&cur) ;
	    	    {
	        	    rs1 = hdb_fetch(op->keyp,key,&cur,&val) ;
	        	    if (rs1 >= 0)
	            	    hdb_delcur(op->keyp,&cur,0) ;
	    	    }
	    	    hdb_curend(op->keyp,&cur) ;
		}
	    }
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
	int		rs ;
	int		rs1 ;
	if ((rs = hdb_curbegin(elp,&cur)) >= 0) {
	    hdb_dat	key ;
	    hdb_dat	val ;
	    while (hdb_enum(elp,&cur,&key,&val) >= 0) {
	        ENT	*ep = (ENT *) val.buf ;
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
	} /* end if (cursor) */
	return rs ;
}
/* end subroutine (keyvals_entfins) */


static int key_start(kep,kname)
KEYVALS_KEY	*kep ;
cchar		kname[] ;
{
	int		rs = SR_OK ;
	int		kl = 0 ;

	memset(kep,0,sizeof(KEYVALS_KEY)) ;

	if (kname != nullptr) {
	    cchar		*kp ;
	    kl = strlen(kname) ;
	    if ((rs = uc_mallocstrw(kname,kl,&kp)) >= 0) {
		kep->kp = kp ;
	        kep->kl = kl ;
	    }
	}

	return (rs >= 0) ? kl : rs ;
}
/* end subroutine (key_start) */


static int key_increment(kep)
KEYVALS_KEY	*kep ;
{

	if (kep == nullptr) return SR_FAULT ;

	kep->count += 1 ;
	return SR_OK ;
}
/* end subroutine (key_increment) */


static int key_decrement(kep)
KEYVALS_KEY	*kep ;
{

	if (kep == nullptr) return SR_FAULT ;

	if (kep->count > 0)
	    kep->count -= 1 ;

#ifdef	COMMENT
	if ((kep->count == 0) && (kep->kp != nullptr)) {
	    uc_free(kep->kp) ;
	    kep->kp = nullptr ;
	    kep->kl = 0 ;
	}
#endif /* COMMENT */

	return kep->count ;
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

static int key_mat(ENT *kep,cchar *kp,int kl) noex {
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

static int entry_start(ep,fi,ki,kep,vp,vl)
KEYVALS_ENT	*ep ;
int		fi, ki ;
KEYVALS_KEY	*kep ;
cchar		*vp ;
int		vl ;
{
	cint	kl = kep->kl ;
	int		rs ;
	cchar		*cp ;

	memclear(ep) ;
	ep->fi = fi ;
	ep->ki = ki ;
	ep->kep = kep ;
	ep->vlen = vl ;

	if ((rs = uc_mallocstrw(vp,vl,&cp)) >= 0) {
	    ep->vname = cp ;
	    key_increment(kep) ;
	}

	return (rs >= 0) ? kl : rs ;
}
/* end subroutine (entry_start) */


static int entry_finish(ep)
KEYVALS_ENT	*ep ;
{
	int		rs = SR_OK ;
	int		rs1 ;
	int		nkeys ;
	int		rc = 0 ;

	if (ep->vname == nullptr) return SR_OK ;

	if (ep->kep == nullptr) return SR_NOANODE ;

	nkeys = key_decrement(ep->kep) ;

	if (ep->vname != nullptr) {
	    rs1 = uc_free(ep->vname) ;
	    if (rs >= 0) rs = rs1 ;
	    ep->vname = nullptr ;
	}

	rc = (nkeys == 0) ? ep->ki : INT_MAX ;
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (entry_finish) */


static int entry_matkey(ep,kp,kl)
KEYVALS_ENT	*ep ;
cchar		*kp ;
int		kl ;
{
	int		rs ;
	int		ki ;

	rs = key_mat(ep->kep,kp,kl) ;
	ki = (rs > 0) ? ep->ki : SR_NOTFOUND ;
	return (rs >= 0) ? ki : rs ;
}
/* end subroutine (entry_matkey) */

static unsigned int hashkeyval(ENT *ep,int len) noex {
	KEY		*kep = ep->kep ;
	uint		hv = 0 ;
	(void) len ;
	hv += hash_elf(kep->kp,kep->kl) ;
	hv += hash_elf(ep->vname,-1) ;
	hv += (ep->fi << 4) ;
	return hv ;
}
/* end subroutine (hashkeyval) */

static int vcmpkeye(KEY *e1p,KEY *e2p) noex {
	int		rc = 0 ;
	if ((rc = (e1p->kp[0] - e2p->kp[0]) == 0) {
	    rc = strcmp(e1p->kp,e2p->kp) ;
	}
	return rc ;
}
/* end subroutine (vcmpkeye) */

static int vcmpkey(cvoid *v1pp,cvoid **v2pp) noex {
	KEY		**e1pp = (KEY *) v1pp ;
	KEY		**e2pp = (KEY *) v2pp ;
	int		rc = 0 ;
	{
	    KEY		*e1p = *e1pp ;
	    KEY		*e2p = *e2pp ;
	    if (e1p || e2p) {
		rc = +1 ;
		if (e1p) {
		    rc = -1 ;
		    if (e2p) {
	    	        rc = vcmpkeye(e1p,e2p) ;
		    }
		}
	    }
	}
	return rc ;
}
/* end subroutine (vcmpkey) */

static int vcmpentry(ENT *e1p,KEYVALS_ENT *e2p,int) noex {
	int		rc = (e1p->fi - e2p->fi) ;
	if (rc == 0) {
	    if ((rc = strcmp(e1p->kep->kp,e2p->kep->kp)) == 0) {
	        rc = strcmp(e1p->vname,e2p->vname) ;
	    }
	}
	return rc ;
}
/* end subroutine (vcmpentry) */


