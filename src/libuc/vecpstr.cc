/* vecpstr */
/* lang=C++20 */

/* vector-packed-string object */
/* version %I% last-modified %G% */

#define	CF_QSORT	0		/* use |qsort(3c)| */

/* revision history:

	= 1998-03-24, David A�D� Morano
        This object module was morphed from some previous one. I do not remember
        what the previous one was.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	vecpstr

	Description:
	This object module creates and manages a string table object.
	This string table can later be written to a file or otherwise
	stored some way so that it can be used in-place later.  This
	is often useful for cache files or ELF code object files.

	Arguments:
	stp		pointer to the vecpstr object
	<others>

	Returns:
	>=0		the total length of the filled up vecpstr so far!
	<0		error


	Notes:
	The index table (optionally) generated by this OBJECT using
	-> hash-linking <- to track down matches.  It does not use
	-> key-linking <-.  So searching the generated index table
	must also use hash-linking.


	Name:
	vecpstr_adduniq

	Description:
	This subroutine adds a string to the object, but first
	checks to see if it is already present.  Specified strings
	that already match an existing entry are not added.

	Synopsis:
	int vecpstr_adduniq(vecpstr *op,cchar *sp,int sl) noex

	Arguments:
	op		vecpstr string to add to
	sp		pointer to key string to add
	sl		length of string to add

	Returns:
	INT_MAX		string was already in container
	>=0		OK
	<0		error

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>
#include	<usystem.h>
#include	<usupport.h>
#include	<vechand.h>
#include	<vstrcmpx.h>
#include	<intceil.h>
#include	<nleadstr.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"vecpstr.h"


/* local defines */

#define	VECPSTR_CHSIZEIZE	512	/* starting table size */
#define	VECPSTR_NSKIP		5

#define	MODP2(v,n)	((v) & ((n) - 1))


/* namespaces */

using std::sort ;			/* actor */
using std::min ;			/* actor */
using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef vecpstr_ch	chunk ;

extern "C" {
    typedef int (*qsort_f)(cvoid *,cvoid *) noex ;
}


/* external subroutines */

extern "C" {
    extern int	nextpowtwo(int) noex ;
}


/* local structures */

struct strentry {
	uint	khash ;
	uint	hi ;
	uint	si ;
} ;


/* forward references */

static int	vecpstr_ctor(vecpstr *) noex ;
static int	vecpstr_dtor(vecpstr *) noex ;
static int	vecpstr_setopts(vecpstr *,int) noex ;
static int	vecpstr_finchunks(vecpstr *) noex ;
static int	vecpstr_extstr(vecpstr *,int) noex ;
static int	vecpstr_newchunk(vecpstr *,int) noex ;
static int	vecpstr_extvec(vecpstr *,int) noex ;
static int	vecpstr_record(vecpstr *,cchar *) noex ;
static int	vecpstr_reset(vecpstr *) noex ;
static int	vecpstr_validx(vecpstr *,int) noex ;
static void	vecpstr_arrsort(vecpstr *,vecpstr_vcmp) noex ;

template<typename ... Args>
static inline int vecpstr_magic(vecpstr *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == VECPSTR_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}

static int chunk_start(vecpstr_ch *,int) noex ;
static int chunk_finish(vecpstr_ch *) noex ;
static int chunk_check(vecpstr_ch *,int) noex ;
static int chunk_add(vecpstr_ch *,cchar *,int,cchar **) noex ;
static int chunk_addkeyval(vecpstr_ch *,cchar *,int,cchar *,int,cchar **) noex ;

static int	indexlen(int) noex ;
static int	indexsize(int) noex ;


/* local subroutines */

consteval int mkoptmask() noex {
	int	r = 0 ;
	r = r | VECPSTR_ODEFAULT ;
	r = r | VECPSTR_OREUSE ;
	r = r | VECPSTR_OCOMPACT ;
	r = r | VECPSTR_OSWAP ;
	r = r | VECPSTR_OSTATIONARY ;
	r = r | VECPSTR_OCONSERVE ;
	r = r | VECPSTR_OSORTED ;
	r = r | VECPSTR_OORDERED ;
	return r ;
}
/* end subroutine (mkoptmask) */


