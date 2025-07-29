/* sbuf_hexp SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* storage buffer (SBuf) object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-24, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sbuf_hexp

	Description:
	Add a shell-quoted string to the sbuf object.

	Synopsis:
	int sbuf_hexp(sbuf *,uint64_t,int) noex

	Arguments:
	sop		pointer to the buffer object
	ap		string to add
	al		length of string to add

	Returns:
	>=0		amount of new space used by the newly stored item
			(not including any possible trailing NUL characters)
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstdint>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<mkquoted.h>
#include	<cthex.h>
#include	<localmisc.h>

#include	"sbuf.h"


/* local defines */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sbuf_hexp(sbuf *op,uint64_t v,int n) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    uint	vi ;
	    cint	dlen = DIGBUFLEN ;
	    char	dbuf[DIGBUFLEN+1] ;
	    switch (n) {
	    case 2:
	        vi = uint(v) ;
	        rs = cthexus(dbuf,dlen,vi) ;
	        break ;
	    case 4:
	        vi = uint(v) ;
	        rs = cthexui(dbuf,dlen,vi) ;
	        break ;
	    case 6:
	        rs = cthexull(dbuf,dlen,v) ;
	        break ;
	    default:
	        rs = SR_INVALID ;
	        break ;
	    } /* end switch */
	    if (rs >= 0) {
	        cchar	*dp = dbuf ;
	        if (n == 6) dp += ((8-n)*2) ;
	        rs = op->strw(dp,(n*2)) ;
	    } /* end if */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sbuf_hexp) */


