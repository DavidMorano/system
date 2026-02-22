/* vecstr_prime SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* vector string operations */
/* version %I% last-modified %G% */

#define	CF_QSORT	0		/* use |qsort(3c)| */

/* revision history:

	= 1998-02-13, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

	= 2023-10-07, David A­D­ Morano
	I updated this to match the updates applied to this (named)
	object back in 2018 but which were coded in another project
	(that used this same object name).  But no code from that
	other project (no names will be given here) were able to
	ever see the light of day (other than living on within my
	own memory).

*/

/* Copyright © 1998,2023 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	vecstr

	Description:
	This object is used when the caller wants to store a COPY
	of the passed string data into a vector-like container (how
	this object is implemented).  A copy of the caller-supplied
	string is stored in the (vector) list.  The advantage is
	that the caller does not have to keep the orginal data
	around in order for the list data to be accessed later.

	Notes:
	The method |vecstr_find| is equivalent to:
		int vecstr_finder(lp,string,nullptr,nullptr) noex
	but is shorter (and hopefully faster).


	Name:
	vecstr_adduniq

	Description:
	This subroutine adds a string to the object, but first
	checks to see if it is already present.  Specified strings
	that already match an existing entry are not added.

	Synopsis:
	int vecstr_adduniq(vecstr *op,cchar *sp,int sl) noex

	Arguments:
	op		vecstr string to add to
	sp		pointer to key string to add
	sl		length of string to add

	Returns:
	INT_MAX		string was already in container
	>=0		OK
	<0		error code (system-return)

	Implementation reminders:
	1. The central vector table is zeroth based. That is, read
	c-strings -- the pointers to those c-strings -- occupt the
	zeroth slot and then continuing upwards.
	2. When making a "string-table" and the assciated record-table,
	the zero-th entry in both the string table and the record
	table are special in that they are not (really) used.  The
	zero-th index into the string-table references a NUL
	(zero-terminated) c-string (which is just a single zero
	(NUL) character.  And the zero-th element of the record
	table contains a zero value (which could be interpreted as
	referencing the corresponding zero-th entry in the string-table.
	But that entry is made-up, in that it was not actually
	encountered in the data (the data accummulated).
	3. Why the zero entries in both the string-table and the
	record-table? Because when making index tables, we want
	the number zero in any slot of the index table to mean that
	the table entry is "empty" and not being used to reference
	any strings anywhere (either in the string-table or in the
	record-table).
	4. Enjoy.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |sort(3c++)| */
#include	<string>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<nulstr.h>
#include	<nleadstr.h>
#include	<strwcpy.h>
#include	<vstrcmp.h>
#include	<intceil.h>
#include	<localmisc.h>

#include	"vecstr.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| + |getlenstr(3u)| */

/* local defines */

#ifndef	CF_QSORT
#define	CF_QSORT	0		/* use |qsort(3c)| */
#endif


/* imported namespaces */

using std::string ;			/* type */
using libuc::libmem ;			/* variable */


/* local typedefs */

extern "C" {
    typedef int (*vs_f)(cvoid *,cvoid *) noex ;
}


/* external subroutines */

extern "C++" {
    int		vecstr_add(vecstr *,string *) noex ;
}


/* external variables */


/* local structures */


/* forward references */

local int	vecstr_ctor(vecstr *) noex ;
local int	vecstr_dtor(vecstr *) noex ;
local int	vecstr_setopts(vecstr *,int) noex ;
local int	vecstr_extvec(vecstr *,int = 0) noex ;
local int	vecstr_addsp(vecstr *,cchar *) noex ;
local int	vecstr_insertsp(vecstr *,int,cchar *) noex ;
local int	vecstr_validx(vecstr *,int) noex ;
local void	vecstr_arrsort(vecstr *,vecstr_vcmp) noex ;


/* local variables */

constexpr int		defents = VECSTR_DEFENTS ;
constexpr int		rsn = SR_NOTFOUND ;
constexpr int		resz = szof(int) ;


/* exported variables */

constexpr vecstrms	vecstrm ;


/* exported subroutines */

