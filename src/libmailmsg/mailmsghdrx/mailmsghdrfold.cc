/* mailmsghdrfold SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* manage folding of a mail-message header line */
/* version %I% last-modified %G% */

#define	CF_STRNBREAK	1		/* use |strnbreak()| */

/* revision history:

	= 2009-04-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2009 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mailmsghdrfold

	Description:
	This object module takes a line of text as input and breaks
	it up into pieces that are folded so as to fit in a specified
	number of print-output columns.

	Synopsis:
	int mailmsghdrfold_start(mailmsghdrfold *op,int mcols,int ln,
		cchar *sp,int sl) noex

	Arguments:
	op		object pointer
	mcols		number of total columns available for this line
	ln		line-number within header instance
	sp		header-value string pointer
	sl		header-value string length

	Returns:
	>=0		OK
	<0		bad (system-return)


	Synopsis:
	int mailmsghdrfold_get(mailmsghdrfold *op,int ncol,cchar **rpp) noex

	Arguments:
	op		object pointer
	mcols		number of total columns available for this line
	ln		line-number within header instance
	ncol		current column number (from beginning of line)
	rpp		pointer to resulting line

	Returns:
	>0		length of line
	==0		done
	<0		bad (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<climits>		/* |UINT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<vecobj.h>
#include	<ascii.h>
#include	<strn.h>
#include	<sfx.h>
#include	<six.h>
#include	<char.h>
#include	<mkchar.h>
#include	<ncol.h>
#include	<ischarx.h>
#include	<localmisc.h>		/* |NTABCOLS| */

#include	"mailmsghdrfold.h"


/* local defines */

#define	MF		MAILMSGHDRFOLD

#undef	LINER
#define	LINER		struct liner

