/* linecenter SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* text fill for line-centering */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	linecenter

	Description:
	This object mediates filling out text to an output file
	using the BFILE package.

*******************************************************************************/

#include	<envstandards.h>	/* must be before others */
#include	<unistd.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<estrings.h>
#include	<mallocxx.h>
#include	<fifostr.h>
#include	<ascii.h>
#include	<strn.h>
#include	<sfx.h>
#include	<sncpyx.h>
#include	<localmisc.h>

#include	"linecenter.h"

import libutil ;

/* local defines */

#define	LC		linecenter
#define	LC_CUR		linecenter_cur
#define	LC_ENT		linecenter_ent

#ifndef	WORDBUFLEN
#define	WORDBUFLEN	100
#endif


/* local namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int linecenter_ctor(linecenter *op,Args ... args) noex {
    	LINECENTER	*hop = op  ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->sqp = new(nothrow) fifostr) != np) {
		rs = SR_OK ;
	    } /* end if (new-fifostr) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (linecenter_ctor) */

static int linecenter_dtor(linecenter *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->sqp) {
		delete op->sqp ;
		op->sqp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (linecenter_dtor) */

template<typename ... Args>
static inline int linecenter_magic(linecenter *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == LINECENTER_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (linecenter_magic) */

static int	linecenter_mkline(LC *,int,char *,int) noex ;
static int	linecenter_storeline(LC *,cchar *,int) noex ;
static int	linecenter_hasbrk(LC *,int,int) noex ;

static int	lenpercent(int,double) noex ;


/* local variables */

constexpr int		fnlen = LINECENTER_FNLEN ;

constexpr double	defpercent = LINECENTER_DEFPERCENT ;

cbool			f_comment = false ;


/* exported variables */


/* exported subroutines */

int linecenter_start(LC *op,cc *fn) noex {
	cint		ne = LINECENTER_DEFLINES ;
	int		rs ;
	if ((rs = linecenter_ctor(op,fn)) >= 0) {
	    if ((rs = sncpy(op->fn,fnlen,fn)) >= 0) {
	        if ((rs = fifostr_start(op->sqp)) >= 0) {
	            cint	sz = (ne + 1) * szof(cchar **) ;
		    if (void *vp ; (rs = uc_malloc(sz,&vp)) >= 0) {
			op->lines = ccharpp(vp) ;
	                op->le = ne ;
	                op->magic = LINECENTER_MAGIC ;
	            }
	            if (rs < 0) {
	                fifostr_finish(op->sqp) ;
		    }
	        } /* end if (fifostr_start) */
	    } /* end if (sncpy) */
	    if (rs < 0) {
		linecenter_dtor(op) ;
	    }
	} /* end if (linecenter_ctor) */
	return rs ;
}
/* end subroutine (linecenter_start) */

int linecenter_finish(LC *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = linecenter_magic(op)) >= 0) {
	    if (op->lines) {
	        for (int i = 0 ; i < op->li ; i += 1) {
	            if (op->lines[i] != nullptr) {
	                rs1 = uc_free(op->lines[i]) ;
		        if (rs >= 0) rs = rs1 ;
		    }
	        }
	        {
	            rs1 = uc_free(op->lines) ;
	            if (rs >= 0) rs = rs1 ;
	            op->lines = nullptr ;
	        }
	    } /* end if */
	    if (op->sqp) {
	        rs1 = fifostr_finish(op->sqp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = linecenter_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (linecenter_magic) */
	return rs ;
}
/* end subroutine (linecenter_finish) */

int linecenter_addword(LC *op,cc *lbuf,int llen) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = linecenter_magic(op,lbuf)) >= 0) {
	    if (llen < 0) llen = lenstr(lbuf) ;
	    if (llen > 0) {
	        c += 1 ;
	        rs = fifostr_add(op->sqp,lbuf,llen) ;
	    }
	    if (rs >= 0) {
	        op->wc += 1 ;
	        op->ccnt += llen ;
	    }
	} /* end if (linecenter_magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (linecenter_addword) */

int linecenter_addline(LC *op,cc *lbuf,int llen) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = linecenter_magic(op,lbuf)) >= 0) {
	    int		sl ;
	    cchar	*sp ;
	    cchar	*cp ;
	    if (llen < 0) llen = lenstr(lbuf) ;
	    sp = lbuf ;
	    sl = llen ;
	    for (int cl ; (cl = sfnext(sp,sl,&cp)) > 0 ; ) {
	        c += 1 ;
	        if ((rs = fifostr_add(op->sqp,cp,cl)) >= 0) {
	            op->wc += 1 ;
	            op->ccnt += cl ;
	        }
	        sl -= intconv((cp + cl) - sp) ;
	        sp = (cp + cl) ;
		if (rs < 0) break ;
	    } /* end for */
	} /* end if (linecenter_magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (linecenter_addline) */