int vecstr_start(vecstr *op,int vn,int vo) noex {
	int		rs ;
	if (vn <= 1) vn = defents ;
	if ((rs = vecstr_ctor(op)) >= 0) ylikely {
	    op->n = vn ;
	    if ((rs = vecstr_setopts(op,vo)) >= 0) ylikely {
	        cint	sz = (vn + 1) * szof(cchar **) ;
	        if (void *va ; (rs = libmem.mall(sz,&va)) >= 0) ylikely {
	            op->va = ccharpp(va) ;
	            op->va[0] = nullptr ;
	            op->stsz = 1 ;
		    op->fl.stsz = true ;	/* starts off true */
	        } /* end if (memory-allocation) */
	    } /* end if */
	    if (rs < 0) {
		vecstr_dtor(op) ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? vn : rs ;
}
/* end subroutine (vecstr_start) */

int vecstr_finish(vecstr *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->va) ylikely {
	        for (int i = 0 ; i < op->i ; i += 1) {
		    cchar	*ep = op->va[i] ;
	            if (ep) {
			char *bp = cast_const<charp>(ep) ;
	                rs1 = libmem.free(bp) ;
		        if (rs >= 0) rs = rs1 ;
	            }
	        } /* end for */
		{
	            rs1 = libmem.free(op->va) ;
	            if (rs >= 0) rs = rs1 ;
	            op->va = nullptr ;
		}
	    } /* end if (populated) */
	    op->c = 0 ;
	    op->i = 0 ;
	    op->n = 0 ;
	    {
		rs1 = vecstr_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecstr_finish) */

int vecstr_audit(vecstr *op) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->va) ylikely {
	        for (int i = 0 ; i < op->i ; i += 1) {
		    cchar	*ep = op->va[i] ;
	            if (ep) {
	                cchar	*cp = (cchar *) ep ;
	                rs |= *cp ;/* access might SEGFAULT (as wanted) */
		        c += 1 ;
	            }
	        } /* end for */
	        rs = (c == op->c) ? SR_OK : SR_BADFMT ;
	    } /* end if (populated) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstr_audit) */

int vecstr_add(vecstr *op,cchar *sp,int sl) noex {
	return vecstr_store(op,sp,sl,nullptr) ;
}
/* end subroutine (vecstr_add) */

int vecstr_adduniq(vecstr *op,cchar *sp,int µsl) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) {
	        if ((rs = vecstr_findn(op,sp,sl)) >= 0) {
	            rs = INT_MAX ;
	        } else if (rs == SR_NOTFOUND) {
	            rs = vecstr_add(op,sp,sl) ;
	        }
	    } /* end if (getlenstr) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecstr_adduniq) */

