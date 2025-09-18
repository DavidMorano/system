/* sbuf_addquoted SUPPORT */
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
	sbuf_addquoted

	Description:
	Add a shell-quoted string to the sbuf object.

	Synopsis:
	int sbuf_addquoted(sbuf *sbp,cchar *ap,int al) noex

	Arguments:
	sbp		pointer to the buffer object
	ap		string to add
	al		length of string to add

	Returns:
	>=0		amount of new space used by the newly stored item
			(not including any possible trailing NUL characters)
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<mkquoted.h>
#include	<localmisc.h>

#include	"sbuf.h"

import libutil ;

/* local defines */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */

constexpr cauto		mall = lm_mall ;
constexpr cauto		mfre = lm_free ;


/* exported variables */


/* exported subroutines */

int sbuf_addquoted(sbuf *sbp,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (sbp && sp) ylikely {
	    if (sl < 0) sl = lenstr(sp) ;
	    {
	        cint	qlen = ((sl * 2) + 3) ;
	        if (char *qbuf ; (rs = mall((qlen+1),&qbuf)) >= 0) {
	            if ((rs = mkquoted(qbuf,qlen,sp,sl)) >= 0) {
	                len = rs ;
	                rs = sbuf_strw(sbp,qbuf,len) ;
	            }
	            rs1 = mfre(qbuf) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (allocation) */
	    } /* end block */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (sbuf_addquoted) */


