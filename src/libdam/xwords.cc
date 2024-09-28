/* xwords SUPPORT */
/* lang=C++20 */

/* extract extra words from a single given word */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	xwords

	Description:
	This object extracts extra possible words from a single
	given word.  The given word is always returned (as one
	extraction) but additional subwords may also be returned.

*******************************************************************************/

#include	<envstandards.h>	/* must be before others */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<vecobj.h>
#include	<six.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"xwords.h"


/* local defines */

#define	XWORDS_WI	struct xwords_wi


/* local namespaces */


/* local typedefs */

typedef xwords_word *	wordp ;


/* external subroutines */


/* external variables */


/* local structures */

struct xwords_wi {
	cchar		*wp ;
	int		wl ;
} ;

typedef xwords_wi *	wip ;


/* forward references */

static int	xwords_more(xwords *,cchar *,int,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int xwords_start(xwords *op,cchar *wbuf,int wlen) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (op && wbuf) {
	    rs = memclear(op) ; /* dangerous ! */
            /* always enter the whole word */
	    op->words[i].wp = wbuf ;
	    op->words[i].wl = wlen ;
	    i += 1 ;
            /* try for possessives */
	    if (wlen > 2) {
	        int	el = (wlen - 2) ;
	        bool	f = false ;
	        if (strncmp((wbuf + el),"'s",2) == 0) {
	            f = true ;
	        } else if (strncmp((wbuf + el),"s'",2) == 0) {
	            f = true ;
	            el += 1 ;
	        }
	        if (f) {
	            op->words[i].wp = wbuf ;
	            op->words[i].wl = el ;
	            i += 1 ;
	        }
	    } /* end if (long enough for extra words) */
	    op->nwords = i ;
	    if (int si ; (si = sichr(wbuf,wlen,'-')) >= 0) {
	        rs = xwords_more(op,wbuf,wlen,si) ;
	        i = rs ;
	        op->nwords = rs ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (xwords_start) */

int xwords_get(xwords *op,int i,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		wl = 0 ;
	if (op) {
	    rs = SR_INVALID ;
	    if (i >= 0) {
	        xwords_word	*w = op->words ;
	        if (op->xa != nullptr) w = op->xa ;
	        if ((i < op->nwords) && (w[i].wp != nullptr)) {
	            wl = w[i].wl ;
	        }
	        if (rpp) {
	            *rpp = (wl > 0) ? w[i].wp : nullptr ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? wl : rs ;
}
/* end subroutine (xwords_get) */

int xwords_finish(xwords *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->xa) {
	        rs1 = uc_free(op->xa) ;
	        if (rs >= 0) rs = rs1 ;
	        op->xa = nullptr ;
	    }
	    op->nwords = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (xwords_finish) */


/* private subroutines */

static int xwords_more(xwords *op,cchar *wbuf,int wlen,int si) noex {
	vecobj		wil ;
	cint		esize = sizeof(XWORDS_WI) ;
	int		rs ;
	int		rs1 ;
	int		n = 0 ;
	if ((rs = vecobj_start(&wil,esize,2,0)) >= 0) {
	    XWORDS_WI	wi ;
	    wi.wp = wbuf ;
	    wi.wl = si ;
	    if ((rs = vecobj_add(&wil,&wi)) >= 0) {
		int	wl = (wlen-(si+1)) ;
		cchar	*wp = (wbuf+(si+1)) ;
		while ((si = sichr(wp,wl,'-')) >= 0) {
		    if (si > 0) {
	    	        wi.wp = wp ;
	    	        wi.wl = si ;
	    	        rs = vecobj_add(&wil,&wi) ;
		    }
		    wl -= (si+1) ;
		    wp += (si+1) ;
		    if (rs < 0) break ;
		} /* end while */
		if ((rs >= 0) && (wl > 0)) {
	    	    wi.wp = wp ;
	    	    wi.wl = wl ;
	    	    rs = vecobj_add(&wil,&wi) ;
		}
		if (rs >= 0) {
		    int		i ;
		    int		j ;
		    void	*vp ;
		    n = op->nwords + vecobj_count(&wil) ;
		    if (n > XWORDS_MAX) {
		        cint	sz = (n * esize) ;
		        if ((rs = uc_malloc(sz,&vp)) >= 0) {
			    op->xa = wordp(vp) ;
			    for (j = 0 ; j < op->nwords ; j += 1) {
			        op->xa[j].wp = op->words[j].wp ;
			        op->xa[j].wl = op->words[j].wl ;
			    }
			    for (i = 0 ; vecobj_get(&wil,i,&vp) >= 0 ; i += 1) {
		    		XWORDS_WI	*ep = wip(vp) ;
			        op->xa[j].wp = ep->wp ;
			        op->xa[j].wl = ep->wl ;
			        j += 1 ;
			    } /* end for */
		        } /* end if (memory-allocation) */
		    } else {
			j = op->nwords ;
			for (i = 0 ; vecobj_get(&wil,i,&vp) >= 0 ; i += 1) {
		   	    XWORDS_WI	*ep = wip(vp) ;
			    op->words[j].wp = ep->wp ;
			    op->words[j].wl = ep->wl ;
			    j += 1 ;
			} /* end for */
		    } /* end if */
		} /* end if (ok) */
	    } /* end if (add first word) */
	    rs1 = vecobj_finish(&wil) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vecobj) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (xwords_more) */


