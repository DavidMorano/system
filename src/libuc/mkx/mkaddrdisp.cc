/* mkaddrdisp SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* create (in a buffer) a sort of nice mail address for display purposes */
/* version %I% last-modified %G% */

#define	CF_NONSTANDARD	1		/* assume non-standard addressing */

/* revision history:

	= 2005-06-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2005 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkaddrdisp

	Description:
	This subroutine creates (in a buffer) a sort of nice mail
	address for display purposes.

	Synopsis:
	int mkaddrdisp(char *mbuf,int mlen,cchar *ap,int al) noex

	Arguments:
	mbuf		buffer to hold result
	mlen		length of given buffer
	ap		argument
	al		argument-length

	Returns:
	>=0		length of created result
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<field.h>
#include	<sbuf.h>
#include	<localmisc.h>

#include	"mkx.h"

import libutil ;

/* local defines */

#ifdef	CF_NONSTANDARD
#define	CF_NONSTANDARD	1		/* assume non-standard addressing */
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

cbool		f_nonstandard = CF_NONSTANDARD ;


/* exported variables */


/* exported subroutines */

int mkaddrdisp(char *abuf,int alen,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (abuf && sp) ylikely {
	    if (sl < 0) sl = lenstr(sp) ;
	    if (sbuf b ; (rs = b.start(abuf,alen)) >= 0) ylikely {
	        cint	flen = sl ;
	        if (char *fbuf ; (rs = uc_malloc((flen+1),&fbuf)) >= 0) {
	            if (field fsb ; (rs = fsb.start(sp,sl)) >= 0) ylikely {
			cnullptr	np{} ;
	                int		c = 0 ;
	                for (int fl ; (fl = fsb.sharg(np,fbuf,flen)) >= 0 ; ) {
	                    cchar	*fp = fbuf ;
	                    if (fl > 0) ylikely {
				if_constexpr (f_nonstandard) {
	                            if (c++ > 0) {
	                                rs = b.chr(' ') ;
			            }
			        } /* end if_constexpr (f_nonstandard) */
	                        if (rs >= 0) ylikely {
	                            rs = b.strw(fp,fl) ;
			        }
			    } /* end if (non-zero positive) */
	                    if (rs < 0) break ;
	                } /* end while */
	                rs1 = fsb.finish ;
			if (rs >= 0) rs = rs1 ;
	            } /* end if (field) */
	            rs1 = uc_free(fbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	        len = b.finish ;
	        if (rs >= 0) rs = len ;
	    } /* end if (sbuf) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mkaddrdisp) */


