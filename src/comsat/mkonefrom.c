/* mkonefrom SUPPORT */
/* lang=C++20 */

/* try to divine the best "from" address from a raw source string */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debug print-outs */
#define	CF_MASSAGE	0		/* allow for massaging */

/* revision history:

	= 1998-06-01, David A­D­ Morano
	This code piece was part of the 'pcsmailcheck(3pcs)'
	subroutine and I pulled it out to make a subroutine that
	can be used in multiple places.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkonefrom

	Description:
	This subroutine extracts the "best" address out of an
	EMA-type of address specification (given in raw string
	form).

	Synopsis:
	int mkonefrom(char *fbuf,int flen,cchar *sp,int sl) noex

	Arguments:
	fbuf		result buffer
	flen		result buffer length
	sp		source string
	sl		source string length

	Returns:
	>=0		length of resulting string
	<0		error (system-return)

	Notes:
	+ Massaging the result:
	In the old days, before header fields could be encoded in
	wacko ways, the result here was the final result. It could
	therefore be massaged to get rid of some cruft that certain
	mailers (who will remain nameless -- for now) would add to
	the field string value. But now-a-days, the result here
	could still be wackily encoded, so massaging will at best
	do nothing, and at worst break the encoded format.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<string.h>
#include	<usystem.h>
#include	<estrings.h>
#include	<ema.h>
#include	<localmisc.h>

#include	"mkx.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

enum atypes {
	atype_comment,
	atype_address,
	atype_route,
	atype_overlast
} ;


/* forward references */

static int	emaentry_bestfrom(EMA_ENT *,char *,int) noex ;


/* local variables */


/* exported subroutines */

int mkonefrom(char *fbuf,int flen,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		len = 0 ;

	if (fbuf == NULL) return SR_FAULT ;
	if (sp == NULL) return SR_FAULT ;

	if (sl < 0) sl = strlen(sp) ;

	fbuf[0] = '\0' ;
	if (sl > 0)  {
	    EMA		a ;
	    EMA_ENT	*ep ;
	    if ((rs = ema_start(&a)) >= 0) {
	        if (ema_parse(&a,sp,sl) >= 0) {
		    if (ema_get(&a,0,&ep) >= 0) {
		        rs = emaentry_bestfrom(ep,fbuf,flen) ;
	                len = rs ;
		    } /* end if (ema_get) */
	        } /* end if (ema_parse) */
	        rs1 = ema_finish(&a) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (ema) */
	} /* end if (non-zero source) */

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mkonefrom) */


/* local subroutines */

static int emaentry_bestfrom(EMA_ENT *ep,char *fbuf,int flen) noex {
	int		rs = SR_OK ;
	int		nl = 0 ;
	int		atype = -1 ;
	int		len = 0 ;
	cchar		*np = NULL ;

	if ((np == NULL) || (nl == 0)) {
	    if (ep->cp != NULL) {
	        atype = atype_comment ;
	        nl = sfshrink(ep->cp,ep->cl,&np) ;
	    }
	}

	if ((np == NULL) || (nl == 0)) {
	    if (ep->ap != NULL) {
	        atype = atype_address ;
	        nl = sfshrink(ep->ap,ep->al,&np) ;
	    }
	}

	if ((np == NULL) || (nl == 0)) {
	    if (ep->rp != NULL) {
	        atype = atype_route ;
	        nl = sfshrink(ep->rp,ep->rl,&np) ;
	    }
	}

#if	CF_MASSAGE
	if ((np != NULL) && (nl > 0)) {
	    int		cl ;
	    const char	*cp ;
	    switch (atype) {
	    case atype_comment:
	        if ((cl = sfsubstance(np,nl,&cp)) > 0) {
	            rs = snwcpy(fbuf,flen,cp,cl) ;
	            len = rs ;
	        }
	        break ;
	    case atype_address:
	        rs = mkdisphdr(fbuf,flen,np,nl) ;
	        len = rs ;
	        break ;
	    case atype_route:
	        rs = snwcpy(fbuf,flen,np,nl) ;
	        len = rs ;
		break ;
	    } /* end switch */
	} /* end if (positive) */
#else /* CF_MASSAGE */
	if ((np != NULL) && (nl > 0)) {
	    switch (atype) {
	    case atype_comment:
	    case atype_address:
	    case atype_route:
	        rs = snwcpy(fbuf,flen,np,nl) ;
	        len = rs ;
		break ;
	    } /* end switch */
	} /* end if (positive) */
#endif /* CF_MASSAGE */

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (emaentry_bestfrom) */


