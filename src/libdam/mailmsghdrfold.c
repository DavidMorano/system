/* mailmsghdrfold */
/* lang=C20 */

/* manage folding of a mail-message header line */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debug print-outs */
#define	CF_STRNBREAK	1		/* use 'strnbreak()' */

/* revision history:

	= 2009-04-10, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 2009 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Description:
	This object module takes a line of text as input and breaks
	it up into pieces that are folded so as to fit in a specified
	number of print-output columns.

	Synopsis:
	int mailmsghdrfold_start(op,mcols,ln,sp,sl)
	MAILMSGHDRFOLD	*op ;
	int		mcols ;
	int		ln ;
	cchar		*sp ;
	int		sl ;

	Arguments:
	op		object pointer
	mcols		number of total columns available for this line
	ln		line-number within header instance
	sp		header-value string pointer
	sl		header-value string length

	Returns:
	<0		bad
	>=0		OK


	Synopsis:
	int mailmsghdrfold_get(op,ncol,rpp)
	MAILMSGHDRFOLD	*op ;
	int		ncol ;
	cchar	**rpp ;

	Arguments:
	op		object pointer
	mcols		number of total columns available for this line
	ln		line-number within header instance
	ncol		current column number (from beginning of line)
	rpp		pointer to resulting line

	Returns:
	<0		bad
	==0		done
	>0		length of line

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<limits.h>
#include	<string.h>
#include	<usystem.h>
#include	<vecobj.h>
#include	<ascii.h>
#include	<char.h>
#include	<mkchar.h>
#include	<localmisc.h>
#include	<sfx.h>
#include	<six.h>

#include	"mailmsghdrfold.h"


/* local defines */

#define	MF		MAILMSGHDRFOLD

#undef	LINER
#define	LINER		struct liner

#undef	PARAMS
#define	PARAMS		struct params

#ifndef	NTABCOLS
#define	NTABCOLS	8
#endif


/* external subroutines */

extern int	ncolstr(int,int,cchar *,int) noex ;
extern int	ncolchar(int,int,int) noex ;

#if	CF_DEBUGS
extern int	debugprintf(cchar *,...) noex ;
extern int	strlinelen(cchar *,int,int) noex ;
#endif

extern char	*strnchr(cchar *,int,int) noex ;
extern char	*strnpbrk(cchar *,int,cchar *) noex ;


/* external variables */


/* local structures */


/* forward references */

static int	findpieces(MF *,int,cchar **) noex ;
static int	findbreaks(MF *,int,cchar **) noex ;
static int	findbreakers(MF *,int,int,cchar **) noex ;
static int	findslices(MF *,int,cchar **) noex ;
static int	findall(MF *,cchar **) noex ;

static int	nextpiece(int,cchar *,int,int *) noex ;
static int	nextbreak(int,int,cchar *,int,int *) noex ;
static int	isskip(int) noex ;
static int	isend(int) noex ;

#if	CF_STRNBREAK
static cchar	*strnbreak(cchar *,int,int) noex ;
#endif


/* local variables */

static cchar	*blank = " " ;
static cchar	*breaks = ";:@.%!=" ;


/* exported subroutines */

