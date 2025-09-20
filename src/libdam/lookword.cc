/* lookword SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* word look-up object */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	lookword

	Description:
	We lookup a word in a dictionary.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<naturalwords.h>
#include	<ascii.h>
#include	<vecobj.h>
#include	<snwcpy.h>
#include	<char.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>		/* |LINEBUFLEN| */

#include	"lookword.h"


/* local defines */

#ifndef	LOGICVAL
#define	LOGICVAL(v)	((v)?1:0)
#endif

/*
 * FOLD and DICT convert characters to a normal form for comparison,
 * according to the user specified flags.
 *
 * DICT expects integers because it uses a non-character value to
 * indicate a character which should not participate in comparisons.
 */
#define NO_COMPARE	(-2)

#define DICT(c)		(isdict(c) ? (c) /* int */ : NO_COMPARE)
#define FOLD(c)		CHAR_TOFC(c)

#define	LW		lookword
#define	LW_CUR		lookword_cur


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static inline int lookword_magic(lookword *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == LOOKWORD_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (lookword_magic) */

static int	lookword_proc(LW *,LW_CUR *,
			cchar *,cchar *,cchar *) noex ;
static int	lookword_mksword(LW *,char *,int,cchar *) noex ;
static int	lookword_record(LW *,LW_CUR *,
			cchar *,cchar *,cchar *) noex ;
static int	lookword_recorder(LW *,LW_CUR *,vecobj *,int) noex ;

static int      compare(LW *,cchar *,cchar *,cchar *,int *) noex ;

static cchar    *binary_search(LW *,cchar *,cchar *,cchar *) noex ;
static cchar    *linear_search(LW *,cchar *,cchar *,cchar *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int lookword_open(LW *op,cchar *dfname,int opts) noex {
	int		rs = SR_FAULT ;
	if (op && dfname) {
	    memclear(op) ;
	    op->fl.dict = LOGICVAL(opts&LOOKWORD_ODICT) ;
	    op->fl.fold = LOGICVAL(opts&LOOKWORD_OFOLD) ;
	    op->fl.word = LOGICVAL(opts&LOOKWORD_OWORD) ;
	    if ((rs = uc_open(dfname,O_RDONLY,0666)) >= 0) {
	        cnullptr	np{} ;
	        cint		fd = rs ;
	        if ((rs = uc_fsize(fd)) >= 0) {
	            size_t	ms = rs ;
	            cint	mp = PROT_READ ;
	            cint	mf = MAP_SHARED ;
	            void	*md{} ;
	            if ((rs = u_mmapbegin(np,ms,mp,mf,fd,0z,&md)) >= 0) {
	                op->fd = fd ;
	                op->md = md ;
	                op->ms = ms ;
	                op->magic = LOOKWORD_MAGIC ;
	            } /* end if (mapfile) */
	        } /* end if (fsize) */
	        if (rs < 0) {
	            u_close(fd) ;
	        }
	    } /* end if (file-open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (lookword_open) */

int lookword_close(LW *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = lookword_magic(op)) >= 0) {
	    if (op->md) {
	        rs1 = u_mmapend(op->md,op->ms) ;
	        if (rs >= 0) rs = rs1 ;
	        op->md = nullptr ;
	        op->ms = 0 ;
	    }
	    if (op->fd >= 0) {
	        rs1 = u_close(op->fd) ;
	        if (rs >= 0) rs = rs1 ;
	        op->fd = -1 ;
	    }
	    op->magic = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (lookword_close) */

int lookword_curbegin(LW *op,LW_CUR *curp) noex {
	int		rs ;
	if ((rs = lookword_magic(op,curp)) >= 0) {
	    rs = memclear(curp) ;
	}
	return rs ;
}
/* end subroutine (lookword_curbegin) */

int lookword_curend(LW *op,LW_CUR *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = lookword_magic(op,curp)) >= 0) {
	    if (curp->ans) {
	        rs1 = uc_free(curp->ans) ;
	        if (rs >= 0) rs = rs1 ;
	        curp->ans = nullptr ;
	    }
	    curp->n = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (lookword_curend) */

int lookword_lookup(LW *op,LW_CUR *curp,cchar *wstr) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = lookword_magic(op,curp,wstr)) >= 0) {
	    cint	slen = NATURALWORDLEN ;
	    char	sbuf[NATURALWORDLEN+1] ;
	    if ((rs = lookword_mksword(op,sbuf,slen,wstr)) > 0) {
	        cchar	*front = charp(op->md) ;
	        cchar	*back = (charp(op->md) + op->ms) ;
	        rs = lookword_proc(op,curp,front,back,sbuf) ;
	        c = rs ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (lookword_lookup) */

int lookword_read(LW *op,LW_CUR *curp,char *rbuf,int rlen) noex {
	int		rs ;
	int		rl = 0 ;
	if ((rs = lookword_magic(op,curp,rbuf)) >= 0) {
	    rbuf[0] = '\0' ;
	    if (curp->i < curp->n) {
	        LOOKWORD_WORD	*ap = (curp->ans + curp->i) ;
	        if ((rs = snwcpy(rbuf,rlen,ap->wp,ap->wl)) >= 0) {
	            rl = rs ;
	            curp->i += 1 ;
	        }
	    } /* end if */
	} /* end if (magic) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (lookword_read) */


/* private subroutines */

static int lookword_proc(LW *op,LW_CUR *curp,
		cc *front,cc *back,cc *wstr) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	front = binary_search(op,front,back,wstr) ;
	front = linear_search(op,front,back,wstr) ;
	if (front) {
	    rs = lookword_record(op,curp,front,back,wstr) ;
	    c = rs ;
	}
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (lookword_proc) */

static int lookword_mksword(LW *op,char *rbuf,int rlen,cchar *s) noex {
	int		rs = SR_OK ;
	int		i ; /* used-afterwards */
	int		dch, fch ;
	cchar		*readp = s ;
	char		*writep = rbuf ;
	for (i = 0 ; (i < rlen) && (s[i] != '\0') ; i += 1) {
	    cint	ch = mkchar(*readp++) ;
	    if (ch == 0) break ;
	    dch = (op->fl.dict) ? DICT(ch) : ch ;
	    if (dch != NO_COMPARE) {
	        fch = (op->fl.fold) ? FOLD(dch) : dch ;
	        *(writep++) = fch ;
	    }
	} /* end for */
	*writep = '\0' ;
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (lookword_mksword) */

static int lookword_record(LW *op,LW_CUR *curp,
		cc *front,cc *back,cc *wstr) noex {
	cnullptr	np{} ;
	vecobj		ans ;
	cint		esize = szof(LOOKWORD_WORD) ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = vecobj_start(&ans,esize,1,0)) >= 0) {
	    LOOKWORD_WORD	w ;
	    int			m ;
	    bool		f_mat = true ;
	    cchar		*tp ;
	    auto cmp = [&] () noex -> bool {
		bool	f = true ;
		f = f && (front < back) ;
		f = f && (compare(op,front,back,wstr,np) == 0) ;
		return f ;
	    } ;
	    while ((rs >= 0) && cmp()) {
	        if ((tp = strchr(front,'\n')) != np) {
	            if (op->fl.word) {
	                f_mat = (compare(op,front,tp,wstr,&m) == 0) ;
	                f_mat = f_mat && 
	                    (wstr[m] == '\0') && (front[m] == '\n') ;
	                if (f_mat) {
	                    c += 1 ;
	                    w.wp = front ;
	                    w.wl = (tp-front) ;
	                    rs = vecobj_add(&ans,&w) ;
	                }
	            } else {
	                c += 1 ;
	                w.wp = front ;
	                w.wl = (tp-front) ;
	                rs = vecobj_add(&ans,&w) ;
	            } /* end if (whole or partial) */
	            front = (tp + 1) ;
	        } else {
	            front = back ;
		}
	    } /* end while */
	    if (rs >= 0) {
	        rs = lookword_recorder(op,curp,&ans,c) ;
	    }
	    rs1 = vecobj_finish(&ans) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vecobj) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (lookword_record) */

static int lookword_recorder(LW *op,LW_CUR *curp,vecobj *alp,int c) noex {
	int		rs = SR_FAULT ;
	int		n = 0 ;
	if (op) {
	    cint	asize = ((c+1) * szof(LOOKWORD_WORD)) ;
	    void	*p ;
	    if (curp->ans) {
	        uc_free(curp->ans) ;
	        curp->ans = nullptr ;
	    }
	    if ((rs = uc_malloc(asize,&p)) >= 0) {
	        LOOKWORD_WORD	*ans = (LOOKWORD_WORD *) p ;
	        void		*vp{} ;
	        curp->ans = (LOOKWORD_WORD *) p ;
	        for (int i = 0 ; vecobj_get(alp,i,&vp) >= 0 ; i += 1) {
	            if (vp) {
	    	        LOOKWORD_WORD	*ep = (LOOKWORD_WORD *) vp ;
	                ans[n].wp = ep->wp ;
	                ans[n].wl = ep->wl ;
	                n += 1 ;
	            }
	        } /* end for */
	        ans[n].wp = nullptr ;
	        ans[n].wl = 0 ;
	        curp->n = n ;
	    } /* end if (m-a) */
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (lookword_recorder) */

/***
 * Binary search for "string" in memory between "front" and "back".
 *
 * This routine is expected to return a pointer to the start of a
 * line at *or before* the first word matching "string".  Relaxing
 * the constraint this way simplifies the algorithm.
 *
 * Invariants:
 * 	'front' points to the beginning of a line at or before the first
 *	matching string.
 *
 * 	'back' points to the beginning of a line at or after the first
 *	matching line.
 *
 * Base of the Invariants.
 * 	front = nullptr;
 *	back = EOF;
 *
 * Advancing the Invariants:
 *
 * 	p = first newline after halfway point from front to back.
 *
 * 	If the string at "p" is not greater than the string to match,
 *	p is the new front.  Otherwise it is the new back.
 *
 * Termination:
 *
 * 	The definition of the routine allows it [to] return at any point,
 *	since front is always at or before the line to print.
 *
 *	In fact, it returns when the chosen "p" equals "back".  This
 *	implies that there exists a string [that] is [at] least
 * 	half as long as (back - front), which in turn implies that
 *	a linear search will be no more expensive than the cost of
 *	simply printing a string or two.
 *
 *	Trying to continue with binary search at this point would be 
 *	more trouble than it's worth.  
 *
****/

#define	SKNL(p,back) \
	    while (((p) < (back)) && (*(p)++ != '\n')) ;

static cchar *binary_search(LW *op,cc *front,cc *back,cc *wstr) noex {
	cchar		*p = front + ((back - front) / 2) ;
	SKNL(p,back) ;
	while ((p < back) && (back > front)) {
	    if (compare(op,p,back,wstr,nullptr) > 0) {
	        front = p ;
	    } else {
	        back = p ;
	    }
	    p = front + ((back - front) / 2) ;
	    SKNL(p,back) ;
	} /* end while */
	return (front) ;
}
/* end subroutine (binary_search) */

/****
  Find the first line that starts with string, linearly searching
  from front to back.  Return nullptr for no such line.

  This routine assumes: 
  + front points at the first character in a line 
  + front is before or at the first line to be printed

 ****/

static cchar *linear_search(LW *op,cc *front,cc *back,cc *wstr) noex {
	int		rc = 0 ;
	while (front < back) {
	    rc = compare(op,front,back,wstr,nullptr) ;
	    if (rc <= 0) break ;
	    SKNL(front,back) ;
	} /* end while */
	return ((rc == 0) ? front : nullptr) ;
}
/* end subroutine (linear_search) */

/****
 Return LESS, GREATER, or EQUAL depending on how the string1 compares
 with string2 (s1 ??? s2).

 * 	o Matches up to len(s1) are EQUAL.
 *	o Matches up to len(s2) are GREATER.
 *
 * The string "s1" is NUL terminated.  The string s2 is '\n' terminated
 * (or "back" terminated).
 ****/

static int compare(LW *op,cchar *s2,cchar *back,cchar *s1,int *rp) noex {
	int		ch1, ch2 ;
	int		fch1, fch2 ;
	int		i = 0 ;
	int		j = 0 ;
	int		rc = 0 ;
	int		f = false ;
	while (s1[i] && ((s2+j) < back) && (s2[j] != '\n')) {
	    ch1 = mkchar(s1[i]) ;
	    ch2 = mkchar(s2[j]) ;
	    if (op->fl.dict && (! isdict(ch2))) {
	        j += 1 ;		/* ignore character in comparison */
	        continue ;
	    }
	    if (op->fl.fold) {
	        fch1 = FOLD(ch1) ;
	        fch2 = FOLD(ch2) ;
	        rc = (fch1 - fch2) ;
	    } else {
	        rc = (ch1 - ch2) ;
	    }
	    if (rc != 0) {
	        f = true ;
	        break ;
	    }
	    i += 1 ;
	    j += 1 ;
	} /* end while */
	if ((! f) && (rc == 0)) {
	    rc = ((s1[i]) ? 1 : 0) ;
	}
	if (rp) *rp = j ;
	return rc ;
}
/* end subroutine (compare) */


