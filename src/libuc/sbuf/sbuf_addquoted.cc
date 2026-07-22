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
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<mkquoted.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"sbuf.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| */

/* local defines */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sbuf_addquoted(sbuf *sbp,cchar *sp,int µsl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (sbp && sp) ylikely {
	    if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) ylikely {
	        cint	qlen = ((sl * 2) + 3) ;
	        if (char *qbuf ; (rs = lm_mall((qlen+1),&qbuf)) >= 0) ylikely {
	            if ((rs = mkquoted(qbuf,qlen,sp,sl)) >= 0) ylikely {
	                len = rs ;
	                rs = sbuf_strw(sbp,qbuf,len) ;
	            }
	            rs1 = lm_free(qbuf) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (allocation) */
	    } /* end if (getlenstr) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (sbuf_addquoted) */


