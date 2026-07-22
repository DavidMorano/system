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
	int sbuf_hexp(sbuf *op,{ux} v,int n) noex

	Arguments:
	op		pointer to the buffer object
	v		value to convert
	n		number of bytes to render

	Returns:
	>=0		amount of new space used by the newly stored item
			(not including any possible trailing NUL characters)
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<concepts>		/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<cthex.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"sbuf.h"


/* local defines */


/* imported namespaces */

using std::integral_signed ;		/* concept */
using std::integral_unsigned ;		/* concept */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<integral_unsigned UT>
int sbuf_hexpx(sbuf *op,int n,UT v) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    cint	dlen = HEXBUFLEN ;
	    char	dbuf[HEXBUFLEN+1] ;
	    if ((rs = cthex(dbuf,dlen,v)) >= 0) ylikely {
	        cchar	*dp = dbuf ;
	        if (n == 6) {
		    dp += ((8 - n) * 2) ;
		}
	        rs = op->strw(dp,(n * 2)) ;
	    } /* end if (cthex) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (sbuf_hexpx) */


/* local variables */


/* exported variables */


/* exported subroutines */

int sbuf_hexpuc		(sbuf *op,int n,uchar v) noex {
    	return sbuf_hexpx(op,n,v) ;
}

int sbuf_hexpus		(sbuf *op,int n,ushort v) noex {
    	return sbuf_hexpx(op,n,v) ;
}

int sbuf_hexpui		(sbuf *op,int n,uint v) noex {
    	return sbuf_hexpx(op,n,v) ;
}

int sbuf_hexpul		(sbuf *op,int n,ulong v) noex {
    	return sbuf_hexpx(op,n,v) ;
}

int sbuf_hexpull	(sbuf *op,int n,ulonglong v) noex {
    	return sbuf_hexpx(op,n,v) ;
}


/* local subroutines */


