/* mkaddrname SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find the best address-name, if one exists */
/* version %I% last-modified %G% */

#define	CF_MASSAGE	0		/* allow for "massaging" */

/* revision history:

	= 1998-06-01, David A­D­ Morano
	This code piece was part of the |pcsmailcheck(3pcs)|
	subroutine and I pulled it out to make a subroutine that
	can be used in multiple places.

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
	<0		error code (system-return)

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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<estrings.h>
#include	<ema.h>
#include	<isoneof.h>
#include	<localmisc.h>

#include	"mkx.h"

import libutil ;

/* local defines */

#ifndef	CF_MASSAGE
#define	CF_MASSAGE	0		/* allow for massaging */
#endif


/* local namespace */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	mkaddrdisp(char *,int,cchar *,int) noex ;
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

static int	emaentry_addrname(ema_ent *,char *,int) noex ;

static bool	isBadAddr(int) noex ;


/* local variables */

static constexpr int	rsbadaddr[] = {
	SR_INVALID,
	SR_DOM,
	0
} ;

constexpr bool		f_massage = CF_MASSAGE ;


/* exported variables */


/* exported subroutines */

int mkaddrname(char *fbuf,int flen,cchar *sp,int sl) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (fbuf && sp) ylikely {
	    rs = SR_OK ;
	    if (sl < 0) sl = lenstr(sp) ;
	    fbuf[0] = '\0' ;
	    if (sl > 0)  ylikely {
	        if (ema a ; (rs = a.start) >= 0) ylikely {
	            if ((rs = a.parse(sp,sl)) >= 0) ylikely {
	        	ema_ent		*ep{} ;
			cauto		eg = ema_get ;
		        for (int i = 0 ; (rs1 = eg(&a,i,&ep)) >= 0 ; i += 1) {
		            rs = emaentry_addrname(ep,fbuf,flen) ;
	                    len = rs ;
			    if (rs != 0) break ;
		        } /* end for (ema_get) */
		        if ((rs >= 0) && (rs1 != rsn)) rs = rs1 ;
		    } else if (isBadAddr(rs)) {
		        rs = SR_OK ;
	            } /* end if (ema_parse) */
	            rs1 = a.finish ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (ema) */
	    } /* end if (non-zero source) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mkaddrname) */


/* local subroutines */

static int emaentry_addrname(ema_ent *ep,char *fbuf,int flen) noex {
	int		rs = SR_OK ;
	int		nl = 0 ;
	int		atype = -1 ;
	int		len = 0 ; /* return-value */
	cchar		*sp = nullptr ;
	if ((sp == nullptr) || (nl == 0)) {
	    if (ep->cp != nullptr) {
	        atype = atype_comment ;
	        nl = sfshrink(ep->cp,ep->cl,&sp) ;
	    }
	}
	if ((sp == nullptr) || (nl == 0)) {
	    if (ep->ap != nullptr) {
	        atype = atype_address ;
	        nl = sfshrink(ep->ap,ep->al,&sp) ;
	    }
	}
	if ((sp == nullptr) || (nl == 0)) {
	    if (ep->rp != nullptr) {
	        atype = atype_route ;
	        nl = sfshrink(ep->rp,ep->rl,&sp) ;
	    }
	}
	if_constexpr (f_massage) {
	    if ((sp != nullptr) && (nl > 0)) {
	        int	cl ;
	        cchar	*cp ;
	        switch (atype) {
	        case atype_comment:
	            if ((cl = sfsubstance(sp,nl,&cp)) > 0) {
	                rs = snwcpy(fbuf,flen,cp,cl) ;
	                len = rs ;
	            }
	            break ;
	        case atype_address:
	            rs = mkaddrdisp(fbuf,flen,sp,nl) ;
	            len = rs ;
	            break ;
	        case atype_route:
	            rs = snwcpy(fbuf,flen,sp,nl) ;
	            len = rs ;
		    break ;
	        } /* end switch */
	    } /* end if (positive) */
	} else {
	    if ((sp != nullptr) && (nl > 0)) {
	        switch (atype) {
	        case atype_comment:
	        case atype_address:
	        case atype_route:
	            rs = snwcpy(fbuf,flen,sp,nl) ;
	            len = rs ;
		    break ;
	        } /* end switch */
	    } /* end if (positive) */
	} /* end if_constexpr (f_massage) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (emaentry_addrname) */

static bool isBadAddr(int rs) noex {
	return isOneOf(rsbadaddr,rs) ;
}
/* end subroutine (emaentry_addrname) */