int linecenter_addlines(LC *op,cc *lbuf,int llen) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = linecenter_magic(op,lbuf)) >= 0) {
	    int		sl, cl ;
	    cchar	*tp, *sp, *cp ;
	    if (llen < 0) llen = lenstr(lbuf) ;
	    sp = lbuf ;
	    sl = llen ;
	    while (sl > 0) {
	        while ((cl = sfnext(sp,sl,&cp)) > 0) {
	            c += 1 ;
	            if ((rs = fifostr_add(op->sqp,cp,cl)) >= 0) {
	                op->wc += 1 ;
	                op->ccnt += cl ;
	            }
	            sl -= intconv((cp + cl) - sp) ;
	            sp = (cp + cl) ;
		    if (rs < 0) break ;
	        } /* end while */
	        if (rs < 0) break ;
	        if ((tp = strnchr(sp,sl,CH_NL)) == nullptr) break ;
	        sl -= intconv((tp + 1) - sp) ;
	        sp = (tp + 1) ;
	    } /* end while (reading lines) */
	} /* end if (linecenter_magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (linecenter_addlines) */

int linecenter_mklinefull(LC *op,char *lbuf,int llen) noex {
	return linecenter_mkline(op,false,lbuf,llen) ;
}
/* end subroutine (linecenter_mklinefull) */

int linecenter_mklinepart(LC *op,char *lbuf,int llen) noex {
	return linecenter_mkline(op,true,lbuf,llen) ;
}
/* end subroutine (linecenter_mklinepart) */

int linecenter_getline(LC *op,int i,cchar **lpp) noex {
	int		rs ;
	int		len = 0 ; /* return-value */
	if ((rs = linecenter_magic(op,lpp)) >= 0) {
	    rs = SR_INVALID ;
	    if (i >= 0) {
		*lpp = nullptr ;
	        if (i < op->li) {
	            *lpp = op->lines[i] ;
	            len = lenstr(*lpp) ;
		    rs = SR_OK ;
	        } else {
	            rs = SR_NOTFOUND ;
		}
	    } /* end if (OK) */
	} /* end if (linecenter_magic) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (linecenter_getline) */

int linecenter_mklines(LC *op,int lwidth,int lbrk) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = linecenter_magic(op)) >= 0) {
	    rs = SR_INVALID ;
	    if (lwidth >= 1) {
	        if (char *lbuf ; (rs = malloc_ml(&lbuf)) >= 0) {
	            cint	llen = rs ;
	            int		linetmplen ;
	            if (lbrk <= 0) {
	                lbrk = lenpercent(lwidth,defpercent) ;
	            }
	            while (rs >= 0) {
		        auto	lcmf = linecenter_mklinefull ;
	                linetmplen = linecenter_hasbrk(op,lwidth,lbrk) ;
	                if (linetmplen <= 0) linetmplen = llen ;
	                if ((rs = lcmf(op,lbuf,linetmplen)) > 0) {
	                    rs = linecenter_storeline(op,lbuf,rs) ;
	                    c += 1 ;
	                }
		        if (rs == 0) break ;
	            } /* end while */
	            while (rs >= 0) {
		        auto	lcmp = linecenter_mklinepart ;
	                linetmplen = linecenter_hasbrk(op,lwidth,lbrk) ;
	                if (linetmplen <= 0) linetmplen = llen ;
	                if ((rs = lcmp(op,lbuf,linetmplen)) > 0) {
	                    rs = linecenter_storeline(op,lbuf,rs) ;
	                    c += 1 ;
	                }
	                if (rs == 0) break ;
	            } /* end while */
	            rs1 = uc_free(lbuf) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a) */
	    } /* end if (valid) */
	} /* end if (linecenter_magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (linecenter_mklines) */


/* private subroutines */

