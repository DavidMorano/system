/* linecenter SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* text fill for line-centering */
/* version %I% last-modified %G% */

#define	CF_SAFE		1		/* (some) safety */

/* revision history:

	= 2008-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	linecenter

	Description:
	This object mediates filling out text to an output file
	using the BFILE package.

*******************************************************************************/

#include	<envstandards.h>	/* must be before others */
#include	<sys/param.h>
#include	<limits.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<string.h>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<estrings.h>
#include	<fifostr.h>
#include	<ascii.h>
#include	<strn.h>
#include	<sfx.h>
#include	<sncpyx.h>
#include	<localmisc.h>

#include	"linecenter.h"


/* local defines */

#define	LC		linecenter

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


/* exported variables */


/* exported subroutines */

int linecenter_start(LC *op,cc *fn) noex {
	cint		ne = LINECENTER_DEFLINES ;
	int		rs ;
	if ((rs = linecenter_ctor(op,fn)) >= 0) {
	    if ((rs = sncpy1(op->fn,fnlen,fn)) >= 0) {
	        if ((rs = fifostr_start(op->sqp)) >= 0) {
	            int	sz = (ne + 1) * szof(cchar **) ;
	            if ((rs = uc_malloc(sz,&op->lines)) >= 0) {
	                op->le = ne ;
	                op->magic = LINECENTER_MAGIC ;
	            }
	            if (rs < 0)
	                fifostr_finish(op->sqp) ;
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
	    {
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
	    if (llen < 0) llen = strlen(lbuf) ;
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
	int		rs = SR_OK ;
	int		sl, cl ;
	int		c = 0 ;
	cchar	*sp, *cp ;

	if (op == nullptr) return SR_FAULT ;
	if (lbuf == nullptr) return SR_FAULT ;

#if	CF_SAFE
	if (op->magic != LINECENTER_MAGIC)
	    return SR_NOTOPEN ;
#endif

	if (llen < 0)
	    llen = strlen(lbuf) ;

	sp = lbuf ;
	sl = llen ;
	while ((cl = nextfield(sp,sl,&cp)) > 0) {

	    c += 1 ;
	    rs = fifostr_add(op->sqp,cp,cl) ;
	    if (rs < 0)
	        break ;

	    if (rs >= 0) {
	        op->wc += 1 ;
	        op->ccnt += cl ;
	    }

	    sl -= ((cp + cl) - sp) ;
	    sp = (cp + cl) ;

	} /* end while */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (linecenter_addline) */

int linecenter_addlines(LC *op,cc *lbuf,int llen) noex {
	int		rs = SR_OK ;
	int		sl, cl ;
	int		c = 0 ;
	cchar		*tp, *sp, *cp ;

	if (op == nullptr) return SR_FAULT ;
	if (lbuf == nullptr) return SR_FAULT ;

#if	CF_SAFE
	if (op->magic != LINECENTER_MAGIC)
	    return SR_NOTOPEN ;
#endif

	if (llen < 0)
	    llen = strlen(lbuf) ;

	sp = lbuf ;
	sl = llen ;
	while (sl > 0) {

	    while ((cl = nextfield(sp,sl,&cp)) > 0) {

	        c += 1 ;
	        rs = fifostr_add(op->sqp,cp,cl) ;
	        if (rs < 0)
	            break ;

	        if (rs >= 0) {
	            op->wc += 1 ;
	            op->ccnt += cl ;
	        }

	        sl -= ((cp + cl) - sp) ;
	        sp = (cp + cl) ;

	    } /* end while */

	    if (rs < 0)
	        break ;

	    if ((tp = strnchr(sp,sl,CH_NL)) == nullptr)
	        break ;

	    sl -= ((tp + 1) - sp) ;
	    sp = (tp + 1) ;

	} /* end while (reading lines) */

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
	int		rs = SR_OK ;
	int		len = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (lpp == nullptr) return SR_FAULT ;

#if	CF_SAFE
	if (op->magic != LINECENTER_MAGIC)
	    return SR_NOTOPEN ;
#endif

	if (i < 0)
	    return SR_INVALID ;

	*lpp = nullptr ;

	if (i < op->li) {
	    *lpp = op->lines[i] ;
	    len = strlen(*lpp) ;
	} else
	    rs = SR_NOTFOUND ;

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (linecenter_getline) */

int linecenter_mklines(LC *op,int llen,int linebrklen) noex {
	int		rs = SR_OK ;
	int		sz ;
	int		len ;
	int		linetmplen ;
	int		c = 0 ;
	char		*lbuf = nullptr ;

	if (op == nullptr)
	    return SR_FAULT ;

#if	CF_SAFE
	if (op->magic != LINECENTER_MAGIC)
	    return SR_NOTOPEN ;
#endif

	if (llen < 1)
	    return SR_INVALID ;

	sz = (llen + 2) ;
	if ((rs = uc_malloc(sz,&lbuf)) >= 0) {

	    if (linebrklen <= 0)
	        linebrklen = lenpercent(llen,LINECENTER_DEFPERCENT) ;

	    while (rs >= 0) {
	        linetmplen = linecenter_hasbrk(op,llen,linebrklen) ;
	        if (linetmplen <= 0) linetmplen = llen ;
	        rs = linecenter_mklinefull(op,lbuf,linetmplen) ;
	        if (rs == 0) break ;
	        if (rs >= 0) {
	            len = rs ;
	            rs = linecenter_storeline(op,lbuf,len) ;
	            c += 1 ;
	        }
	    } /* end while */

	    while (rs >= 0) {
	        linetmplen = linecenter_hasbrk(op,llen,linebrklen) ;
	        if (linetmplen <= 0) linetmplen = llen ;
	        rs = linecenter_mklinepart(op,lbuf,linetmplen) ;
	        if (rs == 0) break ;
	        if (rs >= 0) {
	            len = rs ;
	            rs = linecenter_storeline(op,lbuf,len) ;
	            c += 1 ;
	        }
	    } /* end while */

	    uc_free(lbuf) ;
	} /* end if (m-a) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (linecenter_mklines) */


/* private subroutines */

static int linecenter_storeline(LC *op,cc *lbuf,int llen) noex {
	int		rs = SR_OK ;

	if (llen < 0)
	    return SR_INVALID ;

	if ((op->li + 1) >= op->le) {
	    int		sz ;
	    int		ne = (op->le + 5) ;
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

	return rs ;
}
/* end subroutine (linecenter_storeline) */

static int linecenter_mkline(LC *op,int f_part,char *lbuf,int llen) noex {
	int		rs ;
	int		rs1 ;
	int		tlen = 0 ;
	if ((rs = linecenter_magic(op,lbuf)) >= 0) {
	    rs = SR_INVALID ;
	    if (llen >= 1) {
	int		ll ;
	int		wl, nl ;
	int		ql = 0 ;
	int		f_give = f_part ;
	char		*lp ;
	if (! f_part) {

#ifdef	COMMENT
	    {
		fifostr		*sqp = op->sqp ;
	    ql = 0 ;
	    c = 0 ;
	    for (int i = 0 ; (wl = fifostr_entlen(sqp,i)) >= 0 ; i += 1) {
	        if (wl == 0) continue ;
	        if (c++ > 0) {
	            ql += 1 ;
		}
	        ql += wl ;
	        if (ql >= (llen - 1)) {
	            f_give = true ;
	            break ;
	        }
	    } /* end for */
	    } /* end block */
#else /* COMMENT */
	    ql = op->ccnt ;
	    if (op->wc > 0) {
		ql += (op->wc - 1) ;
	    }
	    if (ql >= (llen - 1)) {
	        f_give = true ;
	    }
#endif /* COMMENT */

	} /* end if (not requested partial) */

	if (f_give) {
	    int		c = 0 ;
	    lp = lbuf ;
	    ll = llen ;
	    while (ll > 0) {

	        rs1 = fifostr_headlen(op->sqp) ;
	        wl = rs1 ;
	        if (rs1 == SR_NOTFOUND) break ;

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

	        rs = fifostr_rem(op->sqp,lp,ll) ;
	        if (rs < 0)
	            break ;

	        if (rs >= 0) {
	            op->wc -= 1 ;
	            op->ccnt -= wl ;
	        }

	        lp += wl ;
	        ll -= wl ;

	    } /* end while (getting words) */

	    tlen = (llen - ll) ;

	} /* end if (giving) */

	    } /* end if (valid) */
	} /* end if (linecenter_magic) */
	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (linecenter_mkline) */

static int linecenter_hasbrk(LC *op,int llen,int brklen) noex {
	int		rs = SR_OK ;
	int		i ;
	int		len ;
	int		wl ;
	int		curlen ;
	int		tmplen = 0 ;

	curlen = op->ccnt ;
	if (op->wc > 0)
	    curlen += (op->wc - 1) ;

	if (curlen > brklen) {
	    cint	wlen = WORDBUFLEN ;
	    cchar	*wp ;
	    char	wbuf[WORDBUFLEN + 1] ;
	    wp = wbuf ;
	    len = 0 ;
	    for (i = 0 ; (wl = fifostr_entread(op->sqp,wbuf,wlen,i)) >= 0 ; 
	        i += 1) {

	    if (wl == 0) continue ;
	    len = wl ;

	    if (i > 0)
	        len += 1 ;

	    if (len >= llen)
	        break ;

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
	return flen ;
}
/* end subroutine (lenpercent) */


