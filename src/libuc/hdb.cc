/* hdb SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* general-purpose in-core hashing */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A�D� Morano
	I wrote this from stratch due to frustration with other
	types of these things. Why I have to wrote something like
	this just goes to show how little strctured containers are
	used now-a-days.

	= 2003-04-19, David A�D� Morano
	I grabbed the previous version and modified it to use a
	look-aside buffer for the entry-node structures. This is
	used instead of 'malloc(3c)' to get and release node
	strutures. The look-aside manager uses 'malloc(3c)' to
	extend the look-aside entries. Entries released back to the
	look-aside manager are not given back to 'malloc(3c)' (via
	'free(3c)' at the present time -- not so easy to do). Anyway,
	we get about an average of 27% to 29% speedup on a combination
	of allocating and freeing entries.

*/

/* Copyright � 1998,2003 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	This package provides a reasonably general-purpose hashing
	object for use in cases calling for in-core hashing.

	One of the features sorely missed in other packages (code
	objects) like this is that this package can store multiple
	key-data pairs with the same keys.

	The "walk" type method, which I never liked in other packages,
	is deprecated in favor of the "enumerate" (|hdb_curenum|) type
	method instead.  There are two types of "delete" methods.
	One deletes all entries with the same key (possibly useful
	for old timers who never knew what to do with muliple
	identical keys in the first place), and the other function
	deletes the entry under the current cursor.

	Oh, the "enumerate" and "fetch" functions require the concept
	of a CURSOR, which is, roughly, a pointer type of a thing
	to the current entry.

	In terms of implementation, these routines are just as
	inelegant, or worse, than the DBM, HDBM, or other type of
	implementations; namely, I implemented hashing with hash
	buckets (using a single table) with chained entries dangling.
	An additional dimension of chains may dangle from each of
	the previous described entries to facilitate muliple entries
	with the same key.

	Important user note:

	This package does not store the user supplied data into its
	own storage space. Only pointers from within the user
	supplied data are stored. For this reason, it is very
	important for the user to not free up any storage locations
	that are still linked inside of this database. Havoc will
	result! Also, when freeing entries in this database, the
	user's data is NOT freed! Further, the DB storage location(s)
	that were used to store the user's data IS freed! This means
	that the user should make pointers to or copies of any data
	that they have (if they ever want to access it again) BEFORE
	they delete the corresponding entry from this database! Got
	it? Do it!

	Implementation note:

	Whew! There may be more hairy code that what is in this
	object, but I don't know where it is! What am I trying to
	say? There may be bugs in this mess! It has held up solidly
	against testing and use already but there could still be
	some corners that are not quite right. Both the core of
	'enumerate' and 'fetch' are very hairy. Fetching is especially
	hairy. Some of the hairyness is due to some efforts (minor)
	to not repeat executing some code unnecessarily. This appoach
	may have been a mistake given the complexity of this whole
	mess already. Maybe there is something to be said for simple
	data strctures after all! Enjoy!

	Synopsis:
	int hdb_start(hdb *op,int n,int at,hsbhas_f h,hsbcmp_f c) noex

	Arguments:
	op		object pointer
	n		starting number of entries (estimated)
	at		allocation-type:
				0=regular
				1=lookaside
	h		the hash function
	c		the key-comparison function

	Returns:
	<0		error
	>=0		OK

	Note - for storing an item into the DB:

	Note that addresses in the key and the value DATUMs should
	point at memory that is not changed for the life of the
	item in the DB -- if the user wants it to be there when
	they access it!

	A "value" structure MUST always be passed to this subroutine
	from the caller. But the buffer address of a "value" can
	be nullptr.

	Also, zero length values can be stored by specifying a value
	length of zero (buffer address of anything).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* <- |strlen(3c)| + |memcmp(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<lookaside.h>
#include	<strn.h>
#include	<hash.h>
#include	<localmisc.h>

#include	"hdb.h"


/* local defines */

#define HDB_MAGICBAD(op)	((op)->magic == HDB_MAGIC)
#define HDB_MAGICGOOD(op)	((op)->magic == HDB_MAGIC)
#define	HDB_PRBUFLEN		20
#define	HDB_D			DAT
#define HDB_KE			struct hdb_ke
#define HDB_VE			struct hdb_ve

#define	DAT			hdb_datum
#define	ENT			hdb_ent
#define	CUR			hdb_cur

#define	ENTRYINFO		struct entryinfo
#define	FETCUR			struct fetchcur


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef uint		hdbhv ;		/* HDB hash-value */

extern "C" {
    typedef uint (*fc_f)(cvoid *,int) noex ;
}


/* external subroutines */

extern "C" {
    int		hdb_delall(hdb *) noex ;
    int		hdb_fetchrec(hdb *,DAT,CUR *,DAT *,DAT *) noex ;
    int		hdb_curenum(hdb *,CUR *,DAT *,DAT *) noex ;
}


/* external variables */


/* local structures */

#ifdef	COMMENT