static int linecenter_storeline(LC *op,cc *lbuf,int llen) noex {
	int		rs = SR_INVALID ;
	if (llen >= 0) {
	    rs = SR_OK ;
	    if ((op->li + 1) >= op->le) {
	        int	sz ;
	        int	ne = (op->le + 5) ;
	        void	**nlines = nullptr ;
	        sz = (ne + 1) * szof(char **) ;
	        if ((rs = uc_realloc(op->lines,sz,&nlines)) >= 0) {
	            op->le = ne ;
	            op->lines = (cchar **) nlines ;
	        }
	    } /* end if */
	    if (rs >= 0) {
	        if (cchar *cp ; (rs = uc_mallocstrw(lbuf,llen,&cp)) >= 0) {
	            op->lines[op->li] = cp ;
	            op->li += 1 ;
	        }
	    } /* end if */
	} /* end if (valid) */
	return rs ;
}
/* end subroutine (linecenter_storeline) */

static int linecenter_mkline(LC *op,int f_part,char *lbuf,int llen) noex {
	int		rs ;
	int		rs1 ;
	int		tlen = 0 ; /* return-value */
	if ((rs = linecenter_magic(op,lbuf)) >= 0) {
	    rs = SR_INVALID ;
	    if (llen >= 1) {
		fifostr		*sqp = op->sqp ;
	        int		wl, nl ;
	        int		ql = 0 ;
	        int		f_give = f_part ;
		rs = SR_OK ;
	        if (! f_part) {
		    if_constexpr (f_comment) {
			int	c = 0 ;
			cauto	fsel = fifostr_entlen ;
	                ql = 0 ;
	                for (int i = 0 ; (wl = fsel(sqp,i)) >= 0 ; i += 1) {
	                    if (wl > 0) {
	                        if (c++ > 0) {
	                            ql += 1 ;
		                }
	                        ql += wl ;
	                        if (ql >= (llen - 1)) {
	                            f_give = true ;
	                            break ;
	                        }
			    }
	                } /* end for */
	            } else {
	                ql = op->ccnt ;
	                if (op->wc > 0) {
		            ql += (op->wc - 1) ;
	                }
	                if (ql >= (llen - 1)) {
	                    f_give = true ;
	                }
		    } /* end if_constexpr (f_comment) */
	        } /* end if (not requested partial) */
	        if ((rs >= 0) && f_give) {
	            int		c = 0 ;
	            int		ll = llen ;
	            char	*lp = lbuf ;
	            while (ll > 0) {
	                rs1 = sqp->headlen ;
	                wl = rs1 ;
	                if (rs1 == SR_NOTFOUND) break ;
			rs = rs1 ;
        		/* ignore zero-length words */
	                if (wl == 0) continue ;
        		/* calculate needed-length ('nl') for this word */
	                nl = (c > 0) ? (wl + 1) : wl ;
		        /* can this word fit in the current line? */
	                if (nl > ll) break ;
		        /* yes: so remove the word from the FIFO to the line */
	                if (c++ > 0) {
	                    *lp++ = ' ' ;
	                    ll -= 1 ;
	                }
			if (rs >= 0) {
	                    if ((rs = sqp->rem(lp,ll)) >= 0) {
	                        op->wc -= 1 ;
	                        op->ccnt -= wl ;
			    }
	                }
	                lp += wl ;
	                ll -= wl ;
			if (rs < 0) break ;
	            } /* end while (getting words) */
	            tlen = (llen - ll) ;
	        } /* end if (giving) */
	    } /* end if (valid) */
	} /* end if (linecenter_magic) */
	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (linecenter_mkline) */

static int linecenter_hasbrk(LC *op,int lwidth,int brklen) noex {
	int		rs = SR_OK ;
	int		curlen = op->ccnt ;
	int		tmplen = 0 ; /* return-value */
	if (op->wc > 0) {
	    curlen += (op->wc - 1) ;
	}
	if (curlen > brklen) {
	    cint	wlen = WORDBUFLEN ;
	    int		wl ;
	    cchar	*wp ;
	    char	wbuf[WORDBUFLEN + 1] ;
	    cauto	fser = fifostr_entread ;
	    wp = wbuf ;
	    for (int i = 0 ; (wl = fser(op->sqp,wbuf,wlen,i)) >= 0 ; i += 1) {
		int	len = wl ;
	        if (wl == 0) continue ;
	        if (i > 0) len += 1 ;
	        if (len >= lwidth) break ;
	        if (len >= brklen) {
	            if ((wp[wl-1] == ',') || (wp[wl-1] == '.')) {
	                tmplen = len ;
	                break ;
	            }
	        } /* end if (break-length test) */
	    } /* end for */
	} /* end if (greater) */
	return (rs >= 0) ? tmplen : rs ;
}
/* end subroutine (linecenter_hasbrk) */

static int lenpercent(int len,double percent) noex {
	double		flen = len ;
	flen *= percent ;
	return int(flen) ;
}
/* end subroutine (lenpercent) */


