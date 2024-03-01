/* buffer_strcompact SUPPORT */
/* lang=C++20 */

/* buffer up a compacted string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	The subroutine was written from scratch but based on previous
	versions of the 'mkmsg' program.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	buffer_strcompact

	Description:
        Store a source string (which is not compacted) into the "buffer"

	Synopsis:
	int buffer_strcompact(buffer *bufp,cchar *sp,int sl) noex

	Arguments:
	bufp		pointer to BUFFER object
	sp		pointer to string
	sl		length of string

	Returns
	>=0		length of string buffered
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<ascii.h>
#include	<sfx.h>
#include	<localmisc.h>

#include	"buffer.h"


/* local defines */


/* external subroutines */

extern "C" {
    int	buffer_strcompact(buffer *,cchar *,int) noex ;
}


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int buffer_strcompact(buffer *bufp,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		cl ;
	int		len = 0 ;
	int		c = 0 ;
	cchar		*cp{} ;
	if (sl < 0) sl = strlen(sp) ;
	while ((cl = sfnext(sp,sl,&cp)) > 0) {
	    if (c++ > 0) {
	        rs = buffer_char(bufp,CH_SP) ;
	        len += rs ;
	    }
	    if (rs >= 0) {
	        rs = buffer_strw(bufp,cp,cl) ;
	        len += rs ;
	    }
	    sl -= ((cp+cl)-sp) ;
	    sp = (cp+cl) ;
	    if (rs < 0) break ;
	} /* end while */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (buffer_strcompact) */