struct hdb_ke {
	HDB_KE		*next ;		/* next in hash chain */
	HDB_VE		*same ;		/* next w/ same key */
	uint		hv ;		/* hash-value of key */
} ;

struct hdb_ve {
	HDB_VE		*same ;		/* next w/ same key */
	DAT		key ;
	DAT		val ;
} ;

#endif /* COMMENT */

struct fetchcur {
	fc_f		hfp ;
	CUR		*curp ;
	uint		hv ;
	int		htl ;
	int		f_ikeyed:1 ;
	int		f_jkeyed:1 ;
} ;

struct entryinfo {
	ENT		*pjep ;
	ENT		*pkep ;
	ENT		*ep ;
} ;


/* forward references */

template<typename ... Args>
static inline int hdb_ctor(hdb *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_NOMEM ;
	    op->magic = 0 ;
	    op->htaddr = nullptr ;
	    op->htlen = 0 ;
	    op->count = 0 ;
	    op->at = 0 ;
	    if ((op->esp = new(nothrow) lookaside) != nullptr) {
		rs = SR_OK ;
	    }
	} /* end if (non-null) */
	return rs ;
}

static inline int hdb_dtor(hdb *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->esp) {
		delete op->esp ;
		op->esp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}

static int	hdb_entnew(hdb *,ENT **) noex ;
static int	hdb_entdel(hdb *,ENT *) noex ;

static int	hdb_findkeye(hdb *,FETCUR *,HDB_D *,ENT **) noex ;
static int	hdb_get(hdb *,int,DAT,CUR *,HDB_D *,DAT *) noex ;

static int	hdb_ext(hdb *) noex ;
static int	hdb_extinsert(hdb *,ENT **,int,ENT *) noex ;
static int	hdb_extkeyfree(hdb *,ENT *) noex ;
static int	hdb_getentry(hdb *,ENTRYINFO *,CUR *) noex ;

static int	entry_load(ENT *,uint,HDB_D *,DAT *) noex ;
static int	entry_match(ENT *,hdbcmp_f,uint,HDB_D *) noex ;

static int	cursor_stabilize(CUR *) noex ;
static int	cursor_inc(CUR *) noex ;

static int	fetchcur_start(FETCUR *,fc_f,int,CUR *,HDB_D *) noex ;
static int	fetchcur_adv(FETCUR *) noex ;

static ENT	**getpoint(hdb *,uint,HDB_D *) noex ;

static inline int defcmpfun(cvoid *s1,cvoid *s2,int sl) noex {
	return memcmp(s1,s2,sl) ;
}

static inline int voidlen(cvoid *buf) noex {
	cchar	*s = charp(buf) ;
	return strlen(s) ;
}

template<typename ... Args>
static inline int hdb_magic(hdb *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == HDB_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}

consteval CUR mkcurnull() noex {
	CUR	c{} ;
	c.i = -1 ;
	return c ;
}

consteval DAT mkdatnull() noex {
	DAT	d{} ;
	return d ;
}

static uint defhashfun(cvoid *vp,int vl) noex {
	cchar		*cp = charp(vp) ;
	return hash_elf(cp,vl) ;
}


/* local variables */

static const DAT	nulldatum = mkdatnull() ;

static const CUR	icur = mkcurnull() ;


/* exported variables */


/* exported subroutines */

int hdb_start(hdb *op,int n,int at,hdbhash_f h,hdbcmp_f c) noex {
	int		rs ;
	if (n < HDB_DEFSIZE) n = HDB_DEFSIZE ;
	if ((rs = hdb_ctor(op)) >= 0) {
	    cint	esize = sizeof(ENT) ;
	    op->at = at ;
	    if (op->at > 0) {
	        cint	lan = max((n/6),6) ;
	        rs = lookaside_start(op->esp,esize,lan) ;
	    }
	    if (rs >= 0) {
		cint	tsize = (n * esize) ;
	        void	*vp{} ;
	        if ((rs = uc_malloc(tsize,&vp)) >= 0) {
	            ENT		**hepp = (ENT **) vp ;
	            memclear(hepp,tsize) ;
	            op->htlen = n ;
	            op->hashfunc = (h) ? h : defhashfun ;
	            op->cmpfunc = (c) ? c : defcmpfun ;
	            op->htaddr = hepp ;
	            op->count = 0 ;
	            op->magic = HDB_MAGIC ;
	        } /* end if (memory-allocation) */
	        if (rs < 0) {
	            if (op->at > 0) lookaside_finish(op->esp) ;
	        }
	    } /* end if (ok) */
	    if (rs < 0) {
		hdb_dtor(op) ;
	    }
	} /* end if (hdn_ctor) */
	return rs ;
}
/* end subroutine (hdb_start) */

