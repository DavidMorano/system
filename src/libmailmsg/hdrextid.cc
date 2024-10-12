/* hdrextid SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* header-extract-id */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	hdrextid

	Description:
	Here we extract an ID from a header value.  The ID is only
	looked for in a "route-address" part of an address
	specification.

	Synopsis:
	int hdrextid(char *rbuf,int rlen,cchar *vp,int vl) noex

	Arguments:
	rbuf		supplied result buffer
	rlen		length of supplied result buffer
	vp		value pointer
	vl		value length

	Returns:
	>=0		length of result
	<0		error (system-return)

	Notes: 
	IDs are derived from the "route-address" in an
	email-addresses (EMAs).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<ema.h>
#include	<sfx.h>
#include	<snwcpy.h>
#include	<localmisc.h>

#include	"hdrextid.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int hdrextid(char *rbuf,int rlen,cchar *abuf,int alen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (rbuf && abuf) {
	    rbuf[0] = '\0' ;
	    if (ema a ; (rs = ema_start(&a)) >= 0) {
	        if ((rs = ema_parse(&a,abuf,alen)) >= 0) {
	            ema_ent	*ep{} ;
	            for (int i = 0 ; ema_get(&a,i,&ep) >= 0 ; i += 1) {
	                if (ep) {
	                    int		sl = 0 ;
	                    cchar	*sp = nullptr ;
	                    if ((ep->rp != nullptr) && (ep->rl > 0)) {
			        sp = ep->rp ;
			        sl = ep->rl ;
			    }
	                    if (sl > 0) {
			        int	al ;
			        cchar	*ap ;
			        if ((al = sfshrink(sp,sl,&ap)) > 0) {
	                            rs = snwcpy(rbuf,rlen,ap,al) ;
	                            len = rs ;
			        }
	                    }
	                } /* end if (non-null) */
		        if (len > 0) break ;
		        if (rs < 0) break ;
	            } /* end for */
	        } /* end if (parse) */
	        rs1 = ema_finish(&a) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ema) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (hdrextid) */


