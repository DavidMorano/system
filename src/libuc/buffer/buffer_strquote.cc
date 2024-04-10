/* buffer_strquote SUPPORT */
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

	Description:
	This method (for object BUFFER) takes the given string and
	inserts it into the "buffer" after it has been shell-quoted.

	Synopsis:
	int buffer_strquote(buffer *bufp,cchar *sp,int sl) noex

	Arguments:
	bufp		pointer to BUFFER object
	sp		pointer to string
	sl		length of string

	Returns
	>=0		length of string buffered
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<ascii.h>
#include	<strn.h>
#include	<strwcpy.h>
#include	<buffer.h>
#include	<localmisc.h>


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

int buffer_strquote(buffer *bufp,cchar *sp,int sl) noex {
	cint		qch = CH_DQUOTE ;
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if (sl < 0) sl = strlen(sp) ;
	if (strnpbrk(sp,sl," \t\r\n\v\f\b\"\\") != NULL) {
	    cint	sz = ((2*sl)+3) ;
	    cchar	*tp ;
	    char	*ap ;
	    if ((rs = uc_malloc(sz,&ap)) >= 0) {
		char	*bp = ap ;
		{
		    *bp++ = qch ;
		    while ((tp = strnpbrk(sp,sl,"\"\\")) != NULL) {
		        bp = strwcpy(bp,sp,(tp-sp)) ;
		        *bp++ = CH_BSLASH ;
		        *bp++ = *tp ;
		        sl -= ((tp+1)-sp) ;
		        sp = (tp+1) ;
		    } /* end while */
		    if (sl > 0) {
		        bp = strwcpy(bp,sp,sl) ;
		    }
		    *bp++ = qch ;
		    rs = buffer_strw(bufp,ap,(bp-ap)) ;
		    len = rs ;
		} /* end block */
		rs1 = uc_free(ap) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (memory-allocation) */
	} else {
	    rs = buffer_strw(bufp,sp,sl) ;
	    len = rs ;
	}
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (buffer_strquote) */


