/* sbuf_addquoted */
/* lang=C20 */

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
	Add a shell-quoted string to the SBUF object.

	Synopsis:
	int sbuf_addquoted(sbuf *sbp,cchar *ap,int al) noex

	Arguments:
	sbp		pointer to the buffer object
	ap		string to add
	al		length of string to add

	Returns:
	>=0		amount of new space used by the newly stored item
			(not including any possible trailing NUL characters)
	<0		error

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<string.h>
#include	<usystem.h>
#include	<mkquoted.h>
#include	<localmisc.h>

#include	"sbuf.h"


/* local defines */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int sbuf_addquoted(SBUF *sbp,cchar *ap,int al) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (sbp && ap) {
	    if (al < 0) al = strlen(ap) ;
	    {
	        cint	qlen = ((al * 2) + 3) ;
	        char	*qbuf = nullptr ;
	        if ((rs = uc_libmalloc((qlen+1),&qbuf)) >= 0) {
	            if ((rs = mkquoted(qbuf,qlen,ap,al)) >= 0) {
	                len = rs ;
	                rs = sbuf_strw(sbp,qbuf,len) ;
	            }
	            rs1 = uc_libfree(qbuf) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (allocation) */
	    } /* end block */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (sbuf_addquoted) */