#undef	PARAMS
#define	PARAMS		struct params


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static inline int mailmsghdrfold_magic(mailmsghdrfold *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == MAILMSGHDRFOLD_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (mailmsghdrfold_magic) */

static int	findpieces(MF *,int,cchar **) noex ;
static int	findbreaks(MF *,int,cchar **) noex ;
static int	findbreakers(MF *,int,int,cchar **) noex ;
static int	findslices(MF *,int,cchar **) noex ;
static int	findall(MF *,cchar **) noex ;

static int	nextpiece(int,cchar *,int,int *) noex ;
static int	nextbreak(int,int,cchar *,int,int *) noex ;

static bool	isskip(int) noex ;

#if	CF_STRNBREAK
static cchar	*strnbreak(cchar *,int,int) noex ;
#endif


/* local variables */

constexpr cchar		blank[] = " " ;
constexpr cchar		breaks[] = ";:@.%!=" ;


/* exported variables */


/* exported subroutines */

int mailmsghdrfold_start(MF *op,int mcols,int ln,cchar *sp,int sl) noex {
    	MAILMSGHDRFOLD	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && sp) {
	    rs = SR_INVALID ;
	    if ((mcols >= 1) && (ln >= 0)) {
		if (sl < 0) sl = strlen(sp) ;
	        rs = memclear(hop) ;
	        op->mcols = mcols ;
	        op->ln = ln ;
	        while (sl && isskip(*sp)) {
	            sp += 1 ;
	            sl -= 1 ;
	        }
	        while (sl && isskip(sp[sl-1])) {
	            sl -= 1 ;
	        }
	        op->sp = sp ;
	        op->sl = sl ;
	        op->magic = MAILMSGHDRFOLD_MAGIC ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailmsghdrfold_start) */

int mailmsghdrfold_finish(MF *op) noex {
	int		rs = SR_FAULT ;
	if ((rs = mailmsghdrfold_magic(op)) >= 0) {
	    op->sp = nullptr ;
	    op->magic = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailmsghdrfold_finish) */

/* get the resulting lines from the folding operation */
int mailmsghdrfold_get(MF *op,int ncol,cchar **rpp) noex {
	int		rs ;
	int		ll = 0 ;
	if ((rs = mailmsghdrfold_magic(op,rpp)) >= 0) {
	    cint	ntab = NTABCOLS ;
	    rs = SR_INVALID ;
	    if (ncol >= 0) {
		rs = SR_OK ;
	        if (op->sl > 0) {
	            cint	ncols = ncolstr(ntab,ncol,op->sp,op->sl) ;
	            if ((ncol + ncols) > op->mcols) {
	                if ((ll = findpieces(op,ncol,rpp)) == 0) {
	                    if ((ll = findbreaks(op,ncol,rpp)) == 0) {
	                        if ((ll = findslices(op,ncol,rpp)) == 0) {
	                            ll = findall(op,rpp) ;
	                        }
	                    }
	                }
	            } else {
	                ll = findall(op,rpp) ;
	            }
	            op->ln += 1 ;
	        } else {
	            *rpp = op->sp ;
	        } /* end if */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? ll : rs ;
}
/* end subroutine (mailmsghdrfold_get) */


/* private subroutines */

static int findpieces(MF *op,int ncol,cchar **rpp) noex {
	cint		mcols = op->mcols ;
	int		sl = op->sl ;
	int		ll = 0 ;
	cchar		*sp = op->sp ;
	if (sl > 0) {
	    int		nc = ncol ;
	    int		pl ;
	    int		ncs ;
	    while (sl && isskip(sp[0])) {
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	    *rpp = sp ;
	    while ((pl = nextpiece(nc,sp,sl,&ncs)) > 0) {
	        if ((nc + ncs) > mcols) {
	            if (ll == 0) {
	                if (op->ln == 0) {
	                    if (rpp != nullptr) *rpp = blank ;
	                    ll = 1 ;
	                }
	            }
	            break ;
	        } /* end if */
	        ll += pl ;
	        nc += ncs ;
	        sp += pl ;
	        sl -= pl ;
	    } /* end while */
	    if (ll > 0) {
	        while (sl && isskip(sp[0])) {
	            sp += 1 ;
	            sl -= 1 ;
	        } /* end while */
	        op->sp = sp ;
	        op->sl = sl ;
	    } /* end if */
	} else {
	    *rpp = sp ;
	} /* end if (string length) */
	return ll ;
}
/* end subroutine (findpieces) */

static int findbreaks(MF *op,int ncol,cchar **rpp) noex {
	int		ll = 0 ;
	for (int i = 0 ; (ll == 0) && breaks[i] ; i += 1) {
	    ll = findbreakers(op,breaks[i],ncol,rpp) ;
	} /* end for */
	return ll ;
}
/* end subroutine (findbreaks) */

static int findbreakers(MF *op,int bch,int ncol,cchar **rpp) noex {
	cint		mcols = op->mcols ;
	int		sl = op->sl ;
	int		ll = 0 ;
	cchar		*sp = op->sp ;
	if (sl > 0) {
	    int		nc = ncol ;
	    int		pl ;
	    int		ncs ;
	    while (sl && isskip(sp[0])) {
	        sp += 1 ;
	        sl -= 1 ;
	    }
	    *rpp = sp ;
	    while ((pl = nextbreak(nc,bch,sp,sl,&ncs)) > 0) {
	        if ((nc + ncs) > mcols) {
	            break ;
	        }
	        ll += pl ;
	        nc += ncs ;
	        sp += pl ;
	        sl -= pl ;
	    } /* end while */
	    if (ll > 0) {
	        while (sl && isskip(sp[0])) {
	            sp += 1 ;
	            sl -= 1 ;
	        }
	        op->sp = sp ;
	        op->sl = sl ;
	    } /* end if */
	} else {
	    *rpp = sp ;
	} /* end if (string length) */
	return ll ;
}
/* end subroutine (findbreakers) */

static int findslices(MF *op,int ncol,cchar **rpp) noex {
	cint		ntab = NTABCOLS ;
	cint		mcols = op->mcols ;
	int		sl = op->sl ;
	int		ll = 0 ;
	cchar		*sp = op->sp ;
	if (sl > 0) {
	    int		si ;
	    int		nc = ncol ;
	    int		n ;
	    while (sl && isskip(sp[0])) {
	        sp += 1 ;
	        sl -= 1 ;
	    }
	    *rpp = sp ;
	    while (sl && sp[0] && (nc < mcols)) {
	        if (sp[0] == CH_DQUOTE) {
	            nc += 1 ;
	            sp += 1 ;
	            sl -= 1 ;
	            ll += 1 ;
	            n = 0 ;
	            if ((si = sidquote(sp,sl)) > 0) {
	                n = ncolstr(ntab,ncol,sp,sl) ;
	            }
	        } else {
	            si = 1 ;
	            n = ncolchar(ntab,nc,sp[0]) ;
	        } /* end if */
	        nc += n ;
	        sp += si ;
	        sl -= si ;
	        ll += si ;
	    } /* end while */
	    while (sl && isskip(sp[0])) {
	        sp += 1 ;
	        sl -= 1 ;
	    }
	    if (ll > 0) {
	        op->sp = sp ;
	        op->sl = sl ;
	    }
	} else {
	    *rpp = sp ;
	} /* end if (string length) */
	return ll ;
}
/* end subroutine (findslices) */

static int findall(MF *op,cchar **rpp) noex {
	int		cl ;
	cchar		*cp ;
	if ((cl = sfshrink(op->sp,op->sl,&cp)) >= 0) {
	    *rpp = cp ;
	}
	op->sp += op->sl ;
	op->sl = 0 ;
	return cl ;
}
/* end subroutine (findall) */

static int nextpiece(int ncol,cchar *sp,int sl,int *ncp) noex {
	cint		ntab = NTABCOLS ;
	int		ncs = 0 ;
	int		cl = sl ;
	int		si ;
	int		n ;
	int		pl = 0 ;
	cchar		*cp = sp ;
/* skip over whitespace */
	while (cl && CHAR_ISWHITE(cp[0])) {
	    n = ncolchar(ntab,ncol,cp[0]) ;
	    cp += 1 ;
	    cl -= 1 ;
	    ncs += n ;
	    ncol += n ;
	} /* end while */
/* skip over the non-whitespace */
	while (cl && cp[0] && (! CHAR_ISWHITE(cp[0]))) {
	    if (cp[0] == CH_DQUOTE) {
	        cp += 1 ;
	        cl -= 1 ;
	        ncs += 1 ;
	        ncol += 1 ;
	        n = 0 ;
	        if ((si = sidquote(cp,cl)) > 0) {
	            n = ncolstr(ntab,ncol,cp,cl) ;
	        }
	    } else {
	        si = 1 ;
	        n = ncolchar(ntab,ncol,cp[0]) ;
	    } /* end if */
	    cp += si ;
	    cl -= si ;
	    ncs += n ;
	    ncol += n ;
	} /* end while */
/* done */
	*ncp = ncs ;
	pl = (cp - sp) ;
	return pl ;
}
/* end subroutine (nextpiece) */

static int nextbreak(int ncol,int bch,cchar *sp,int sl,int *ncp) noex {
	cint		ntab = NTABCOLS ;
	int		ncs = 0 ;
	int		cl = sl ;
	int		n ;
	int		pl = 0 ;
	cchar		*tp ;
	cchar		*cp = sp ;
	*ncp = 0 ;
/* skip over whitespace */
	while (cl && CHAR_ISWHITE(cp[0])) {
	    n = ncolchar(ntab,ncol,cp[0]) ;
	    cp += 1 ;
	    cl -= 1 ;
	    ncs += n ;
	    ncol += n ;
	} /* end while */
/* find a possible break */
#if	CF_STRNBREAK
	if ((tp = strnbreak(cp,cl,bch)) != nullptr) {
	    n = ncolstr(ntab,ncol,cp,((tp+1)-cp)) ;
	    ncs += n ;
	    pl = ((tp+1) - sp) ;
	    *ncp = ncs ;
	} /* end if */
#else
	if ((tp = strnchr(cp,cl,bch)) != nullptr) {
	    n = ncolstr(ntab,ncol,cp,((tp+1)-cp)) ;
	    ncs += n ;
	    pl = ((tp+1) - sp) ;
	    *ncp = ncs ;
	} /* end if */
#endif /* CF_STRNBREAK */
	*ncp = ncs ;
/* done */
	return pl ;
}
/* end subroutine (nextbreak) */

static bool isskip(int ch) noex {
	return (CHAR_ISWHITE(ch) || iseol(ch)) ;
}
/* end subrouine (isskip) */

#if	CF_STRNBREAK
cchar *strnbreak(cchar *sp,int sl,int bch) noex {
	int		si ;
	bool		f = false ;
	bch &= UCHAR_MAX ;
	while (sl && *sp) {
	    cint	ch = mkchar(sp[0]) ;
	    if (ch == CH_DQUOTE) {
	        sp += 1 ;
	        sl -= 1 ;
	        si = sidquote(sp,sl) ;
	    } else {
	        si = 1 ;
	        f = (ch == bch) ;
	        if (f) break ;
	    }
	    sp += si ;
	    sl -= si ;
	} /* end while */
	return (f) ? sp : nullptr ;
}
/* end subroutine (strnbreak) */
#endif /* CF_STRNBREAK */


