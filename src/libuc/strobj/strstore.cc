/* strstore SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* string storeage-table object */
/* version %I% last-modified %G% */

#define	CF_PREALLOC	1		/* pre-allocate a chunk */

/* revision history:

	= 1998-03-24, David A­D­ Morano
	This object module was morphed from some previous one.  I
	do not remember what the previous one was.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strstore

	Description:
	This object module creates and manages a string storage
	object.  This object is sort of a write-only storage area
	for storing strings.  Strings cannot be deleted from the
	object once they are added (something not commonly needed
	anyway).  Since most string storage requirements do NOT
	need the ability to remove strings once they are entered,
	this object provides a nice optimized manager for handling
	that sort of situation.  

	Arguments:
	op		pointer to the strstore object
	<others>

	Returns:
	>=0		the total length of the filled up strstore so far!
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<vecobj.h>
#include	<intceil.h>
#include	<hash.h>
#include	<hashindex.h>
#include	<nextpowtwo.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"strstore.h"

import libutil ;

/* local defines */

#ifndef	CF_PREALLOC
#define	CF_PREALLOC	0
#endif

#define	MODP2(v,n)	((v) & ((n) - 1))


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */

extern "C" {
    typedef int (*vo_f)(vecobj *,int,void **) noex ;
}


/* external subroutines */


/* local structures */

struct strentry {
	uint	khash ;
	uint	hi ;
	uint	si ;
} ;


/* forward references */

int		strstore_store(strstore *,cchar *,int,cchar **) noex ;
int		strstore_already(strstore *,cchar *,int) noex ;

