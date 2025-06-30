/* buffer_strquote SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* take a string and insert it into the buffer in quoted form */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	The subroutine was written from scratch but was based on
	code that was in the 'mkmsg' program.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	buffer_strquote

	Description:
	This method (for object BUFFER) takes the given string and
	inserts it into the "buffer" after it has been shell-quoted.

	Synopsis:
	int buffer_strquote(buffer *op,cchar *sp,int sl) noex

	Arguments:
	op		pointer to BUFFER object
	sp		pointer to string
	sl		length of string

	Returns
	>=0		length of string buffered
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<ascii.h>
#include	<strn.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"buffer.h"

import libutil ;

/* local defines */


/* external subroutines */

extern "C" {
    int	buffer_strquote(buffer *,cchar *,int) noex ;
}


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int buffer_strquote(buffer *op,cchar *sp,int sl) noex {
	cint		qch = CH_DQUOTE ;
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if (sl < 0) sl = xstrlen(sp) ;
	if (strnbrk(sp,sl," \t\r\n\v\f\b\"\\") != nullptr) {
	    cint	sz = ((2 * sl) + 3) ;
	    if (char *ap ; (rs = uc_malloc(sz,&ap)) >= 0) {
	        cchar	*tp ;
		char	*bp = ap ;
		{
		    *bp++ = qch ;
		    while ((tp = strnbrk(sp,sl,"\"\\")) != nullptr) {
			cint	tl = intconv(tp - sp) ;
		        bp = strwcpy(bp,sp,tl) ;
		        *bp++ = CH_BSLASH ;
		        *bp++ = *tp ;
		        sl -= intconv((tp + 1) - sp) ;
		        sp = (tp+1) ;
		    } /* end while */
		    if (sl > 0) {
		        bp = strwcpy(bp,sp,sl) ;
		    }
		    *bp++ = qch ;
		    {
		        cint	tl = intconv(bp - ap) ;
		        rs = buffer_strw(op,ap,tl) ;
		    }
		    len = rs ;
		} /* end block */
		rs1 = uc_free(ap) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} else {
	    rs = buffer_strw(op,sp,sl) ;
	    len = rs ;
	}
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (buffer_strquote) */