int hdb_finish(hdb *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = hdb_magic(op)) >= 0) {
		{
	            rs1 = hdb_delall(op) ;
	            if (rs >= 0) rs = rs1 ;
		}
	        if (op->htaddr) {
	            rs1 = uc_free(op->htaddr) ;
	            if (rs >= 0) rs = rs1 ;
	            op->htaddr = nullptr ;
		}
	        if (op->at > 0) {
	            rs1 = lookaside_finish(op->esp) ;
	            if (rs >= 0) rs = rs1 ;
		}
	    {
	        rs1 = hdb_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (hdb_finish) */

int hdb_delall(hdb *op) noex {
	int		rs ;
	if ((rs = hdb_magic(op)) >= 0) {
	        ENT	**hepp = op->htaddr ;
	        for (int idx = 0 ; idx < op->htlen ; idx += 1) {
	            ENT	*hp, *next, *nhp, *nnhp ;
	            for (hp = hepp[idx] ; hp ; hp = next) {
	                if (hp->same != nullptr) {
	                    nhp = hp->same ;
	                    while (nhp->same != nullptr) {
	                        nnhp = nhp->same ;
	                        hdb_entdel(op,nhp) ;
	                        nhp = nnhp ;
	                    } /* end while */
	                    hdb_entdel(op,nhp) ;
	                } /* end if (freeing intermediate entries) */
	                next = hp->next ;
	                hp->next = nullptr ;	/* optional */
	                hdb_entdel(op,hp) ;
	            } /* end for */
	            hepp[idx] = nullptr ;
	        } /* end for */
	        op->count = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (hdb_delall) */

int hdb_store(hdb *op,HDB_D key,DAT val) noex {
	int		rs ;
	if ((rs = hdb_magic(op,key.buf)) >= 0) {
	        if (key.len < 0) {
	            key.len = voidlen(key.buf) ;
	        }
	        if (val.buf && (val.len < 0)) {
	            key.len = voidlen(key.buf) ;
	        }
	        if (op->count >= (op->htlen * 1)) {
	            rs = hdb_ext(op) ;
	        }
	        if (rs >= 0) {
	            ENT		*ep{} ;
	            if ((rs = hdb_entnew(op,&ep)) >= 0) {
	                ENT	*ohp ;
	                uint	hv = (*op->hashfunc)(key.buf,key.len) ;
	                if ((rs = entry_load(ep,hv,&key,&val)) >= 0) {
		            ENT	**nextp = getpoint(op,hv,&key) ;
	                    ohp = *nextp ;
	                    if (ohp) {
	                        while (ohp->same) {
	                            ohp = ohp->same ;
	                        }
	                        ohp->same = ep ;
	                    } else {
	                        *nextp = ep ;	/* append to hash chain */
	                    } /* end if */
	                    op->count += 1 ;
	                } else {
	                    hdb_entdel(op,ep) ;
	                }
	            } /* end if (hdb_entnew) */
	        } /* end if (ok) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (hdb_store) */

/* delete all entries with a specified key */

/****

	This subroutine deletes all entries with the specified "key".
	We return the number of entries deleted.

****/

int hdb_delkey(hdb *op,HDB_D key) noex {
	int		rs ;
	int		n = 0 ;
	if ((rs = hdb_magic(op,key.buf)) >= 0) {
	        ENT	*hp ;
	        ENT	**nextp ;
	        uint	hv = (*op->hashfunc)(key.buf,key.len) ;
	        if (key.len < 0) {
	            key.len = voidlen(key.buf) ;
	        }
/* get the point of deletion (if there is one) */
	        nextp = getpoint(op,hv,&key) ;
	        hp = *nextp ;
	        if (hp != nullptr) {
	            int	ocount = op->count ;
/* unlink this entry from the chain */
	            *nextp = hp->next ;			/* skip this entry */
	            hp->next = nullptr ;
/* OK, we are isolated now from the chain */
	            while (hp->same != nullptr) {
	        	ENT	*nhp = hp->same ;
	                op->count -= 1 ;
	                hdb_entdel(op,hp) ;
	                hp = nhp ;
	            } /* end while */
	            op->count -= 1 ;
	            hdb_entdel(op,hp) ;
	            n = (ocount - op->count) ;
	        } else {
	            rs = SR_NOTFOUND ;
	        }
	} /* end if (magic) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (hdb_delkey) */

/* delete an entry by its cursor */

/****
	This subroutine deletes one entry specified by the given
	cursor.  The cursor is set so that further fetches from the
	same cursor will work properly (the cursor is moved back)!

	f_adv=0		cursor backs up
	f_adv=1		cursor advances to net entry

****/

int hdb_delcur(hdb *op,CUR *curp,int f_adv) noex {
	int		rs ;
	if ((rs = hdb_magic(op,curp)) >= 0) {
	        ENTRYINFO	ei{} ;
	        cursor_stabilize(curp) ;
	        if ((rs = hdb_getentry(op,&ei,curp)) >= 0) {
	            CUR		ncur = *curp ;
	            ENT		**hepp = op->htaddr ;
	            ENT		*ep = ei.ep ;
	            ENT		*pjep = ei.pjep ;
	            ENT		*pkep = ei.pkep ;
	            int		i, j ;
        /* determine any necessary cursor adjustment */
	            if (f_adv) {
	                if (ep->same == nullptr) { /* code-reviewed */
	                    ncur.k = 0 ;
	                    if (ep->next == nullptr) {
				cint	htlen = op->htlen ;
	                        ncur.j = 0 ;
	                        for (i = (ncur.i + 1) ; i < htlen ; i += 1) {
	                            if (hepp[i] != nullptr) break ;
	                        }
	                        ncur.i = i ;
	                    } else {
	                        ncur.j += 1 ;
		            }
	                } /* end if */
	            } else {
	                if (curp->k == 0) {
	            	    ENT		*pep{} ;
	                    int		k ;
	                    if (curp->j == 0) {
	                        ncur.j = 0 ;
	                        ncur.k = 0 ;
	                        for (i = (curp->i - 1) ; i >= 0 ; i -= 1) {
	                            if (hepp[i] != nullptr) break ;
	                        }
	                        if (i >= 0) {
	                            pep = hepp[i] ;
	                            for (j = 0 ; pep->next ; j += 1) {
	                                pep = pep->next ;
	                            }
	                            ncur.j = j ;
	                            for (k = 0 ; pep->same ; k += 1) {
	                                pep = pep->same ;
	                            }
	                            ncur.k = k ;
	                        }
	                        ncur.i = i ;
	                    } else {
	                        ncur.j = (curp->j - 1) ;
	                        pep = pjep ;
	                        for (k = 0 ; pep->same ; k += 1) {
	                            pep = pep->same ;
	                        }
	                        ncur.k = k ;
	                    }
	                } else {
	                    ncur.k = (curp->k - 1) ;
	                }
	            } /* end if (cursor disposition) */
        /* do all necessary list pointer adjustments */
	            if (curp->k == 0) { /* code-reviewed */
	                i = curp->i ;
	                if (curp->j == 0) {
	                    if (ep->same != nullptr) {
	                        (ep->same)->next = ep->next ;
	                        hepp[i] = ep->same ;
	                    } else {
	                        hepp[i] = ep->next ;
	                    }
	                } else {
	                    if (ep->same != nullptr) {
	                        (ep->same)->next = ep->next ;
	                        pjep->next = ep->same ;
	                    } else {
	                        pjep->next = ep->next ;
		            }
	                } /* end if */
	            } else {
	                pkep->same = ep->same ;
	            }
        /* update cursor */
	            *curp = ncur ;
        /* delete the entry itself (return to free-memory pool) */
	            op->count -= 1 ;
	            rs = hdb_entdel(op,ep) ;
	        } /* end if (hdb_getentry) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (hdb_delcur) */

/* advance the cursor to the next entry matching the key-cursor pair */
int hdb_nextrec(hdb *op,HDB_D key,CUR *curp) noex {
	return hdb_fetchrec(op,key,curp,nullptr,nullptr) ;
}
/* end subroutine (hdb_nextrec) */

/* subroutine to fetch the next data corresponding to a key */
int hdb_fetch(hdb *op,HDB_D key,CUR *curp,DAT *valp) noex {
	return hdb_fetchrec(op,key,curp,nullptr,valp) ;
}
/* end subroutine (hdb_fetch) */

/* fetch the next whole record by key-cursor combination */
int hdb_fetchrec(hdb *op,DAT key,CUR *curp,DAT *keyp,DAT *valp) noex {
	int		rs ;
	if ((rs = hdb_magic(op,curp,key.buf)) >= 0) {
	        FETCUR		fc ;
	        CUR		ncur ;
	        int		htlen = op->htlen ;
		hdbhash_f	hf = op->hashfunc ;
	        if (key.len < 0) {
	            key.len = voidlen(key.buf) ;
	        }
	        if (curp) {
	            ncur = *curp ;
	        } else {
	            ncur = icur ;
	        }
	        if ((rs = fetchcur_start(&fc,hf,htlen,&ncur,&key)) >= 0) {
	            ENT		*ep{} ;
	            rs = hdb_findkeye(op,&fc,&key,&ep) ;
	            if ((rs == SR_NOTFOUND) && (fetchcur_adv(&fc) >= 0)) {
	                rs = hdb_findkeye(op,&fc,&key,&ep) ;
	            }
	            if (rs >= 0) {
	                if (curp != nullptr) *curp = ncur ;
	                if (ep) {
	                    if (keyp) *keyp = ep->key ;
	                    if (valp) *valp = ep->val ;
	                }
	            }
	        } /* end if (fetchcur_start) */
	        if (rs < 0) {
	            if (keyp) *keyp = nulldatum ;
	            if (valp) *valp = nulldatum ;
	        } /* end if */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (hdb_fetchrec) */

/* get the current record under the CURRENT cursor regardless of key */
int hdb_getrec(hdb *op,CUR *curp,DAT *keyp,DAT *valp) noex {
	return hdb_get(op,false,nulldatum,curp,keyp,valp) ;
}
/* end subroutine (hdb_getrec) */

/* get the current record under the cursor but only if it matches the key */
int hdb_getkeyrec(hdb *op,DAT key,CUR *curp,DAT *keyp,DAT *valp) noex {
	return hdb_get(op,true,key,curp,keyp,valp) ;
}
/* end subroutine (hdb_getkeyrec) */

/* advance the cursor to the next entry regardless of key */
int hdb_next(hdb *op,CUR *curp) noex {
	return hdb_curenum(op,curp,nullptr,nullptr) ;
}
/* end subroutine (hdb_next) */

/* subroutine to enumerate all entries */

/****
	This subroutine will return all entries in the DB using the
	given cursor to sequence through it all.  Specifically, it
	differs from |hdb_getrec()| in that it "fetches" the NEXT
	entry after the one under the current cursor!  It then
	updates the cursor to the returned entry.  This returns
	'SR_NOTFOUND' on exhausting all entries.

****/

int hdb_curenum(hdb *op,CUR *curp,HDB_D *keyp,DAT *valp) noex {
	int		rs ;
	if ((rs = hdb_magic(op,curp)) >= 0) {
	        CUR	ncur ;
	        ENT	*ep = nullptr ;
                ENT	**hepp = op->htaddr ;
		rs = SR_NOTFOUND ;
	        if (curp != nullptr) {
	            ncur = *curp ;
	        } else {
	            ncur = icur ;
	        }
	        cursor_inc(&ncur) ;
	        while (ncur.i < op->htlen) {
	            ENT		*jep ;
	            if ((jep = hepp[ncur.i]) != nullptr) {
		        int	j = 0 ; /* <- used afterwards */
	                for (j = 0 ; j < ncur.j ; j += 1) {
	                    if (jep->next == nullptr) break ;
	                    jep = jep->next ;
	                } /* end for */
	                if (j == ncur.j) {
	                    while (jep != nullptr) {
			        int	k = 0 ;	/* <- used afterwards */
	                        ep = jep ;
	                        for (k = 0 ; k < ncur.k ; k += 1) {
	                            if (ep->same == nullptr) break ;
	                            ep = ep->same ;
	                        } /* end for */
	                        if (k == ncur.k) {
	                            rs = SR_OK ;
	                            break ;
	                        }
	                        jep = jep->next ;
	                        ncur.j += 1 ;
	                        ncur.k = 0 ;
	                    } /* end while */
	                    if (rs >= 0) break ;
	                } /* end if */
	            } /* end if (have_i) */
	            ncur.i += 1 ;
	            ncur.j = 0 ;
	            ncur.k = 0 ;
	        } /* end while */
	        if ((rs >= 0) && ep) {
	            if (curp) *curp = ncur ;
	            if (keyp) *keyp = ep->key ;
	            if (valp) *valp = ep->val ;
	        } /* end if (found an entry) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (hdb_curenum) */

/* count of items in container */
int hdb_count(hdb *op) noex {
	int		rs ;
	if ((rs = hdb_magic(op)) >= 0) {
		rs = op->count ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (hdb_count) */

int hdb_curbegin(hdb *op,CUR *curp) noex {
	int		rs ;
	if ((rs = hdb_magic(op,curp)) >= 0) {
		*curp = icur ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (hdb_curbegin) */

int hdb_curend(hdb *op,CUR *curp) noex {
	int		rs ;
	if ((rs = hdb_magic(op,curp)) >= 0) {
		*curp = icur ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (hdb_curend) */

int hdb_hashtablen(hdb *op,uint *rp) noex {
	int		rs ;
	if ((rs = hdb_magic(op,rp)) >= 0) {
		*rp = op->htlen ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (hdb_hashtablen) */

int hdb_hashtabcounts(hdb *op,int *rp,int n) noex {
	int		rs ;
	int		tc = 0 ;
	if ((rs = hdb_magic(op,rp)) >= 0) {
		rs = SR_INVALID ;
	        if (n >= op->htlen) {
	            ENT		**hepp ;
	            ENT		*hp ;
	            int		hi ;
		    rs = SR_OK ;
	            for (hi = 0 ; hi < op->htlen ; hi += 1) {
	                int	c = 0 ;
	                hepp = (op->htaddr + hi) ;
	                for (hp = *hepp ; hp ; hp = hp->next) {
	                    c += 1 ;
	                    if (hp->same) {
	                        ENT	*sep ;
	                        for (sep = hp->same ; sep ; sep = sep->same) {
	                            c += 1 ;
	                        }
	                    } /* end if (same keys) */
	                } /* end for */
	                rp[hi] = c ;
	                tc += c ;
	            } /* end for */
	        } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? tc : rs ;
}
/* end subroutine (hdb_hashtabcounts) */

int hdb_audit(hdb *op) noex {
	int		rs ;
	int		n = 0 ;
	if ((rs = hdb_magic(op)) >= 0) {
		ENT	**hepp = op->htaddr ;
	        for (int i = 0 ; i < op->htlen ; i += 1) {
	            ENT		*ep ;
	            int		j = 0 ;
	            for (ep = hepp[i] ; ep ; (ep = ep->next),(j += 1)) {
	                n += 1 ;
	                while ((rs >= 0) && ep->same) {
	                    n += 1 ;
	                    ep = ep->same ;
	                } /* end while */
	                if (rs < 0) break ;
	            } /* end for */
	            if (rs < 0) break ;
	        } /* end for */
	        if (rs >= 0) {
	            if (n != op->count) {
	                rs = SR_BADFMT ;
	            }
	        }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (hdb_audit) */


/* private subroutines */

static int hdb_entnew(hdb *op,ENT **epp) noex {
	int		rs ;
	if (op->at == 0) {
	    cint	esize = sizeof(ENT) ;
	    rs = uc_malloc(esize,epp) ;
	} else {
	     rs = lookaside_get(op->esp,epp) ;
	}
	return rs ;
}
/* end subroutine (hdb_entnew) */

static int hdb_entdel(hdb *op,ENT *ep) noex {
	int		rs = SR_OK ;
	if (op->at == 0) {
	    rs = uc_free(ep) ;
	} else {
	    rs = lookaside_release(op->esp,ep) ;
	}
	return rs ;
}
/* end subroutine (hdb_entdel) */

static int hdb_findkeye(hdb *op,FETCUR *fcp,HDB_D *kp,ENT **epp) noex {
	CUR		*curp = fcp->curp ;
	ENT		*jep ;
	int		rs = SR_NOTFOUND ;
	int		rs1 ;
	if ((jep = op->htaddr[curp->i]) != nullptr) {
	    int		i = 0 ;	/* used afterwards */
	    fcp->f_ikeyed = true ;
	    for (i = 0 ; i < curp->j ; i += 1) {
	        if (jep->next == nullptr) break ;
	        jep = jep->next ;
	    } /* end for */
	    if (i == curp->j) {
	        for ( ; jep != nullptr ; jep = jep->next) {
	            rs1 = entry_match(jep,op->cmpfunc,fcp->hv,kp) ;
	            if (rs1 > 0) {
	                rs = SR_OK ;
	                break ;
	            }
	            curp->j += 1 ;
	        } /* end for */
	    } /* end if */
	    if (rs >= 0) {
	        ENT	*ep = jep ;
	        fcp->f_jkeyed = true ;
	        for (i = 0 ; i < curp->k ; i += 1) {
	            if (ep->same == nullptr) break ;
	            ep = ep->same ;
	        } /* end for */
	        if (i < curp->k) {
	            rs = SR_NOTFOUND ;
		}
	        if ((rs >= 0) && epp) {
	            *epp = ep ;
		}
	    } /* end if (j-keyed) */
	} /* end if (was found) */
	return rs ;
}
/* end subroutine (hdb_findkeye) */

static int hdb_get(hdb *op,int f,DAT key,CUR *curp,DAT *keyp,DAT *valp) noex {
	int		rs = SR_FAULT ;
	if (op && curp && keyp) {
	    rs = SR_NOTOPEN ;
	    if (HDB_MAGICGOOD(op)) {
		rs = SR_OK ;
	        if ((!f) || key.buf) {
	            ENTRYINFO	ei{} ;
	            cursor_stabilize(curp) ;
	            if ((rs = hdb_getentry(op,&ei,curp)) >= 0) {
	                ENT	*ep = ei.ep ;
	                if (f && (key.buf != nullptr)) {
	                    int 	rc = 1 ;
	                    if (key.len == ep->key.len) {
			        cvoid	*v1 = key.buf ;
				cvoid	*v2 = ep->key.buf ;
	                        rc = (*op->cmpfunc)(v1,v2,key.len) ;
	                    }
	                    if (rc != 0) rs = SR_NOTFOUND ;
	                } /* end if (key comparison) */
	                if (rs >= 0) {
	                    if (keyp != nullptr) *keyp = ep->key ;
	                    if (valp != nullptr) *valp = ep->val ;
	                }
	            } /* end if (entry found) */
	            if (rs < 0) {
	                if (keyp != nullptr) *keyp = nulldatum ;
	                if (valp != nullptr) *valp = nulldatum ;
	            }
	        } /* end if (valid) */
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (hdb_get) */

static int hdb_ext(hdb *op) noex {
	ENT		**htaddr = op->htaddr ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		nhtlen = (op->htlen * 2) ;
	int		sz ;
	void		*vp{} ;
	sz = nhtlen * sizeof(ENT *) ;
	if ((rs = uc_malloc(sz,&vp)) >= 0) {
	    int		i = 0 ; /* <- used later */
	    ENT		*hep, *nhep ;
	    ENT		**nhtaddr = (ENT **) vp ;
	    memclear(nhtaddr,sz) ;
	    for (i = 0 ; i < op->htlen ; i += 1) {
	        if (htaddr[i] != nullptr) {
	            for (hep = htaddr[i] ; hep ; hep = nhep) {
	                nhep = hep->next ;
	                rs = hdb_extinsert(op,nhtaddr,nhtlen,hep) ;
	                if (rs < 0) break ;
	            } /* end for */
	            if (rs >= 0) {
	                for (hep = htaddr[i] ; hep ; hep = nhep) {
	                    nhep = hep->next ;
	                    hdb_entdel(op,hep) ;
	                } /* end for */
	            }
	            if (rs < 0) break ;
		} /* end if (non-null) */
	    } /* end for */
	    if (rs >= 0) {
	        uc_free(op->htaddr) ;
	        op->htaddr = nhtaddr ;
	        op->htlen = nhtlen ;
	    } else {
	        cint	n = min(i,nhtlen) ;
	        for (i = 0 ; i < n ; i += 1) {
	            if (nhtaddr[i] != nullptr) {
	                for (hep = nhtaddr[i] ; hep ; hep = nhep) {
	                    nhep = hep->next ;
	                    hdb_extkeyfree(op,hep) ;
	                } /* end for */
	            }
	        } /* end for */
	        rs1 = uc_free(nhtaddr) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end block (success) */
	} /* end if (m-a) */
	return rs ;
}
/* end subroutine (hdb_extend) */

static int hdb_extinsert(hdb *op,ENT **htaddr,int htlen,ENT *hep) noex {
	ENT		*nhep = nullptr ;
	int		rs ;
	if ((rs = hdb_entnew(op,&nhep)) >= 0) {
	    hdbhv	hv = hep->hv ;
	    int		hi ;
/* copy existing to new */
	    *nhep = *hep ;		/* memberwise copy */
	    nhep->next = nullptr ;		/* this is newly determined */
/* determine how to insert the new entry */
	    hi = (hv % htlen) ;
	    if (htaddr[hi] != nullptr) {
		ENT	*ep = htaddr[hi] ;
	        htaddr[hi] = nhep ;
	        nhep->next = ep ;
	    } else {
	        htaddr[hi] = nhep ;
	    }
	} /* end if (hdb_entnew) */
	return rs ;
}
/* end subroutine (hdb_extinsert) */

static int hdb_extkeyfree(hdb *op,ENT *shep) noex {
	ENT		*nhep{} ;
	int		rs = SR_OK ;
	for (ENT *hep = shep ; hep ; hep = nhep) {
	    nhep = hep->same ;
	    hdb_entdel(op,hep) ;
	} /* end for */
	return rs ;
}
/* end subroutine (hdb_extkeyfree) */

static int hdb_getentry(hdb *op,ENTRYINFO *eip,CUR *curp) noex {
	int		rs = SR_OK ;
	int		i = curp->i ;
	if ((i >= 0) && (i < op->htlen)) {
	    ENT		**hepp = op->htaddr ;
	    ENT		*pjep = nullptr ;
	    ENT		*pkep = nullptr ;
	    ENT		*ep = hepp[i] ;
	    bool	f_notdone = true ;
	    while ((i < op->htlen) && ((ep = hepp[i]) == nullptr)) {
	        i += 1 ;
	    }
	    if (curp->i != i) {
	        curp->i = i ;
	        curp->j = 0 ;
	        curp->k = 0 ;
	        if (ep) f_notdone = false ;
	    }
	    if (f_notdone) {
	        if (ep != nullptr) {
		    int	j ; /* find pointers to this cursor entry */
	            for (j = 0 ; j < curp->j ; j += 1) { /* code-reviewed */
	                if (ep->next == nullptr) break ;
	                pjep = ep ;
	                ep = ep->next ;
	            } /* end for */
	            if (j < curp->j) {
	                curp->k = 0 ;
	                curp->j = 0 ;
	                curp->i += 1 ;
	                rs = hdb_getentry(op,eip,curp) ;
	            }
	            if (rs >= 0) { 
			int	k ; /* code-reviewed */
	                for (k = 0 ; k < curp->k ; k += 1) {
	                    if (ep->same == nullptr) break ;
	                    pkep = ep ;
	                    ep = ep->same ;
	                } /* end for */
	                if (k < curp->k) {
	                    curp->k = 0 ;
	                    curp->j += 1 ;
	                    rs = hdb_getentry(op,eip,curp) ;
	                }
	                if (rs >= 0) {
	                    eip->pjep = pjep ;
	                    eip->pkep = pkep ;
	                    eip->ep = ep ;
	                }
	            } /* end if (ok) */
	        } else {
	            rs = SR_NOTFOUND ;
	        } /* end if (no-null) */
	    } /* end if (f_notdone) */
	} else {
	    rs = SR_NOTFOUND ;
	} /* end if (in bounds) */
	return rs ;
}
/* end subroutine (hdb_getentry) */

static int entry_load(ENT *ep,hdbhv hv,DAT *keyp,DAT *valp) noex {
	int		rs = SR_FAULT ;
	if (ep && keyp && valp) {
	    rs = memclear(ep) ;
	    ep->next = nullptr ;
	    ep->same = nullptr ;
	    ep->hv = hv ;
	    ep->key = *keyp ;
	    ep->val = *valp ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entry_load) */

static int entry_match(ENT *ep,hdbcmp_f cmpfunc,uint hv,DAT *keyp) noex {
	int		ekeylen = ep->key.len ;
	int		f = true ;
	cvoid		*ekeybuf = ep->key.buf ;
	f = f && (ep->hv == hv) ;
	f = f && (ekeylen == keyp->len) ;
	f = f && ((*cmpfunc)(ekeybuf,keyp->buf,keyp->len) == 0) ;
	return f ;
}
/* end subroutine (entry_match) */

static int cursor_stabilize(CUR *curp) noex {
	int		rs = SR_FAULT ;
	if (curp) {
	    rs = SR_OK ;
	    if (curp->i >= 0) {
	        if (curp->j < 0) {
	            curp->j = 0 ;
	            curp->k = 0 ;
	        } else {
	            if (curp->k < 0) {
	                curp->k = 0 ;
	            }
	        }
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (cursor_stabilize) */

static int cursor_inc(CUR *curp) noex {
	int		rs = SR_FAULT ;
	if (curp) {
	    rs = SR_OK ;
	    if (curp->i < 0) {
	        curp->i = 0 ;
	        curp->j = 0 ;
	        curp->k = 0 ;
	    } else {
	        if (curp->j < 0) {
	            curp->j = 0 ;
	            curp->k = 0 ;
	        } else {
	            if (curp->k <= 0) {
	                curp->k = 1 ;
	            } else {
	                curp->k += 1 ;
	            }
	        }
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (cursor_inc) */

static int fetchcur_start(FETCUR *fcp,fc_f hfp,int htl,CUR *curp,DAT *kp) noex {
	int		rs = SR_NOTFOUND ;
	if (curp->i < htl) {
	    rs = SR_OK ;
	    fcp->hfp = hfp ;
	    fcp->htl = htl ;
	    fcp->curp = curp ;
	    fcp->f_ikeyed = false ;
	    fcp->f_jkeyed = false ;
	    fcp->hv = (*fcp->hfp)(kp->buf,kp->len) ;
	    if (curp->i < 0) {
	        curp->i = fcp->hv % fcp->htl ;
	        fcp->f_ikeyed = true ;
	        curp->j = 0 ;
	        curp->k = 0 ;
	    } else if (curp->j < 0) {
	        curp->j = 0 ;
	        curp->k = 0 ;
	    } else if (curp->k <= 0) {
	        curp->k = 1 ;
	    } else {
	        curp->k += 1 ;
	    }
	}
	return rs ;
}
/* end subroutine (fetchcur_start) */

static int fetchcur_adv(FETCUR *fcp) noex {
	int		rs = SR_ALREADY ;
	int		idx = 0 ;
	if (! fcp->f_ikeyed) {
	    CUR		*curp = fcp->curp ;
	    cint	ci = (fcp->hv % fcp->htl) ;
#ifdef	COMMENT /* the hash on the key is always calculated before here */
	    fcp->hv = (*fcp->hfp)(kp->buf,kp->len) ;
#endif
	    if (curp->i < ci) {
	        rs = SR_OK ;
	        curp->i = ci ;
	        curp->j = 0 ;
	        curp->k = 0 ;
	        fcp->f_ikeyed = true ;
	        fcp->f_jkeyed = false ;
	    }
	    idx = curp->i ;
	} /* end if (needed) */
	return (rs >= 0) ? idx : rs ;
}
/* end subroutine (fetchcur_adv) */

/* see if an entry is in the database */

/****
	The returned value is the address of the pointer that refers
	to the found object.  The pointer may be nullptr if the object
	was not found.  If so, this pointer should be updated with
	the address of the object to be inserted, if insertion is
	desired.

****/

static ENT **getpoint(hdb *op,uint hv,HDB_D *keyp) noex {
	ENT		*ep ;
	ENT		**hepp ;
	ENT		*pep = nullptr ;
	int		hi = (hv % op->htlen) ;
	int		keylen = keyp->len ;
	cchar		*keydat = charp(keyp->buf) ;
	hepp = (op->htaddr + hi) ;
	for (ep = *hepp ; ep ; ep = ep->next) {
	    cchar	*hpkeydat = charp(ep->key.buf) ;
	    bool	f = true ;
	    if (hpkeydat) {
	        f = f && (ep->hv == hv) && (ep->key.len == keylen) ;
	        f = f && ((*op->cmpfunc)(hpkeydat,keydat,keylen) == 0) ;
	        if (f) break ;
	        pep = ep ;
	    }
	} /* end for */
	return (pep) ? &pep->next : hepp ;
}
/* end subroutine (getpoint) */