template<typename ... Args>
static inline int strstore_ctor(strstore *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    op->magic = 0 ;
	    op->csp = nullptr ;
	    op->chsize = 0 ;
	    op->totalsize = 0 ;
	    op->c = 0 ;
	    if ((op->clp = new(nothrow) vechand) != np) ylikely {
	        if ((op->nlp = new(nothrow) vechand) != np) ylikely {
	            if ((op->lap = new(nothrow) lookaside) != np) ylikely {
	                if ((op->hlp = new(nothrow) hdb) != np) ylikely {
			    rs = SR_OK ;
	                } /* end if (new-hdb) */
		        if (rs < 0) {
		            delete op->lap ;
		            op->lap = nullptr ;
		        }
	            } /* end if (new-lookaside) */
		    if (rs < 0) {
		        delete op->nlp ;
		        op->nlp = nullptr ;
		    }
	        } /* end if (new-vechand) */
		if (rs < 0) {
		    delete op->clp ;
		    op->clp = nullptr ;
		}
	    } /* end if (new-vechand) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (strstore_ctor) */

static int strstore_dtor(strstore *op) noex {
	int		rs = SR_OK ;
	if (op->hlp) {
	    delete op->hlp ;
	    op->hlp = nullptr ;
	}
	if (op->lap) {
	    delete op->lap ;
	    op->lap = nullptr ;
	}
	if (op->nlp) {
	    delete op->nlp ;
	    op->nlp = nullptr ;
	}
	if (op->clp) {
	    delete op->clp ;
	    op->clp = nullptr ;
	}
	return rs ;
}
/* end subroutine (strstore_dtor) */

template<typename ... Args>
static inline int strstore_magic(strstore *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magic == STRSTORE_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}

static int	strstore_chunknew(strstore *,int) noex ;
static int	strstore_chunkfins(strstore *) noex ;
static int	strstore_manage(strstore *,cchar *,int,int) noex ;

static int	chunk_start(strstore_ch *,int) noex ;
static int	chunk_adv(strstore_ch *) noex ;
static int	chunk_finish(strstore_ch *) noex ;


/* local variables */

constexpr bool		f_prealloc = CF_PREALLOC ;


/* local subroutines */

static inline int indexlen(int n) noex {
	return nextpowtwo(n) ;
}
/* end subroutine (indexlen) */

static inline int indexsize(int il) noex {
	return ((il + 1) * 3 * szof(int)) ;
}
/* end subroutine (indexsize) */


/* exported subroutines */

int strstore_start(strstore *op,int n,int csz) noex {
	int		rs ;
	if (n < STRSTORE_STARTLEN) n = STRSTORE_STARTLEN ;
	if (csz < STRSTORE_CHUNKSIZE) csz = STRSTORE_CHUNKSIZE ;
	if ((rs = strstore_ctor(op)) >= 0) ylikely {
	    vechand	*clp = op->clp ;
	    cint	vo = VECHAND_OORDERED ;
	    cint	nch = max((n/6),6) ;
	    op->chsize = csz ;
	    if ((rs = vechand_start(clp,nch,vo)) >= 0) ylikely {
	        if ((rs = vechand_start(op->nlp,n,vo)) >= 0) ylikely {
		    cint	isz = szof(int) ;
	            if ((rs = lookaside_start(op->lap,isz,n)) >= 0) ylikely {
		        cnullptr	np{} ;
		        cint		hn = ((n*3)/2) ;
	                if ((rs = hdb_start(op->hlp,hn,true,np,np)) >= 0) {
	                    op->magic = STRSTORE_MAGIC ;
			    if_constexpr (f_prealloc) {
	                        rs = strstore_chunknew(op,0) ;
	                        if (rs < 0)  {
	                            hdb_finish(op->hlp) ;
	                            op->magic = 0 ;
	                        }
			    } /* end if_constexpr (f_prealloc) */
	                } /* end if (hdb-start) */
	                if (rs < 0) {
	                    lookaside_finish(op->lap) ;
			}
	            } /* end if (lookaside_start) */
	            if (rs < 0) {
		        vechand_finish(op->nlp) ;
		    }
	        } /* end if (vechand) */
	        if (rs < 0) {
		    vechand_finish(op->clp) ;
		}
	    } /* end if (vechand) */
	    if (rs < 0) {
		strstore_dtor(op) ;
	    }
	} /* end if (strstore_ctor) */
	return rs ;
}
/* end subroutine (strstore_start) */

int strstore_finish(strstore *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = strstore_magic(op)) >= 0) ylikely {
            {
                rs1 = hdb_finish(op->hlp) ;
                if (rs >= 0) rs = rs1 ;
            }
            {
                rs1 = lookaside_finish(op->lap) ;
                if (rs >= 0) rs = rs1 ;
            }
            {
                rs1 = vechand_finish(op->nlp) ;
                if (rs >= 0) rs = rs1 ;
            }
            {
                rs1 = strstore_chunkfins(op) ;
                if (rs >= 0) rs = rs1 ;
            }
            {
                rs1 = vechand_finish(op->clp) ;
                if (rs >= 0) rs = rs1 ;
            }
            {
                rs1 = strstore_dtor(op) ;
                if (rs >= 0) rs = rs1 ;
            }
            op->csp = nullptr ;
            op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (strstore_finish) */

int strstore_add(strstore *op,cchar *sp,int sl) noex {
	return strstore_store(op,sp,sl,nullptr) ;
}
/* end subroutine (strstore_add) */

int strstore_adduniq(strstore *op,cchar *sp,int sl) noex {
	cint		nrs = SR_NOTFOUND ;
	int		rs ;
	if ((rs = strstore_already(op,sp,sl)) == nrs) {
	    rs = strstore_store(op,sp,sl,nullptr) ;
	}
	return rs ;
}
/* end subroutine (strstore_adduniq) */

int strstore_store(strstore *op,cchar *sp,int sl,cchar **rpp) noex {
	int		rs ;
	int		si = 0 ;
	if ((rs = strstore_magic(op,sp)) >= 0) ylikely {
	    if (sl < 0) sl = lenstr(sp) ;
	    {
	        strstore_ch	*cckp = op->csp ;
		cint		amount = (sl + 1) ;
		char		*ep = nullptr ;
	        if ((cckp == nullptr) || (amount > (cckp->csz - cckp->i))) {
	            rs = strstore_chunknew(op,amount) ;
	            cckp = op->csp ;
	        }
	        if (rs >= 0) ylikely {
		    ep = (cckp->cdata + cckp->i) ;
	            si = op->totalsize ;
	            strwcpy(ep,sp,sl) ;
	            if ((rs = vechand_add(op->nlp,ep)) >= 0) {
	                cint	i = rs ;
	                rs = strstore_manage(op,ep,sl,si) ;
	                if (rs < 0) vechand_del(op->nlp,i) ;
	            }
	        } /* end if */
	        if (rs >= 0) ylikely {
	            cckp->i += amount ;
	            cckp->c += 1 ;		/* count in chunk */
	            op->c += 1 ;		/* count in object */
	            op->totalsize += amount ;
	        }
		if (rpp) {
		    cchar	*cep = charp(ep) ;
		    *rpp = (rs >= 0) ? cep : nullptr ;
		}
	    } /* end block */
	} /* end if (magic) */
	return (rs >= 0) ? si : rs ;
}
/* end subroutine (strstore_store) */

int strstore_curbegin(strstore *op,strstore_cur *curp) noex {
	int		rs ;
	if ((rs = strstore_magic(op,curp)) >= 0) ylikely {
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (strstore_curbegin) */

int strstore_curend(strstore *op,strstore_cur *curp) noex {
	int		rs ;
	if ((rs = strstore_magic(op,curp)) >= 0) ylikely {
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (strstore_curend) */

int strstore_enum(strstore *op,strstore_cur *curp,cchar **rpp) noex {
	int		rs ;
	int		cl = 0 ;
	if ((rs = strstore_magic(op,curp,rpp)) >= 0) ylikely {
	        cint	val = (curp->i >= 0) ? (curp->i+1) : 0 ;
		rs = SR_NOTFOUND ;
	        if (val < op->c) ylikely {
		    if (void *vp ; (rs = vechand_get(op->nlp,val,&vp)) >= 0) {
			cchar	*cp = charp(vp) ;
	                cl = lenstr(cp) ;
	                if (rpp) *rpp = cp ;
	                curp->i = val ;
	            }
	        } /* end if */
	} /* end if (magic) */
	return (rs >= 0) ? cl : rs ;
}
/* end subroutine (strstore_enum) */

int strstore_already(strstore *op,cchar *sp,int sl) noex {
	int		rs ;
	int		si = 0 ;
	if ((rs = strstore_magic(op,sp)) >= 0) ylikely {
	    rs = SR_NOTOPEN ;
	    if (sl < 0) sl = lenstr(sp) ;
	        hdb_dat	key, val ;
	        key.buf = sp ;
	        key.len = sl ;
	        if ((rs = hdb_fetch(op->hlp,key,nullptr,&val)) >= 0) {
	            int		*ip = (int *) val.buf ;
	            si = *ip ;
	        } /* end if */
	} /* end if (magic) */
	return (rs >= 0) ? si : rs ;
}
/* end subroutine (strstore_already) */

int strstore_count(strstore *op) noex {
	int		rs ;
	if ((rs = strstore_magic(op)) >= 0) ylikely {
	    rs = op->c ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (strstore_count) */

int strstore_size(strstore *op) noex {
	int		rs ;
	if ((rs = strstore_magic(op)) >= 0) ylikely {
	    rs = op->totalsize ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (strstore_size) */

int strstore_strsize(strstore *op) noex {
	int		rs ;
	if ((rs = strstore_magic(op)) >= 0) ylikely {
	    cint	vsz = szof(int) ;
	    rs = iceil(op->totalsize,vsz) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (strstore_strsize) */

int strstore_strmk(strstore *op,char *tabp,int tabl) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = strstore_magic(op,tabp)) >= 0) ylikely {
		cint	size = iceil(op->totalsize,szof(int)) ;
		rs = SR_OVERFLOW ;
		if (tabl >= size) ylikely {
		    vechand	*vhp = op->clp ;
		    char	*bp = tabp ;
		    void	*vp{} ;
		    rs = SR_OK ;
		    for (int i = 0 ; vechand_get(vhp,i,&vp) >= 0 ; i += 1) {
			strstore_ch	*cckp = (strstore_ch *) vp ;
	    	        if (cckp) {
	        	    if (cckp->cdata) {
	            	        c += 1 ;
	                        memcpy(bp,cckp->cdata,cckp->i) ;
	                        bp += cckp->i ;
	                    }
	    	        }
		    } /* end for */
		    while (bp < (tabp + tabl)) {
	    		*bp++ = '\0' ;
		    }
		} /* end if (sized OK) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (strstore_strmk) */

int strstore_recsize(strstore *op) noex {
	int		rs ;
	if ((rs = strstore_magic(op)) >= 0) ylikely {
	    cint	n = (op->c + 1) ;
	    rs = (n + 1) * szof(int) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (strstore_recsize) */

int strstore_recmk(strstore *op,int *rdata,int rsize) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = strstore_magic(op,rdata)) >= 0) ylikely {
	    cint	n = (op->c + 1) ;
	    {
		cint	sz = (n + 1) * szof(int) ;
		rs = SR_OVERFLOW ;
	        if (rsize >= sz) ylikely {
		    hdb		*hp = op->hlp ;
	            hdb_cur	cur ;
	            hdb_dat	key ;
	            hdb_dat	val ;
	            rdata[c++] = 0 ;	/* ZERO-entry is NUL-string */
	            if ((rs = hdb_curbegin(hp,&cur)) >= 0) {
	                while (hdb_curenum(hp,&cur,&key,&val) >= 0) {
	                    int	*ip = (int *) val.buf ;
	                    rdata[c++] = *ip ;
	                } /* end while (looping through strings) */
	                hdb_curend(hp,&cur) ;
	            } /* end if */
	            rdata[c] = -1 ;
	        } /* end if (valid size) */
	    } /* end block */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (strstore_recmk) */

int strstore_indlen(strstore *op) noex {
	int		rs ;
	if ((rs = strstore_magic(op)) >= 0) ylikely {
	    rs = indexlen(op->c+1) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (strstore_indlen) */

int strstore_indsize(strstore *op) noex {
	int		rs ;
	if ((rs = strstore_magic(op)) >= 0) ylikely {
	    cint il = indexlen(op->c+1) ;
	    rs = indexsize(il) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (strstore_indsize) */

int strstore_indmk(strstore *op,int (*it)[3],int itsize,int nskip) noex {
	int		rs ;
	int		rs1 ;
	int		sc = 0 ;
	if (nskip < 0) nskip = 0 ;
	if ((rs = strstore_magic(op,it)) >= 0) ylikely {
	    cint	il = indexlen(op->c+1) ;
	    rs = SR_OVERFLOW ;
	    if (cint isz = indexsize(il) ; itsize >= isz) ylikely {
                cint	esz = szof(strentry) ;
		cint	vn = op->c ;
                cint	vo = VECOBJ_OCOMPACT ;
                memset(it,0,isz) ;
                if (vecobj ses ; (rs = vecobj_start(&ses,esz,vn,vo)) >= 0) {
                    strentry        se ;
                    hdb             *hp = op->hlp ;
                    hdb_cur         cur ;
                    hdb_dat         key ;
                    hdb_dat         val ;
                    uint            khash, chash, nhash ;
                    int             lhi, nhi, hi, si ;
                    if ((rs = hdb_curbegin(hp,&cur)) >= 0) {
                        auto        he = hdb_curenum ;
                        while ((rs1 = he(hp,&cur,&key,&val)) >= 0) {
                            cint    sl = key.len ;
                            int     *ip = intp(val.buf) ;
                            cchar   *sp = charp(key.buf) ;
                            si = *ip ;
                            khash = hash_elf(sp,sl) ;
                            hi = hashindex(khash,il) ;
                            if (it[hi][0] == 0) {
                                it[hi][0] = si ;
                                it[hi][1] = int(khash & INT_MAX) ;
                                it[hi][2] = 0 ;
                                sc += 1 ;
                            } else {
                                se.khash = khash ;
                                se.si = si ;
                                se.hi = hi ;
                                rs = vecobj_add(&ses,&se) ;
                            } /* end if */
                            if (rs < 0) break ;
                        } /* end while (looping through strings) */
                        if (rs1 != SR_NOTFOUND) rs = rs1 ;
                        rs1 = hdb_curend(hp,&cur) ;
                        if (rs >= 0) rs = rs1 ;
                    } /* end if (cursor) */
                    if (rs >= 0) ylikely {
                        auto        v = vecobj_get ;
                        void        *vp{} ;
                        for (int i = 0 ; v(&ses,i,&vp) >= 0 ; i += 1) {
                            strentry        *sep = (strentry *) vp ;
                            int             c = 0 ;
                            khash = sep->khash ;
                            si = sep->si ;
                            hi = sep->hi ;
                            chash = (khash & INT_MAX) ;
                            nhash = khash ;
                            while (it[hi][0] > 0) {
                                if ((it[hi][1] & INT_MAX) == chash) break ;
                                it[hi][1] |= (~ INT_MAX) ;
                                nhash = hash_again(nhash,c,nskip) ;
                                hi = hashindex(nhash,il) ;
                                c += 1 ;
                            } /* end while */
                            sc += c ;
                            if (it[hi][0] > 0) {
                                lhi = hi ;
                                while ((nhi = it[lhi][2]) > 0) {
                                    lhi = nhi ;
                                }
                                hi = hashindex((lhi + 1),il) ;
                                while (it[hi][0] > 0) {
                                    hi = hashindex((hi + 1),il) ;
                                }
                                it[lhi][2] = hi ;
                            } /* end while */
                            it[hi][0] = si ;
                            it[hi][1] = chash ;
                            it[hi][2] = 0 ;
                        } /* end for */
                        it[il][0] = -1 ;
                        it[il][1] = 0 ;
                        it[il][2] = 0 ;
                        if (sc < 0) sc = 0 ;
                    } /* end if */
                    rs1 = vecobj_finish(&ses) ;
                    if (rs >= 0) rs = rs1 ;
                } /* end if (vecobj) */
	    } /* end if (size OK) */
	} /* end if (magic) */
	return (rs >= 0) ? sc : rs ;
}
/* end subroutine (strstore_indmk) */


/* private subroutines */

static int strstore_chunknew(strstore *op,int amount) noex {
	cint		csize = szof(strstore_ch) ;
	int		rs ;
	if (op->chsize > amount) amount = op->chsize ;
	if (void *vp ; (rs = uc_malloc(csize,&vp)) >= 0) ylikely {
	    strstore_ch		*cep = (strstore_ch *) vp ;
	    if ((rs = chunk_start(cep,(amount + 1))) >= 0) ylikely {
	        if ((rs = vechand_add(op->clp,cep)) >= 0) ylikely {
	    	    op->csp = cep ;
	            if (op->totalsize == 0) {
	                chunk_adv(cep) ;
	                op->totalsize = 1 ;
		    }
		}
		if (rs < 0) {
		    chunk_finish(cep) ;
		}
	    }
	    if (rs < 0) {
	        uc_free(cep) ;
	    }
	} /* end if (memory-allocations) */
	return rs ;
}
/* end subroutine (strstore_chunknew) */

static int strstore_chunkfins(strstore *op) noex {
	vechand		*clp = op->clp ;
	int		rs = SR_OK ;
	int		rs1 ;
	void		*vp{} ;
	for (int i = 0 ; vechand_get(clp,i,&vp) >= 0 ; i += 1) {
	    if (vp) {
	        strstore_ch	*cckp = (strstore_ch *) vp ;
		{
	            rs1 = chunk_finish(cckp) ;
	            if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = uc_free(cckp) ;
	            if (rs >= 0) rs = rs1 ;
		}
	    }
	} /* end for */
	return rs ;
}
/* end subroutine (strstore_chunkfins) */

static int strstore_manage(strstore *op,cchar *kp,int kl,int si) noex {
	int		rs ;
	if (int *ip{} ; (rs = lookaside_get(op->lap,&ip)) >= 0) ylikely {
	    hdb_dat	key ;
	    hdb_dat	val ;
	    *ip = si ;
	    key.buf = kp ;
	    key.len = kl ;
	    val.buf = ip ;
	    val.len = szof(int *) ;
	    rs = hdb_store(op->hlp,key,val) ;
	    if (rs < 0) {
	        lookaside_release(op->lap,ip) ;
	    }
	} /* end if */
	return rs ;
}
/* end subroutine (strstore_manage) */

static int chunk_start(strstore_ch *cnp,int csz) noex {
	int		rs = SR_INVALID ;
	memclear(cnp) ;
	if (csz > 0) {
	    cnp->csz = csz ;
	    rs = uc_malloc(csz,&cnp->cdata) ;
	}
	return rs ;
}
/* end subroutine (chunk_start) */

static int chunk_adv(strstore_ch *cnp) noex {
	cnp->cdata[0] = '\0' ;
	cnp->i += 1 ;
	return SR_OK ;
}
/* end subroutine (chunk_adv) */

static int chunk_finish(strstore_ch *cnp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (cnp->cdata) {
	    rs1 = uc_free(cnp->cdata) ;
	    if (rs >= 0) rs = rs1 ;
	    cnp->cdata = nullptr ;
	}
	cnp->csz = 0 ;
	cnp->i = 0 ;
	cnp->c = 0 ;
	return rs ;
}
/* end subroutine (chunk_finish) */