/* local variables */

constexpr bool		f_qsort = CF_QSORT ;

constexpr int		optmask = mkoptmask() ;

constexpr int		resz = sizeof(int) ;


/* exported subroutines */

int vecpstr_start(vecpstr *op,int n,int chsize,int opts) noex {
	int		rs ;
	if ((rs = vecpstr_ctor(op)) >= 0) {
	    if (chsize < VECPSTR_CHSIZEIZE) chsize = VECPSTR_CHSIZEIZE ;
	    if (n < 0) n = 0 ;
	    op->chsize = chsize ;
	    op->an = n ;
	    if ((rs = vecpstr_setopts(op,opts)) >= 0) {
	        cint	nc = min(((n * 6) / chsize),1) ;
	        if ((rs = vechand_start(op->clp,nc,0)) >= 0) {
	            op->magic = VECPSTR_MAGIC ;
	        }
	    }
	    if (rs < 0) {
		vecpstr_dtor(op) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecpstr_start) */

int vecpstr_finish(vecpstr *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = vecpstr_magic(op)) >= 0) {
	    if (op->va) {
	        rs1 = uc_libfree(op->va) ;
	        if (rs >= 0) rs = rs1 ;
	        op->va = nullptr ;
	    }
	    {
	        rs1 = vecpstr_finchunks(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = vechand_finish(op->clp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = vecpstr_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecpstr_finish) */

int vecpstr_audit(vecpstr *op) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = vecpstr_magic(op)) >= 0) {
		for (int i = 0 ; i < op->i ; i += 1) {
	    	    if (op->va[i] != nullptr) {
			cchar	*cp = op->va[i] ;
			c += 1 ;
			{
			    const uintptr_t vv = uintptr_t(cp) ;
			    {
				cint	v = int(vv & INT_MAX) ;
	        	        rs |= v ;	/* access might SEGFAULT */
			    }
			}
	    	    } /* end if (non-empty entry) */
		} /* end for */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecpstr_audit) */

int vecpstr_add(vecpstr *op,cchar *sp,int sl) noex {
	return vecpstr_store(op,sp,sl,nullptr) ;
}
/* end subroutine (vecpstr_add) */

int vecpstr_adduniq(vecpstr *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (op && sp) {
	    if (sl < 0) sl = strlen(sp) ;
	    if ((rs = vecpstr_findn(op,sp,sl)) >= 0) {
	        rs = INT_MAX ;
	    } else if (rs == SR_NOTFOUND) {
	        rs = vecpstr_add(op,sp,sl) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecpstr_adduniq) */

int vecpstr_addkeyval(vecpstr *op,cchar *kp,int kl,cchar *vp,int vl) noex {
	int		rs ;
	int		i = 0 ;
	if ((rs = vecpstr_magic(op,kp)) >= 0) {
		int	amount = 0 ;
	        if (kl < 0) kl = strlen(kp) ;
	        amount += (kl+1) ;
	        if (vp) {
	            if (vl < 0) vl = strlen(vp) ;
	            amount += (vl+1) ;
	        }
	        if ((rs = vecpstr_extstr(op,amount)) >= 0) {
	            cchar	*cp{} ;
	            if ((rs = chunk_addkeyval(op->ccp,kp,kl,vp,vl,&cp)) >= 0) {
	                if ((rs = vecpstr_record(op,cp)) >= 0) {
	                    i = rs ;
	        	    op->stsize += amount ;
			}
	            }
	        }
	} /* end if (magic) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecpstr_addkeyval) */

int vecpstr_store(vecpstr *op,cchar *sp,int sl,cchar **rpp) noex {
	int		rs ;
	int		i = 0 ;
	if ((rs = vecpstr_magic(op,sp)) >= 0) {
	    if (sl < 0) sl = strlen(sp) ;
	    {
	        cint	amount = (sl + 1) ;
	        cchar	*cp = nullptr ;
	        if ((rs = vecpstr_extstr(op,amount)) >= 0) {
	            if ((rs = chunk_add(op->ccp,sp,sl,&cp)) >= 0) {
	                if ((rs = vecpstr_record(op,cp)) >= 0) {
	                     i = rs ;
	                     op->stsize += amount ;
			}
	            }
	        }
	        if (rpp) {
	            *rpp = (rs >= 0) ? cp : nullptr ;
	        }
	    } /* end block */
	} /* end if (magic) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecpstr_store) */

int vecpstr_already(vecpstr *op,cchar *sp,int sl) noex {
	return vecpstr_findn(op,sp,sl) ;
}
/* end subroutine (vecpstr_already) */

int vecpstr_get(vecpstr *op,int i,cchar **rpp) noex {
	int		rs ;
	if ((rs = vecpstr_validx(op,i)) >= 0) {
	        if (rpp) {
	            *rpp = (rs >= 0) ? op->va[i] : nullptr ;
	        }
	} /* end if (validx) */
	return rs ;
}
/* end subroutine (vecpstr_get) */

int vecpstr_getlast(vecpstr *op,cchar **spp) noex {
	int		rs ;
	int		i = 0 ;
	if ((rs = vecpstr_magic(op)) >= 0) {
	        rs = SR_NOTFOUND ;
	        if (op->va && (op->c > 0)) {
	            i = (op->i-1) ;
	            while ((i >= 0) && (op->va[i] == nullptr)) {
		        i -= 1 ;
	            }
	            if (i >= 0) rs = SR_OK ;
	        } /* end if (populated) */
	        if (spp) {
	             *spp = (rs >= 0) ? op->va[i] : nullptr ;
	        }
	} /* end if (magic) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecpstr_getlast) */

int vecpstr_del(vecpstr *op,int i) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = vecpstr_validx(op,i)) >= 0) {
		bool	f_fi = false ;
	            if (op->va[i] != nullptr) {
	                op->c -= 1 ;		/* decrement list count */
	                if (op->f.stsize) {
	                    op->stsize -= (strlen(op->va[i]) + 1) ;
	                }
	            } /* end if (freeing the actual string data) */
	            if (op->f.ostationary) {
	                op->va[i] = nullptr ;
	                if (i == (op->i - 1)) op->i -= 1 ;
	                f_fi = true ;
	            } else if (op->f.issorted || op->f.oordered) {
	                if (op->f.ocompact) {
	                    op->i -= 1 ;
	                    for (int j = i ; j < op->i ; j += 1) {
	                        op->va[j] = op->va[j + 1] ;
		            }
	                    op->va[op->i] = nullptr ;
	                } else {
	                    op->va[i] = nullptr ;
	                    if (i == (op->i - 1)) op->i -= 1 ;
	                    f_fi = true ;
	                } /* end if */
	            } else {
	                bool	f = (op->f.oswap || op->f.ocompact) ;
	                if (f && (i < (op->i - 1))) {
	                    op->va[i] = op->va[op->i - 1] ;
	                    op->va[--op->i] = nullptr ;
	                    op->f.issorted = false ;
	                } else {
	                    op->va[i] = nullptr ;
	                    if (i == (op->i - 1)) op->i -= 1 ;
	                    f_fi = true ;
	                } /* end if */
	            } /* end if */
	            if (op->f.oconserve) {
	                while (op->i > i) {
	                    if (op->va[op->i - 1] != nullptr) break ;
	                    op->i -= 1 ;
	                } /* end while */
	            } /* end if */
	            if (f_fi && (i < op->fi)) {
	                op->fi = i ;
	            }
		    c = op->c ;
	    op->f.stsize = false ;
	} /* end if (validx) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecpstr_del) */

int vecpstr_delall(vecpstr *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = vecpstr_magic(op)) >= 0) {
	    {
	        rs1 = vecpstr_finchunks(op) ;
	        if (rs >= 0) rs = rs1 ;
	        op->ccp = nullptr ;
	    }
	    {
	        rs1 = vechand_delall(op->clp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->va) {
	        rs1 = uc_libfree(op->va) ;
	        if (rs >= 0) rs = rs1 ;
	        op->va = nullptr ;
	    }
	    if (rs >= 0) {
	        rs = vecpstr_reset(op) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecpstr_delall) */

int vecpstr_count(vecpstr *op) noex {
	int		rs ;
	if ((rs = vecpstr_magic(op)) >= 0) {
		rs = op->c ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecpstr_count) */

int vecpstr_sort(vecpstr *op,vecpstr_vcmp vcf) noex {
	int		rs ;
	int		i = 0 ;
	if ((rs = vecpstr_magic(op)) >= 0) {
	        if (op->va) {
		    rs = SR_OK ;
		    if (vcf == nullptr) vcf = vstrcmp ;
	            if ((! op->f.issorted) && (op->i > 1)) {
		        vecpstr_arrsort(op,vcf) ;
	            } /* end if (sorting) */
	            op->f.issorted = true ;
		} /* end if (populated) */
	} /* end if (magic) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecpstr_sort) */

int vecpstr_search(vecpstr *op,cchar *sp,vecpstr_vcmp vcf,cchar **rpp) noex {
	int		rs ;
	int		i = 0 ;
	if ((rs = vecpstr_magic(op,sp)) >= 0) {
		rs = SR_NOTFOUND ;
	        if (op->va) {
		    cchar	**rpp2 ;
		    if (vcf == nullptr) vcf = vstrcmp ;
	            if (op->f.issorted) {
	                cint	esize = sizeof(char *) ;
	                qsort_f	scf = qsort_f(vcf) ;
	                rpp2 = (cchar **) bsearch(&sp,op->va,op->i,esize,scf) ;
	                rs = SR_NOTFOUND ;
	                if (rpp2) {
	                    i = (rpp2 - op->va) ;
	                    rs = SR_OK ;
	                }
	            } else {
	                for (i = 0 ; i < op->i ; i += 1) {
	                    rpp2 = (op->va + i) ;
	                    if (*rpp2) {
	                        if ((*vcf)(&sp,rpp2) == 0) break ;
		            }
	                } /* end for */
	                rs = (i < op->i) ? SR_OK : SR_NOTFOUND ;
	            } /* end if (sorted or not) */
		} /* end if (was populated) */
	        if (rpp) {
	            *rpp = (rs >= 0) ? op->va[i] : nullptr ;
	        }
	} /* end if (magic) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecpstr_search) */

int vecpstr_finder(vecpstr *op,cchar *sp,vecpstr_vcmp vcf,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	if ((rs = vecpstr_magic(op,sp)) >= 0) {
		rs = SR_NOTFOUND ;
	        if (op->va) {
		   int		i = 0 ; /* <- used afterwards */
	           if (vcf == nullptr) vcf = vstrcmp ;
	           for (i = 0 ; i < op->i ; i += 1) {
	                if (op->va[i]) {
	                   if ((*vcf)(&sp,(op->va+i)) == 0) break ;
	                }
	           } /* end for */
	           if (i < op->i) rs = i ;
	           if (rpp) {
	               *rpp = (rs >= 0) ? op->va[i] : nullptr ;
	           }
		} /* end if (populated) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecpstr_finder) */

int vecpstr_find(vecpstr *op,cchar *sp) noex {
        int             rs ;
	if ((rs = vecpstr_magic(op,sp)) >= 0) {
                rs = SR_NOTFOUND ;
                if (op->va) {
                    cint        sch = sp[0] ; /* ok: all promoted similarly */
                    int         i ;
                    for (i = 0 ; i < op->i ; i += 1) {
                        cchar   *ep = op->va[i] ;
                        if (ep) {
                            if ((sch == ep[0]) && (strcmp(sp,ep) == 0)) break ;
                        }
                    } /* end for */
                    if (i < op->i) rs = i ;
                } /* end if (populated) */
	} /* end if (magic) */
        return rs ;
}
/* end subroutine (vecpstr_find) */

int vecpstr_findn(vecpstr *op,cchar *sp,int sl) noex {
	int		rs ;
	if ((rs = vecpstr_magic(op,sp)) >= 0) {
	        int	sch = sp[0] ; /* ok: since all get promoted similarly */
	        int	i = 0 ; /* <- used afterwards */
	        rs = SR_NOTFOUND ;
	    if (sl < 0) sl = strlen(sp) ;
	        for (i = 0 ; i < op->i ; i += 1) {
	            cchar	*ep = op->va[i] ;
	            if (ep && (sch == ep[0])) {
		        cint	m = nleadstr(ep,sp,sl) ;
		        if ((m == sl) && (ep[m] == '\0')) break ;
	            }
	        } /* end for */
	        if (i < op->i) rs = i ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecpstr_findn) */

int vecpstr_findaddr(vecpstr *op,cchar *addr) noex {
	int		rs ;
	if ((rs = vecpstr_magic(op,addr)) >= 0) {
		rs = SR_NOTFOUND ;
	        if (op->va) {
	            for (int i = 0 ; i < op->i ; i += 1) {
	                cchar	*ep = op->va[i] ;
	                if (ep && (addr == ep)) {
	                    rs = i ;
	                    break ;
	                }
	            } /* end for */
		} /* end if (populated) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecpstr_findaddr) */

int vecpstr_getsize(vecpstr *op) noex {
	int		rs ;
	int		size = 0 ;
	if ((rs = vecpstr_magic(op)) >= 0) {
	        if (op->stsize > 0) size = op->stsize ;
	} /* end if (magic) */
	return (rs >= 0) ? size : rs ;
}
/* end subroutine (vecpstr_getsize) */

int vecpstr_strsize(vecpstr *op) noex {
	int		rs ;
	int		stsize = 1 ;
	if ((rs = vecpstr_magic(op)) >= 0) {
	        if (! op->f.stsize) {
	            for (int i = 0 ; i < op->i ; i += 1) {
			cchar	*ep = op->va[i] ;
	                if (ep) {
	                    stsize += (strlen(ep) + 1) ;
		        }
	            } /* end for */
	            op->stsize = stsize ;
	            op->f.stsize = true ;
	        } /* end if (calculating size) */
	        stsize = uceil(op->stsize,resz) ;
	} /* end if (magic) */
	return (rs >= 0) ? stsize : rs ;
}
/* end subroutine (vecpstr_strsize) */

int vecpstr_strmk(vecpstr *op,char *tab,int tabsize) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = vecpstr_magic(op,tab)) >= 0) {
		    typedef int (*get_f)(vechand *,int,void **) ;
		    cint	stsize = uceil(op->stsize,sizeof(int)) ;
		    rs = SR_OVERFLOW ;
	            if (tabsize >= stsize) {
			vechand		*clp = op->clp ;
	                char		*bp = tab ;
			void		*vp{} ;
			get_f 		get = vechand_get ;
			rs = SR_OK ;
			*bp++ = '\0' ;	/* <- zeroth entry */
			c += 1 ;
	                for (int i = 0 ; get(clp,i,&vp) >= 0 ; i += 1) {
	                    vecpstr_ch	*ccp = (vecpstr_ch *) vp ;
	                    if (ccp && ccp->tab) {
	                        c += ccp->count ;
	                        memcpy(bp,ccp->tab,ccp->tablen) ;
	                        bp += ccp->tablen ;
		            }
	                } /* end for */
	                while (bp < (tab + tabsize)) {
	                    *bp++ = '\0' ;
	                }
	            } /* end if (not overflow) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecpstr_strmk) */

int vecpstr_recsize(vecpstr *op) noex {
	int		rs ;
	int		rsize = (2 * resz) ;
	if ((rs = vecpstr_magic(op)) >= 0) {
	    if (op->va) {
		rsize += (op->c * resz) ;
	    } /* end if (populated) */
	} /* end if (magic) */
	return (rs >= 0) ? rsize : rs ;
}
/* end subroutine (vecpstr_recsize) */

int vecpstr_recmk(vecpstr *op,int *rec,int recsize) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = vecpstr_magic(op,rec)) >= 0) {
	        cint	rsize = ((op->c + 2) * resz) ;
		rs = SR_OVERFLOW ;
	        if (recsize >= rsize) {
	            int		si = 0 ;
		    rs = SR_OK ;
	            rec[c++] = si ;
	            si += 1 ;
	            for (int i = 0 ; i < op->i ; i += 1) {
			cchar	*ep = op->va[i] ;
	                if (ep) {
	                    rec[c++] = si ;
	                    si += (strlen(ep) + 1) ;
		        }
	            } /* end for */
	            rec[c] = -1 ;
	        } /* end if (not overflow) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecpstr_recmk) */

int vecpstr_recmkstr(vecpstr *op,int *rec,int recs,char *tab,int tabs) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = vecpstr_magic(op,rec,tab)) >= 0) {
            if (op->stsize == 0) {
                rs = vecpstr_strsize(op) ;
            }
	    if (rs >= 0) {
		cint	stsize = iceil(op->stsize,resz) ;
		rs = SR_OVERFLOW ;
	        if (tabs >= stsize) {
	            cint	rsize = ((op->i + 2) * resz) ;
	            if (recs >= rsize) {
		        char	*bp = tab ;		/* table pointer */
	                rec[c++] = 0 ;
	                *bp++ = '\0' ;
	                for (int i = 0 ; i < op->i ; i += 1) {
			    cchar	*ep = op->va[i] ;
	                    if (ep) {
	                        rec[c++] = (bp - tab) ;
	                        bp = (strwcpy(bp,ep,-1) + 1) ;
		            }
	                } /* end for */
	                rec[c] = -1 ;
	            } /* end if (record-size OK) */
	        } /* end if (string-size OK) */
	    } /* end if (ok) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecpstr_recmkstr) */

int vecpstr_indlen(vecpstr *op) noex {
	int		rs ;
	int		il = 1 ;
	if ((rs = vecpstr_magic(op)) >= 0) {
	        il = indexlen(op->i + 1) ;
	} /* end if (magic) */
	return (rs >= 0) ? il : rs ;
}
/* end subroutine (vecpstr_indlen) */

int vecpstr_indsize(vecpstr *op) noex {
	int		rs ;
	int		isize = 0 ;
	if ((rs = vecpstr_magic(op)) >= 0) {
	        cint	il = indexlen(op->i + 1) ;
	        isize = indexsize(il) ;
	} /* end if (magic) */
	return (rs >= 0) ? isize : rs ;
}
/* end subroutine (vecpstr_indsize) */

int vecpstr_getvec(vecpstr *op,cchar ***rppp) noex {
	int		rs ;
	int		i = 0 ;
	if ((rs = vecpstr_magic(op,rppp)) >= 0) {
	        if ((rs = vecpstr_extvec(op,1)) >= 0) {
	            *rppp = op->va ;
		    i = op->i ;
	        }
	} /* end if (magic) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecpstr_getvec) */


/* private subroutines */

static int vecpstr_ctor(vecpstr *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    nullptr_t	np{} ;
	    rs = SR_NOMEM ;
	    op->va = nullptr ;
	    op->c = 0 ;
	    op->i = 0 ;
	    op->n = 0 ;
	    op->fi = 0 ;
	    op->stsize = 0 ;
	    op->ccp = nullptr ;
	    op->magic = 0 ;
	    op->chsize = 0 ;
	    op->an = 0 ;
	    if ((op->clp = new(nothrow) vechand) != np) {
	        rs = SR_OK ;
	    } /* end if (new) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecpstr_ctor) */

static int vecpstr_dtor(vecpstr *op) noex {
	int		rs = SR_OK ;
	if (op->clp) {
	    delete op->clp ;
	    op->clp = nullptr ;
	}
	return rs ;
}
/* end subroutine (vecpstr_dtor) */

static int vecpstr_setopts(vecpstr *op,int vopts) noex {
	int		rs = SR_INVALID ;
	if ((vopts & optmask) == 0) {
	    rs = SR_OK ;
	    op->f = {} ;
	    if (vopts & VECPSTR_OREUSE) op->f.oreuse = 1 ;
	    if (vopts & VECPSTR_OSWAP) op->f.oswap = 1 ;
	    if (vopts & VECPSTR_OSTATIONARY) op->f.ostationary = 1 ;
	    if (vopts & VECPSTR_OCOMPACT) op->f.ocompact = 1 ;
	    if (vopts & VECPSTR_OSORTED) op->f.osorted = 1 ;
	    if (vopts & VECPSTR_OORDERED) op->f.oordered = 1 ;
	    if (vopts & VECPSTR_OCONSERVE) op->f.oconserve = 1 ;
	} /* end if (valid vopts) */
	return rs ;
}
/* end subroutine (vecpstr_setopts) */

static int vecpstr_finchunks(vecpstr *op) noex {
	vechand		*clp = op->clp ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	void		*vp ;
	for (int i = 0 ; vechand_get(clp,i,&vp) >= 0 ; i += 1) {
	    vecpstr_ch	*ccp = (vecpstr_ch *) vp ;
	    if (ccp) {
	        c += 1 ;
		{
	            rs1 = chunk_finish(ccp) ;
	            if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = uc_libfree(ccp) ;
	            if (rs >= 0) rs = rs1 ;
		}
	    }
	} /* end for */
	op->ccp = nullptr ;
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecpstr_finchunks) */

static int vecpstr_extstr(vecpstr *op,int amount) noex {
	int		rs = SR_OK ;
	if (op->ccp) {
	    if (chunk_check(op->ccp,amount) > 0) op->ccp = nullptr ;
	} /* end if (check if could add to existing chunk) */
	if ((rs >= 0) && (amount > 0) && (op->ccp == nullptr)) {
	    rs = vecpstr_newchunk(op,amount) ;
	}
	return rs ;
}
/* end subroutine (vecpstr_extstr) */

static int vecpstr_newchunk(vecpstr *op,int amount) noex {
	cint		size = sizeof(vecpstr_ch) ;
	int		rs ;
	void		*vp{} ;
	op->ccp = nullptr ;
	if ((rs = uc_libmalloc(size,&vp)) >= 0) {
	    op->ccp = (vecpstr_ch *) vp ;
	    if (amount < op->chsize) amount = op->chsize ;
	    if ((rs = chunk_start(op->ccp,amount)) >= 0) {
	        rs = vechand_add(op->clp,op->ccp) ;
		if (rs < 0)
		    chunk_finish(op->ccp) ;
	    } /* end if (chunk) */
	    if (rs < 0) {
	        uc_libfree(op->ccp) ;
	        op->ccp = nullptr ;
	    }
	} /* end if (memory-allocation) */
	return rs ;
}
/* end subroutine (vecpstr_newchunk) */

static int vecpstr_extvec(vecpstr *op,int n) noex {
	int		rs = SR_OK ;
	if ((op->i + 1) > op->n) {
	    int		nn ;
	    int		vasize ;
	    void	*np{} ;
	    if (op->va == nullptr) {
	        cint	dn = (n > 0) ? n : VECPSTR_DEFENTS ;
	        nn = op->an ;
	        if (nn < dn) nn = dn ;
	        vasize = (nn + 1) * sizeof(char **) ;
	        rs = uc_libmalloc(vasize,&np) ;
	    } else {
	        nn = (op->n + 1) * 2 ;
	        vasize = (nn + 1) * sizeof(char **) ;
	        rs = uc_librealloc(op->va,vasize,&np) ;
	        op->va = nullptr ;
	    } /* end if */
	    if (rs >= 0) {
	        op->va = (cchar **) np ;
	        op->va[op->i] = nullptr ;
	        op->n = nn ;
	    }
	} /* end if (needed) */
	return rs ;
}
/* end subroutine (vecpstr_extvec) */

static int vecpstr_record(vecpstr *op,cchar *sp) noex {
	int		rs = SR_OK ;
	int		i = 0 ;
	if ((op->i + 1) > op->n) {
	    rs = vecpstr_extvec(op,-1) ;
	}
	if (rs >= 0) {
	    i = op->i ;
	    op->va[(op->i)++] = sp ;
	    op->va[op->i] = nullptr ;
	    op->c += 1 ;
	}
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecpstr_record) */

static int vecpstr_reset(vecpstr *op) noex {
	op->c = 0 ;
	op->i = 0 ;
	op->n = 0 ;
	op->fi = 0 ;
	op->stsize = 0 ;
	return SR_OK ;
}
/* end subroutine (vecpstr_reset) */

static void vecpstr_arrsort(vecpstr *op,vecpstr_vcmp vcf) noex {
	qsort_f		scf = qsort_f(vcf) ;
	if constexpr (f_qsort) {
	    cint	esize = sizeof(char *) ;
	    qsort(op->va,op->i,esize,scf) ;
	} else {
	    cint	i = op->i ;
	    cchar	**va = op->va ;
	    sort(va,(va+i),scf) ;
	}
}
/* end subroutine (vecpstr_arrsort) */

static int vecpstr_validx(vecpstr *op,int i) noex {
	int		rs ;
	if ((rs = vecpstr_magic(op)) >= 0) {
	    return ((i >= 0) && (i < op->i)) ? SR_OK : SR_NOTFOUND ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecpstr_validx) */

static int chunk_start(vecpstr_ch *ccp,int chsize) noex {
	int		rs ;
	void		*vp{} ;
	chsize = iceil(chsize,8) ;
	memclear(ccp) ;
	if ((rs = uc_libmalloc(chsize,&vp)) >= 0) {
	    ccp->tab = (char *) vp ;
	    ccp->tabsize = chsize ;
	    ccp->tab[0] = '\0' ;
	    ccp->tablen = 0 ;
	} /* end if (memory-allocation) */
	return rs ;
}
/* end subroutine (chunk_start) */

static int chunk_finish(vecpstr_ch *ccp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (ccp->tab) {
	    rs1 = uc_libfree(ccp->tab) ;
	    if (rs >= 0) rs = rs1 ;
	    ccp->tab = nullptr ;
	}
	ccp->tabsize = 0 ;
	return rs ;
}
/* end subroutine (chunk_finish) */

static int chunk_check(vecpstr_ch *ccp,int amount) noex {
	return (amount > (ccp->tabsize - ccp->tablen)) ;
}
/* end subroutine (chunk_check) */

static int chunk_add(vecpstr_ch *ccp,cchar *sp,int sl,cchar **rpp) noex {
	cint		amount = (sl + 1) ;
	int		rs = SR_OK ;
	if (amount <= (ccp->tabsize - ccp->tablen)) {
	    char	*bp = (ccp->tab + ccp->tablen) ;
	    strwcpy(bp,sp,sl) ;
	    ccp->tablen += amount ;
	    ccp->count += 1 ;
	    *rpp = bp ;
	} else {
	    rs = SR_BUGCHECK ;
	}
	return rs ;
}
/* end subroutine (chunk_add) */

static int chunk_addkeyval(vecpstr_ch *ccp,cchar *kp,int kl,
		cchar *vp,int vl,cchar **rpp) noex {
	cint		amount = (kl+1+vl+1) ;
	int		rs = SR_OK ;
	if (amount <= (ccp->tabsize - ccp->tablen)) {
	    char	*bp = (ccp->tab + ccp->tablen) ;
	    bp = strwcpy(bp,kp,kl) ;
	    *bp++ = '=' ;
	    if (vp != nullptr) {
	        strwcpy(bp,vp,vl) ;
	    } else {
		*bp++ = '\0' ;
	    }
	    ccp->tablen += amount ;
	    ccp->count += 1 ;
	    *rpp = bp ;
	} else {
	    rs = SR_BUGCHECK ;
	}
	return rs ;
}
/* end subroutine (chunk_addkeyval) */

static int indexlen(int n) noex {
	return nextpowtwo(n) ;
}
/* end subroutine (indexlen) */

static int indexsize(int il) noex {
	cint		isize = ((il + 1) * 3 * sizeof(int)) ;
	return isize ;
}
/* end subroutine (indexsize) */