int vecstr_addkeyval(vecstr *op,cchar *kp,int kl,cchar *vp,int vl) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (op && kp) ylikely {
	    rs = SR_OK ;
	    if (kl < 0) kl = lenstr(kp) ;
	    if ((vl < 0) || ((vp == nullptr) && (vl != 0))) {
	        vl = (vp) ? lenstr(vp) : 0 ;
	    }
	    if ((op->i + 1) > op->n) {
	        rs = vecstr_extvec(op) ;
	    }
	    if (rs >= 0) {
	        cint	sz = (kl + 1 + vl + 1) ;
	        if (char *ap ; (rs = libmem.mall(sz,&ap)) >= 0) {
		    char	*bp = ap ;
	            bp = strwcpy(bp,kp,kl) ;
	            *bp++ = '=' ;
	            if (vp) bp = strwcpy(bp,vp,vl) ;
	            *bp = '\0' ;
	            op->stsz += sz ;
	            i = vecstr_addsp(op,ap) ;
	        } /* end if (memory-allocation) */
	    } /* end if (ok) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecstr_addkeyval) */

int vecstr_insert(vecstr *op,int ii,cchar *sp,int µsl) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (op) ylikely {
	    if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) {
	        if ((rs = vecstr_validx(op,ii)) >= 0) ylikely {
	             if ((op->i + 1) > op->n) {
	                 rs = vecstr_extvec(op) ;
	             }
	             if (rs >= 0) {
	                 cint	sz = (sl + 1) ;
	                 if (char *bp ; (rs = libmem.mall(sz,&bp)) >= 0) {
	                     strwcpy(bp,sp,sl) ;
	                     op->stsz += sz ;
	                     i = vecstr_insertsp(op,ii,bp) ;
	                 } /* end if (memory-allocation) */
	             } /* end if (OK) */
	        } /* end if (valid index) */
	    } /* end if (getlenstr) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecstr_insert) */

int vecstr_store(vecstr *op,cchar *sp,int µsl,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (op) ylikely {
	    if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) {
		rs = SR_OK ;
	        if ((op->i + 1) > op->n) {
	            rs = vecstr_extvec(op) ;
	        }
	        if (rs >= 0) {
	            cint	sz = (sl + 1) ;
	            if (char *bp ; (rs = libmem.mall(sz,&bp)) >= 0) {
	                char *ep = strwcpy(bp,sp,sl) ;
	                op->stsz += intconv(ep - bp) ;
	                i = vecstr_addsp(op,bp) ;
		        if (rpp) *rpp = bp ;
	            } /* end if (m-a) */
	        } /* end if (ok) */
	    } /* end if (getlenstr) */
	    if (rpp && (rs < 0)) *rpp = nullptr ;
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecstr_store) */

int vecstr_already(vecstr *op,cchar *sp,int sl) noex {
	return vecstr_findn(op,sp,sl) ;
}
/* end subroutine (vecstr_already) */

int vecstr_get(vecstr *op,int i,cchar **spp) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_NOTFOUND ;
	    if (op->va) ylikely {
		rs = vecstr_validx(op,i) ;
	        if (spp) {
	            *spp = (rs >= 0) ? op->va[i] : nullptr ;
	        }
	    } /* end if (populated) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecstr_get) */

int vecstr_getlast(vecstr *op,cchar **spp) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (op) ylikely {
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
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecstr_getlast) */

int vecstr_del(vecstr *op,int i) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op) ylikely {
	    rs = SR_NOTFOUND ;
	    if (op->va) ylikely {
		if ((rs = vecstr_validx(op,i)) >= 0) ylikely {
		    bool	f_fi = false ;
	            if (op->va[i] != nullptr) {
	                op->c -= 1 ;		/* decrement list count */
	                if (op->fl.stsz) {
	                    op->stsz -= (lenstr(op->va[i]) + 1) ;
	                }
			char *bp = cast_const<charp>(op->va[i]) ;
	    		libmem.free(bp) ;
		    } /* end if (freeing the actual string data) */
		    if (op->fl.ostationary) {
	    	        op->va[i] = nullptr ;
	    	        if (i == (op->i - 1)) op->i -= 1 ;
	    	        f_fi = true ;
		    } else if (op->fl.issorted || op->fl.oordered) {
	                if (op->fl.ocompact) {
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
			bool	f = (op->fl.oswap || op->fl.ocompact) ;
			f = f && (i < (op->i - 1)) ;
			if (f) {
	                    op->va[i] = op->va[op->i - 1] ;
	                    op->va[--op->i] = nullptr ;
	                    op->fl.issorted = false ;
	                } else {
	                    op->va[i] = nullptr ;
	                    if (i == (op->i - 1)) op->i -= 1 ;
	                    f_fi = true ;
	                } /* end if */
	            } /* end if */
	            if (op->fl.oconserve) {
	                while (op->i > i) {
	                    if (op->va[op->i - 1] != nullptr) break ;
	                    op->i -= 1 ;
	                } /* end while */
	            } /* end if */
	            if (f_fi && (i < op->fi)) op->fi = i ;
		} else if (rs == SR_NOTFOUND) {
		    rs = vecstr_delall(op) ;
	        } /* end if (valid index) */
		op->fl.stsz = false ;
		op->stsz = 0 ;
		c = op->c ;
	    } /* end if (populated) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstr_del) */

int vecstr_delall(vecstr *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->va) ylikely {
	        for (int i = 0 ; i < op->i ; i += 1) {
	            if (op->va[i] != nullptr) {
			char *bp = cast_const<charp>(op->va[i]) ;
		        rs1 = libmem.free(bp) ;
		        if (rs >= 0) rs = rs1 ;
	            }
	        } /* end for */
	        op->i = 0 ;
	        op->fi = 0 ;
	        op->va[0] = nullptr ;
	        op->c = 0 ;
	        op->stsz = 1 ;
	    } /* end if (populated) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecstr_delall) */

int vecstr_count(vecstr *op) noex {
	int	rs = SR_FAULT ;
	if (op) ylikely {
	    rs = op->c ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecstr_count) */

int vecstr_sort(vecstr *op,vecstr_vcmp vcf) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->va) ylikely {
	        if (vcf == nullptr) vcf = vstrcmp ;
	        if ((! op->fl.issorted) && (op->c > 1)) {
		    c = op->c ;
		    vecstr_arrsort(op,vcf) ;
	        } /* end if (sorting) */
	        op->fl.issorted = true ;
	    } /* end if (populated) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstr_sort) */

int vecstr_search(vecstr *op,cchar *sp,vecstr_vcmp vcf,cchar **rpp) noex {
	int		rs  = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_NOTFOUND ;
	    if (op->va) ylikely {
		int	i = 0 ;
	        cchar	**spp{} ;
	        if (vcf == nullptr) vcf = vstrcmp ;
	        if (op->fl.osorted && (! op->fl.issorted)) {
	            op->fl.issorted = true ;
	            if (op->c > 1) {
		        vecstr_arrsort(op,vcf) ;
	            }
	        } /* end if (sorting) */
	        if (op->fl.issorted) {
		    csize	alen = size_t(op->i) ;
	            csize	esize = sizeof(char *) ;
	            qsort_f	scf = qsort_f(vcf) ;
	            spp = (cchar **) bsearch(&sp,op->va,alen,esize,scf) ;
	            rs = SR_NOTFOUND ;
	            if (spp) {
	                i = intconv(spp - op->va) ;
	                rs = i ;
	            }
	        } else {
	            for (i = 0 ; i < op->i ; i += 1) {
	                spp = (op->va + i) ;
	                if (*spp) {
	                    if ((*vcf)(&sp,spp) == 0) break ;
		        }
	            } /* end for */
		    if (i < op->i) rs = i ;
	        } /* end if (sorted or not) */
	        if (rpp) {
	            *rpp = (rs >= 0) ? op->va[i] : nullptr ;
	        }
	    } /* end if (populated) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecstr_search) */

int vecstr_searchl(vecstr *op,cchar *sp,int sl,vecstr_vcmp vcf,cc **rpp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		i = -1 ;
	if (op && sp) ylikely {
	    cchar	*s{} ;
	    if (nulstr ns ; (rs = ns.start(sp,sl,&s)) >= 0) ylikely {
		if ((rs = vecstr_search(op,s,vcf,rpp)) >= 0) {
		    i = rs ;
		} else if (rs == rsn) {
		    rs = SR_OK ;
		}
	        rs1 = ns.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (nulstr) */
	} /* end if (non-null) */
	if ((rs >= 0) && (i < 0)) {
	    rs = SR_NOTFOUND ; /* real errors priority */
	}
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecstr_searchl) */

int vecstr_finder(vecstr *op,cchar *sp,vecstr_vcmp vcf,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_NOTFOUND ;
	    if (vcf == nullptr) vcf = vstrcmp ;
	    if (op->va) ylikely {
		int	i ; /* used-afterwards */
	        for (i = 0 ; i < op->i ; i += 1) {
	            if (op->va[i]) {
	                if ((*vcf)(&sp,(op->va + i)) == 0) break ;
	            }
	        } /* end for */
	        if (i < op->i) rs = i ;
	        if (rpp) {
	            *rpp = (rs >= 0) ? op->va[i] : nullptr ;
	        }
	    } /* end if (populated) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecstr_finder) */

int vecstr_find(vecstr *op,cchar *sp) noex {
	int		rs = SR_FAULT ;
	if (op && sp) ylikely {
	    rs = SR_NOTFOUND ;
	    if (op->va) ylikely {
		cint	sch = sp[0] ; /* ok: since all get promoted similarly */
		int	i ;
	        for (i = 0 ; i < op->i ; i += 1) {
	            cchar	*ep = op->va[i] ;
	            if (ep) {
	                if ((sch == ep[0]) && (strcmp(sp,ep) == 0)) break ;
		    }
	        } /* end for */
	        if (i < op->i) rs = i ;
	    } /* end if (populated) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecstr_find) */

int vecstr_findn(vecstr *op,cchar *sp,int µsl) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) {
	        rs = SR_NOTFOUND ;
	        if (op->va) ylikely {
	            cint	sch = sp[0] ; /* ok: all promoted similarly */
	    	    int	i{} ;		/* <- used-afterwards */
	            for (i = 0 ; i < op->i ; i += 1) {
	                cchar	*ep = op->va[i] ;
	                if (ep && (sch == ep[0])) {
		            cint	m = nleadstr(ep,sp,sl) ;
		            if ((m == sl) && (ep[m] == '\0')) break ;
		        }
	            } /* end for */
	            if (i < op->i) rs = i ;
	        } /* end if (populated) */
	    } /* end if (getlenstr) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecstr_findn) */

int vecstr_findaddr(vecstr *op,cchar *addr) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_NOTFOUND ;
	    if (op->va) ylikely {
	        for (int i = 0 ; i < op->i ; i += 1) {
	            cchar	*ep = op->va[i] ;
	            if (ep && (addr == ep)) {
		        rs = i ;
		        break ;
	            }
	        } /* end for */
	    } /* end if (populated) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecstr_findaddr) */

int vecstr_getvec(vecstr *op,mainv *rppp) noex {
	int		rs = SR_FAULT ;
	if (op && rppp) ylikely {
	    if ((rs = vecstr_extvec(op)) >= 0) ylikely {
	        if (op->va) ylikely {
		    rs = op->c ;
		    *rppp = mainv(op->va) ;
		}
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecstr_getvec) */

int vecstr_strsize(vecstr *op) noex {
	int		rs = SR_FAULT ;
	int		stsz = 1 ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->va) ylikely {
	        if (! op->fl.stsz) {
	            for (int i = 0 ; i < op->i ; i += 1) {
			cchar	*ep = op->va[i] ;
	                if (ep) {
	                    stsz += (lenstr(ep) + 1) ;
		        }
	            } /* end for */
	            op->stsz = stsz ;
	            op->fl.stsz = true ;
	        } /* end if (calculating size) */
	        stsz = iceil(op->stsz,szof(int)) ;
	    } /* end if (populated) */
	} /* end if (non-null) */
	return (rs >= 0) ? stsz : rs ;
}
/* end subroutine (vecstr_strsize) */

int vecstr_strmk(vecstr *op,char *tab,int tabs) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op && tab) ylikely {
	    cint 	stsz = iceil(op->stsz,resz) ;
	    rs = SR_OVERFLOW ;
	    if (tabs >= stsz) {
	        char	*bp = tab ;
	        rs = SR_OK ;
	        *bp++ = '\0' ;	/* <- no chunks: do it ourselves */
		c += 1 ;	/* and count it */
	    	if (op->va) {
	            for (int i = 0 ; i < op->i ; i += 1) {
			cchar	*ep = op->va[i] ;
	                if (ep) {
	    	            c += 1 ;
	                    bp = strwcpy(bp,ep,-1) + 1 ;
		        }
	            } /* end for */
	        } /* end if (populated) */
		while (bp < (tab + tabs)) {
	            *bp++ = '\0' ;
	        }
	    } /* end if (supplied table large enough) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstr_strmk) */

int vecstr_recsize(vecstr *op) noex {
	int		rs = SR_FAULT ;
	int		rsz = (2 * resz) ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->va) ylikely {
	        rsz += (op->c * resz) ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? rsz : rs ;
}
/* end subroutine (vecstr_recsize) */

int vecstr_cksize(vecstr *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    if ((rs = op->stsz) == 0) {
	        rs = vecstr_strsize(op) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecstr_cksize) */

int vecstr_recmk(vecstr *op,int *rec,int recs) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op && rec) ylikely {
	    cint	rsz = ((op->c + 2) * resz) ;
	    rs = SR_OVERFLOW ;
	    if (recs >= rsz) ylikely {
	        int	si = 0 ;
		rs = SR_OK ;
	        rec[c++] = si ;		/* <- zeroth entry is zero */
	        si += 1 ;
		if (op->va) {
	            for (int i = 0 ; i < op->i ; i += 1) {
			cchar	*ep = op->va[i] ;
	                if (ep) {
	                    rec[c++] = si ;
	                    si += (lenstr(ep) + 1) ;
		        }
	            } /* end for */
		} /* end if (populated) */
	        rec[c] = -1 ;			/* <- last record entry */
	    } /* end if (user-supplied size was OK) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstr_recmk) */

int vecstr_recmkstr(vecstr *op,int *rec,int recs,char *tab,int tabs) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op && rec && tab) ylikely {
	    rs = SR_OK ;
	    if (op->stsz == 0) {
		rs = vecstr_strsize(op) ;
	    }
	    if (rs >= 0) ylikely {
		cint 	stsz = iceil(op->stsz,resz) ;
		rs = SR_OVERFLOW ;
	        if (tabs >= stsz) ylikely {
	            cint	rsz = ((op->c + 2) * resz) ;
	            if (recs >= rsz) {
	    		char	*bp = tab ;
	    		rec[c++] = 0 ;		/* <- zeroth entry is zero */
	    		*bp++ = '\0' ;
	                    for (int i = 0 ; i < op->i ; i += 1) {
				cchar	*ep = op->va[i] ;
	                        if (ep) {
	                            rec[c++] = intconv(bp - tab) ;
	                            bp = (strwcpy(bp,ep,-1) + 1) ;
		                }
	                    } /* end for */
	                rec[c] = -1 ;		/* <- last record entry */
	            } /* end if (record-size OK) */
		} /* end if (string-size OK) */
	    } /* end if (OK so far) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstr_recmkstr) */

int vecstr_avmkstr(vecstr *op,cchar **av,int avs,char *tab,int tabs) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op && av && tab) ylikely {
	    if ((rs = op->cksize) >= 0) ylikely {
		int	sz = iceil(op->stsz,szof(int)) ;
		rs = SR_OVERFLOW ;
	        if (tabs >= sz) ylikely {
	            sz = (op->c + 1) * szof(int) ;
	            if (avs >= sz) {
		        char	*bp = tab ;
		        rs = SR_OK ;
	                *bp++ = '\0' ;
	                for (int i = 0 ; i < op->i ; i += 1) {
		            cchar	*ep = op->va[i] ;
	                    if (ep) {
	                        av[c++] = bp ;
	                        bp = strwcpy(bp,ep,-1) + 1 ;
		            }
	                } /* end for */
	                av[c] = nullptr ;
	            }
	        }
	    } /* end if (vecstr_cksize) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstr_avmkstr) */

int vecstr_add(vecstr *op,string *strp) noex {
	int		rs = SR_FAULT ;
	if (op && strp) ylikely {
	    cchar	*sp = strp->c_str() ;
	    cint	sl = intconv(strp->size()) ;
	    rs = vecstr_add(op,sp,sl) ;
	}
	return rs ;
} /* end subroutine (vecstr_add) */


/* private subroutines */

local int vecstr_ctor(vecstr *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    op->va = nullptr ;
	    op->c = 0 ;
	    op->i = 0 ;
	    op->n = 0 ;
	    op->fi = 0 ;
	    op->stsz = 0 ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (vecstr_ctor) */

local int vecstr_dtor(vecstr *op) noex {
	int		rs = SR_OK ;
	if (op->va) {
	    op->va = nullptr ;
	}
	return rs ;
} /* end subroutine (vecstr_dtor) */

consteval int mkoptmask() noex {
	int		m = 0 ;
	m |= vecstrm.reuse ;
	m |= vecstrm.compact ;
	m |= vecstrm.swap ;
	m |= vecstrm.stationary ;
	m |= vecstrm.conserve ;
	m |= vecstrm.sorted ;
	m |= vecstrm.ordered ;
	return m ;
}
/* end subroutine (mkoptmask) */

local int vecstr_setopts(vecstr *op,int vo) noex {
	constexpr int	optmask = mkoptmask() ;
	int		rs = SR_INVALID ;
	if ((vo & (~ optmask)) == 0) ylikely {
	    rs = SR_OK ;
	    op->fl = {} ;
	    if (vo & vecstrm.reuse)		op->fl.oreuse		= true ;
	    if (vo & vecstrm.swap)		op->fl.oswap		= true ;
	    if (vo & vecstrm.stationary)	op->fl.ostationary	= true ;
	    if (vo & vecstrm.compact)		op->fl.ocompact		= true ;
	    if (vo & vecstrm.sorted)		op->fl.osorted		= true ;
	    if (vo & vecstrm.ordered)		op->fl.oordered		= true ;
	    if (vo & vecstrm.conserve)		op->fl.oconserve	= true ;
	} /* end if (valid options) */
	return rs ;
}
/* end subroutine (vecstr_setopts) */

local int vecstr_extvec(vecstr *op,int n) noex {
	int		rs = SR_OK ;
	if ((op->i + 1) > op->n) {
	    int		nn ;
	    int		sz ;
	    void	*na{} ;
	    if (op->va == nullptr) {
	        nn = (n > 0) ? n : defents ;
	        sz = (nn + 1) * szof(char **) ;
	        rs = libmem.mall(sz,&na) ;
	    } else {
	        nn = (op->n + 1) * 2 ;
	        sz = (nn + 1) * szof(char **) ;
	        rs = libmem.rall(op->va,sz,&na) ;
	    }
	    if (rs >= 0) {
	        op->va = (cchar **) na ;
		op->va[op->i] = nullptr ;
	        op->n = nn ;
	    }
	} /* end if (extension needed) */
	return rs ;
}
/* end subroutine (vecstr_extvec) */

local int vecstr_addsp(vecstr *op,cchar *sp) noex {
	int		i = 0 ;
	cbool		f = op->fl.oreuse || op->fl.oconserve ;
	bool		f_done = false ;
	if (f && (op->c < op->i)) {
	    i = op->fi ;
	    while ((i < op->i) && (op->va[i] != nullptr)) {
	        i += 1 ;
	    }
	    op->fi = i + 1 ;
	    if (i < op->i) {
	        op->va[i] = (char *) sp ;
	        f_done = true ;
	    }
	} /* end if (reuse or conserve) */
	if (! f_done) {
	    i = op->i ;
	    op->va[op->i++] = sp ;
	    op->va[op->i] = nullptr ;
	}
	op->c += 1 ;
	op->fl.issorted = false ;
	return i ;
}
/* end subroutine (vecstr_addsp) */

local int vecstr_insertsp(vecstr *op,int ii,cchar *sp) noex {
	if (ii == op->i) {
	    op->i += 1 ;
	    op->va[op->i] = nullptr ;
	} else if (op->va[ii] != nullptr) {
	    int		i ;
	    for (i = (ii + 1) ; i < op->i ; i += 1) {
		if (op->va[i] == nullptr) break ;
	    }
	    if (i == op->i) {
	        op->i += 1 ;
	        op->va[op->i] = nullptr ;
	    }
	    for (int j = i ; j > ii ; j -= 1) {
		op->va[j] = op->va[j-1] ;
	    }
	} /* end if */
	op->va[ii] = sp ;
	op->c += 1 ;
	op->fl.issorted = false ;
	return ii ;
}
/* end subroutine (vecstr_insertsp) */

local int vecstr_validx(vecstr *op,int i) noex {
	return ((i >= 0) && (i < op->i)) ? SR_OK : SR_NOTFOUND ;
}
/* end subroutine (vecstr_validx) */

local void vecstr_arrsort(vecstr *op,vecstr_vcmp vcf) noex {
	cchar	**va = op->va ;
	csize	alen = size_t(op->i) ;
	csize	esize = sizeof(char *) ;
	qsort_f	scf = qsort_f(vcf) ;
	qsort(va,alen,esize,scf) ;
}
/* end subroutine (vecstr_arrsort) */

int vecstr::add(cchar *sp,int sl) noex {
	return vecstr_add(this,sp,sl) ;
}

int vecstr::adduniq(cchar *sp,int sl) noex {
	return vecstr_adduniq(this,sp,sl) ;
}

int vecstr::addsyms(cchar *on,mainv sv) noex {
	return vecstr_addsyms(this,on,sv) ;
}

int vecstr::addpath(cchar *sp,int sl) noex {
	return vecstr_addpath(this,sp,sl) ;
}

int vecstr::insert(int ai,cchar *sp,int sl) noex {
	return vecstr_insert(this,ai,sp,sl) ;
}

int vecstr::store(cchar *sp,int sl,cchar **rpp) noex {
	return vecstr_store(this,sp,sl,rpp) ;
}

int vecstr::already(cchar *sp,int sl) noex {
	return vecstr_already(this,sp,sl) ;
}

int vecstr::loadfile(int fu,cchar *fname) noex {
	return vecstr_loadfile(this,fu,fname) ;
}

int vecstr::get(int ai,cchar **rpp) noex {
	return vecstr_get(this,ai,rpp) ;
}

int vecstr::getlast(cchar **spp) noex {
    	return vecstr_getlast(this,spp) ;
}

int vecstr::envadd(cchar *kp,cchar *valp,int vall) noex {
	return vecstr_envadd(this,kp,valp,vall) ;
}

int vecstr::envset(cchar *kp,cchar *valp,int vall) noex {
	return vecstr_envset(this,kp,valp,vall) ;
}

int vecstr::envfile(cchar *fn) noex {
	return vecstr_envfile(this,fn) ;
}

int vecstr::find(cchar *s) noex {
    	return vecstr_find(this,s) ;
}

int vecstr::findn(cchar *sp,int sl) noex {
    	return vecstr_findn(this,sp,sl) ;
}

int vecstr::search(cchar *s,vecstr_f vcmp,cchar **rpp) noex {
	return vecstr_search(this,s,vcmp,rpp) ;
}

int vecstr::finder(cchar *s,vecstr_f vcmp,cchar **rpp) noex {
	return vecstr_finder(this,s,vcmp,rpp) ;
}

int vecstr::getvec(mainv *rppp) noex {
	return vecstr_getvec(this,rppp) ;
}

int vecstr::del(int ai) noex {
	if (ai < 0) ai = 0 ;
	return vecstr_del(this,ai) ;
}

void vecstr::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("vecstr",rs,"fini-finish") ;
	}
} /* end method (vecstr::dtor) */

vecstr::operator int () noex {
    	int		rs = SR_NOTOPEN ;
	if (n > 0) {
	    rs = c ;
	}
	return rs ;
} /* end method (vecstr::operator) */

int vecstr_st::operator () (int vn,int vo) noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    rs = vecstr_start(op,vn,vo) ;
	} /* end if (non-null) */
	return rs ;
} /* end method (vecstr_st::operator) */

int vecstr_so::operator () (vecstr_f cmp) noex  {
	return vecstr_sort(op,cmp) ;
} /* end method (vecstr_so:operator) */

vecstr_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case vecstrmem_addcspath:
	        rs = vecstr_addcspath(op) ;
	        break ;
	    case vecstrmem_count:
	        rs = vecstr_count(op) ;
	        break ;
	    case vecstrmem_delall:
	        rs = vecstr_delall(op) ;
	        break ;
	    case vecstrmem_strsize:
	        rs = vecstr_strsize(op) ;
	        break ;
	    case vecstrmem_recsize:
	        rs = vecstr_recsize(op) ;
	        break ;
	    case vecstrmem_cksize:
	        rs = vecstr_cksize(op) ;
	        break ;
	    case vecstrmem_audit:
	        rs = vecstr_audit(op) ;
	        break ;
	    case vecstrmem_finish:
	        rs = vecstr_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (vecstr_co::operator) */

bool vecstr_iter::operator == (const vecstr_iter &oit) noex {
	return (va == oit.va) && (i == oit.i) && (ii == oit.ii) ;
}

bool vecstr_iter::operator != (const vecstr_iter &oit) noex {
	bool		f = false ;
	f = f || (va != oit.va) ;
	f = f || (ii != oit.ii) ;
	if (!f) {
	    f = (i < oit.i) ;
	}
	return f ;
} /* end method (vecstr_iter::operator) */

vecstr_iter vecstr_iter::operator + (int n) const noex {
	vecstr_iter	rit(va,i,i) ;
	rit.i = ((rit.i + n) >= 0) ? (rit.i + n) : 0 ;
	return rit ;
}

vecstr_iter &vecstr_iter::operator += (int n) noex {
	increment(n) ;
	return *this ;
}

vecstr_iter vecstr_iter::operator ++ () noex { /* pre */
	increment() ;
	return *this ;
}

vecstr_iter vecstr_iter::operator ++ (int) noex { /* post */
	vecstr_iter	pre(*this) ;
	increment() ;
	return pre ;
}

void vecstr_iter::increment(int n) noex {
	if ((i + n) < 0) n = -i ;
	if (n != 0) {
	    i += n ;
	    while ((i < ii) && (va[i] == nullptr)) {
	        i += 1 ;
	    }
	}
} /* end method (vecstr_iter::increment) */


