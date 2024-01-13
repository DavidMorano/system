/* mkaddrname SUPPORT */
/* lang=C++20 */

/* find the best address-name, if one exists */
/* version %I% last-modified %G% */

#define	CF_MASSAGE	0		/* allow for "massaging" */

/* revision history:

	= 1998-06-01, David A­D­ Morano
	This code piece was part of the 'pcsmailcheck(3pcs)' subroutine and I
	pulled it out to make a subroutine that can be used in multiple
	places.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkaddrname

	Description:
	This subroutine extracts the "best" address out of an
	EMA-type of address specification (given in raw string
	form).

	Synopsis:
	int mkaddrname(char *fbuf,int flen,cchar *sp,int sl) noex

	Arguments:
	fbuf		result buffer
	flen		result buffer length
	sp		source string
	sl		source string length

	Returns:
	>=0		length of resulting string
	<0		error (system-return)

	Notes:
	1. Massaging the result:
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
#include	<cstring>		/* for |strlen(3c)| */
#include	<usystem.h>
#include	<estrings.h>
#include	<ema.h>
#include	<isoneof.h>
#include	<localmisc.h>

#include	"mkx.h"


/* local defines */


/* local namespace */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	mkdisphdr(char *,int,cchar *,int) noex ;
}


/* external variables */


/* local structures */

enum atypes {
	atype_comment,
	atype_address,
	atype_route,
	atype_overlast
} ;


/* forward references */

static int	emaentry_addrname(EMA_ENT *,char *,int) noex ;
static int	isBadAddr(int) noex ;


/* local variables */

static constexpr int	rsbadaddr[] = {
	SR_INVALID,
	SR_DOM,
	0
} ;

constexpr bool	f_massage = CF_MASSAGE ;


/* exported variables */


/* exported subroutines */

int mkaddrname(char *fbuf,int flen,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (fbuf && sp) {
	    rs = SR_OK ;
	    if (sl < 0) sl = strlen(sp) ;
	    fbuf[0] = '\0' ;
	    if (sl > 0)  {
	        EMA		a ;
	        EMA_ENT		*ep{} ;
	        if ((rs = ema_start(&a)) >= 0) {
	            if ((rs = ema_parse(&a,sp,sl)) >= 0) {
			auto	eg = ema_get ;
		        cint	rsn = SR_NOTFOUND ;
		        for (int i = 0 ; (rs1 = eg(&a,i,&ep)) >= 0 ; i += 1) {
		            rs = emaentry_addrname(ep,fbuf,flen) ;
	                    len = rs ;
			    if (rs != 0) break ;
		        } /* end for (ema_get) */
		        if ((rs >= 0) && (rs1 != rsn)) rs = rs1 ;
		    } else if (isBadAddr(rs)) {
		        rs = SR_OK ;
	            } /* end if (ema_parse) */
	            rs1 = ema_finish(&a) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (ema) */
	    } /* end if (non-zero source) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mkaddrname) */


/* local subroutines */

static int emaentry_addrname(EMA_ENT *ep,char *fbuf,int flen) noex {
	int		rs = SR_OK ;
	int		nl = 0 ;
	int		atype = -1 ;
	int		len = 0 ;
	cchar		*np = nullptr ;
	if ((np == nullptr) || (nl == 0)) {
	    if (ep->cp != nullptr) {
	        atype = atype_comment ;
	        nl = sfshrink(ep->cp,ep->cl,&np) ;
	    }
	}
	if ((np == nullptr) || (nl == 0)) {
	    if (ep->ap != nullptr) {
	        atype = atype_address ;
	        nl = sfshrink(ep->ap,ep->al,&np) ;
	    }
	}
	if ((np == nullptr) || (nl == 0)) {
	    if (ep->rp != nullptr) {
	        atype = atype_route ;
	        nl = sfshrink(ep->rp,ep->rl,&np) ;
	    }
	}
	if constexpr (f_massage) {
	    if ((np != nullptr) && (nl > 0)) {
	        int		cl ;
	        cchar	*cp ;
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
	} else {
	    if ((np != nullptr) && (nl > 0)) {
	        switch (atype) {
	        case atype_comment:
	        case atype_address:
	        case atype_route:
	            rs = snwcpy(fbuf,flen,np,nl) ;
	            len = rs ;
		    break ;
	        } /* end switch */
	    } /* end if (positive) */
	} /* end if-constexpr (f_massage) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (emaentry_addrname) */

static int isBadAddr(int rs) noex {
	return isOneOf(rsbadaddr,rs) ;
}
/* end subroutine (emaentry_addrname) */


