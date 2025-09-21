/* buffer_stropaque SUPPORT */
/* charset=ISO8859-1 */
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
	buffer_stropaque

	Description:
	Store a source string, which is not opaque (has white-space
	in it), into the "buffer" buf while removing any white-space
	from it.

	Synopsis:
	int buffer_stropaque(buffer *op,cchar *sp,int sl) noex

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
#include	<sfx.h>
#include	<localmisc.h>

#include	"buffer.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| */

/* local defines */


/* external subroutines */

extern "C" {
    int	buffer_stropaque(buffer *,cchar *,int) noex ;
}


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int buffer_stropaque(buffer *op,cchar *sp,int µsl) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ; /* return-value */
	if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) {
	    cchar	*cp{} ;
	    rs = SR_OK ;
	    for (int cl ; (cl = sfnext(sp,sl,&cp)) > 0 ; ) {
	        rs = buffer_strw(op,cp,cl) ;
	        len += rs ;
	        sl -= intconv((cp + cl) - sp) ;
	        sp = (cp+cl) ;
	        if (rs < 0) break ;
	    } /* end for */
	} /* end if (getlenstr) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (buffer_stropaque) */