int mailmsghdrfold_start(MF *op,int mcols,int ln,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (op && sp) {
	    rs = SR_INVALID ;
	    if ((mcols >= 1) && (ln >= 0)) {
		rs = SR_OK ;
		if (sl < 0) sl = strlen(sp) ;
	        memclear(op) ;
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
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->magic == MAILMSGHDRFOLD_MAGIC) {
	        op->magic = 0 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailmsghdrfold_finish) */

/* get the resulting lines from the folding operation */
int mailmsghdrfold_get(MF *op,int ncol,cchar **rpp) noex {
	const int	ntab = NTABCOLS ;
	int		ll = 0 ;

	if (op == NULL) return SR_FAULT ;
	if (rpp == NULL) return SR_FAULT ;

	if (op->magic != MAILMSGHDRFOLD_MAGIC) return SR_NOTOPEN ;

	if (ncol < 0) return SR_INVALID ;

#if	CF_DEBUGS
	debugprintf("mailmsghdrfold_get: ent mcols=%u ccol=%u\n",
	    op->mcols,ncol) ;
#endif

	if (op->sl > 0) {
	    const int	ncols = ncolstr(ntab,ncol,op->sp,op->sl) ;
#if	CF_DEBUGS
	    debugprintf("mailmsghdrfold_get: ncols=%u\n",ncols) ;
#endif
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

#if	CF_DEBUGS
	{
	    cchar	*lp = *rpp ;
	    debugprintf("mailmsghdrfold_get: ret str=>%t<\n",
	        lp,strlinelen(lp,ll,50)) ;
	    debugprintf("mailmsghdrfold_get: ret ll=%u\n",ll) ;
	}
#endif

	return ll ;
}
/* end subroutine (mailmsghdrfold_get) */


/* private subroutines */

static int findpieces(MF *op,int ncol,cchar **rpp) noex {
	const int	mcols = op->mcols ;
	int		sl = op->sl ;
	int		ll = 0 ;
	cchar	*sp = op->sp ;

#if	CF_DEBUGS
	debugprintf("findpieces: ent ncol=%u\n",ncol) ;
#endif

	if (sl > 0) {
	    int		nc = ncol ;
	    int		pl ;
	    int		ncs ;

	    while (sl && isskip(sp[0])) {
	        sp += 1 ;
	        sl -= 1 ;
	    }

	    *rpp = sp ;
	    while ((pl = nextpiece(nc,sp,sl,&ncs)) > 0) {

#if	CF_DEBUGS
	        debugprintf("findpieces: nc=%u ncs=%u\n",nc,ncs) ;
	        debugprintf("findpieces: pl=%d piece=>%t<\n",
	            pl,sp,strlinelen(sp,pl,40)) ;
#endif

	        if ((nc + ncs) > mcols) {
	            if (ll == 0) {
	                if (op->ln == 0) {
	                    if (rpp != NULL) *rpp = blank ;
	                    ll = 1 ;
	                }
	            }
	            break ;
	        }

	        ll += pl ;
	        nc += ncs ;
	        sp += pl ;
	        sl -= pl ;

	    } /* end while */

#if	CF_DEBUGS
	    {
	        cchar	*lp = *rpp ;
	        debugprintf("findpieces: str=>%t<\n",
	            lp,strlinelen(lp,ll,40)) ;
	    }
#endif

	    if (ll > 0) {
	        while (sl && isskip(sp[0])) {
	            sp += 1 ;
	            sl -= 1 ;
	        }
	        op->sp = sp ;
	        op->sl = sl ;
	    }

	} else {
	    *rpp = sp ;
	} /* end if (string length) */

#if	CF_DEBUGS
	{
	    cchar	*lp = *rpp ;
	    debugprintf("findpieces: str=>%t<\n",
	        lp,strlinelen(lp,ll,50)) ;
	    debugprintf("findpieces: ret ll=%u\n",ll) ;
	}
#endif

	return ll ;
}
/* end subroutine (findpieces) */

static int findbreaks(MF *op,int ncol,cchar **rpp) noex {
	int		i ;
	int		ll = 0 ;
	for (i = 0 ; (ll == 0) && breaks[i] ; i += 1) {
	    ll = findbreakers(op,breaks[i],ncol,rpp) ;
	} /* end for */
	return ll ;
}
/* end subroutine (findbreaks) */

static int findbreakers(MF *op,int bch,int ncol,cchar **rpp) noex {
	const int	mcols = op->mcols ;
	int		sl = op->sl ;
	int		ll = 0 ;
	cchar		*sp = op->sp ;

#if	CF_DEBUGS
	debugprintf("findbreakers: ent bch=»%c« ncol=%u\n",bch,ncol) ;
	debugprintf("findbreakers: mcols=%d sl=%d\n",mcols,sl) ;
#endif

	if (sl > 0) {
	    int		nc = ncol ;
	    int		pl ;
	    int		ncs ;

#if	CF_DEBUGS
	    debugprintf("findbreakers: going\n") ;
#endif
	    while (sl && isskip(sp[0])) {
	        sp += 1 ;
	        sl -= 1 ;
	    }

#if	CF_DEBUGS
	    debugprintf("findbreakers: while-before\n") ;
#endif

	    *rpp = sp ;
	    while ((pl = nextbreak(nc,bch,sp,sl,&ncs)) > 0) {

#if	CF_DEBUGS
	        debugprintf("findbreakers: nc=%u ncs=%u\n",nc,ncs) ;
	        debugprintf("findbreakers: pl=%d piece=>%t<\n",
	            pl,sp,strlinelen(sp,pl,40)) ;
#endif

	        if ((nc + ncs) > mcols) {
	            break ;
	        }

	        ll += pl ;
	        nc += ncs ;
	        sp += pl ;
	        sl -= pl ;

	    } /* end while */

#if	CF_DEBUGS
	    {
	        cchar	*lp = *rpp ;
	        debugprintf("findbreakers: line=>%t<\n",
	            lp,strlinelen(lp,ll,40)) ;
	    }
#endif

	    if (ll > 0) {
	        while (sl && isskip(sp[0])) {
	            sp += 1 ;
	            sl -= 1 ;
	        }
	        op->sp = sp ;
	        op->sl = sl ;
	    }

	} else {
	    *rpp = sp ;
	} /* end if (string length) */

#if	CF_DEBUGS
	{
	    cchar	*lp = *rpp ;
	    debugprintf("findbreakers: str=>%t<\n",
	        lp,strlinelen(lp,ll,50)) ;
	    debugprintf("findbreakers: ret ll=%u\n",ll) ;
	}
#endif

	return ll ;
}
/* end subroutine (findbreakers) */

static int findslices(MF *op,int ncol,cchar **rpp) noex {
	const int	ntab = NTABCOLS ;
	const int	mcols = op->mcols ;
	int		sl = op->sl ;
	int		ll = 0 ;
	cchar	*sp = op->sp ;

#if	CF_DEBUGS
	debugprintf("findslices: ent ncol=%u\n",ncol) ;
#endif

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

#if	CF_DEBUGS
	{
	    cchar	*lp = *rpp ;
	    debugprintf("findslices: str=>%t<\n",
	        lp,strlinelen(lp,ll,50)) ;
	    debugprintf("findslices: ret ll=%u\n",ll) ;
	}
#endif

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
	const int	ntab = NTABCOLS ;
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

#if	CF_DEBUGS
	debugprintf("mailmsghdrfold/nextpiece: ret pl=%u ncs=%u\n",pl,ncs) ;
#endif

	return pl ;
}
/* end subroutine (nextpiece) */

static int nextbreak(int ncol,int bch,cchar *sp,int sl,int *ncp) noex {
	const int	ntab = NTABCOLS ;
	int		ncs = 0 ;
	int		cl = sl ;
	int		n ;
	int		pl = 0 ;
	cchar	*tp ;
	cchar	*cp = sp ;

#if	CF_DEBUGS
	debugprintf("mailmsghdrfold/nextbreak: ent bch=»%c« sl=%d\n",
	    bch,sl) ;
#endif

	*ncp = 0 ;

/* skip over whitespace */

	while (cl && CHAR_ISWHITE(cp[0])) {
	    n = ncolchar(ntab,ncol,cp[0]) ;
#if	CF_DEBUGS
	    debugprintf("mailmsghdrfold/nextbreak: ncolchar() n=%d\n",n) ;
#endif
	    cp += 1 ;
	    cl -= 1 ;
	    ncs += n ;
	    ncol += n ;
	} /* end while */

/* find a possible break */

#if	CF_STRNBREAK
	if ((tp = strnbreak(cp,cl,bch)) != NULL) {
	    n = ncolstr(ntab,ncol,cp,((tp+1)-cp)) ;
	    ncs += n ;
	    pl = ((tp+1) - sp) ;
	    *ncp = ncs ;
	} /* end if */
#else
	if ((tp = strnchr(cp,cl,bch)) != NULL) {
	    n = ncolstr(ntab,ncol,cp,((tp+1)-cp)) ;
	    ncs += n ;
	    pl = ((tp+1) - sp) ;
	    *ncp = ncs ;
	} /* end if */
#endif /* CF_STRNBREAK */

	*ncp = ncs ;

/* done */

#if	CF_DEBUGS
	debugprintf("mailmsghdrfold/nextbreak: ret pl=%u ncs=%u\n",pl,ncs) ;
#endif

	return pl ;
}
/* end subroutine (nextbreak) */

static int isskip(int ch) noex {
	return (CHAR_ISWHITE(ch) || isend(ch)) ;
}
/* end subrouine (isskip) */

static int isend(int ch) noex {
	return ((ch == '\r') || (ch == '\n')) ;
}
/* end subroutine (isend) */

#if	CF_STRNBREAK
cchar *strnbreak(cchar *sp,int sl,int bch) noex {
	int		ch ;
	int		si ;
	int		f = FALSE ;
	bch &= UCHAR_MAX ;
	while (sl && *sp) {
	    ch = MKCHAR(sp[0]) ;
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
	return (f) ? sp : NULL ;
}
/* end subroutine (strnbreak) */
#endif /* CF_STRNBREAK */


