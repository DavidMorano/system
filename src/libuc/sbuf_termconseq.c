/* sbuf_termconseq SUPPORT */
/* lang=C20 */

/* extra method for the storage buffer (SBuf) object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-24, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sbuf_termconseq

	Description:
	This subroutine stores a user-specified number of blank
	characters into the buffer.

	Synopsis:
	int sbuf_termconseq(sbuf *sbp,int name,int a1,int a2) noex

	Arguments:
	op		pointer to the buffer object
	n		mame of contrl-sequence
	a1		argument-1
	a2		argument-2

	Returns:
	>=0		amount of new space used by the newly stored item
			(not including any possible trailing NUL characters)
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<stdarg.h>
#include	<usystem.h>
#include	<termconseq.h>
#include	<localmisc.h>

#include	"sbuf.h"


/* local defines */

#ifndef	TERMCONSEQLEN
#define	TERMCONSEQLEN	84		/* why this value? */
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int sbuf_termconseq(sbuf *sbp,int name,cchar *is,int na,...) noex {
	int		rs = SR_FAULT ;
	int		tl = 0 ;
	if (sbp) {
	    rs = SR_INVALID ;
	    if (name > 0) {
		va_list	ap ;
	        cint	tlen = TERMCONSEQLEN ;
	        char	tbuf[TERMCONSEQLEN + 1] ;
		va_begin(ap,na) ;
	        if ((rs = termconseqva(tbuf,tlen,name,is,na,ap)) >= 0) {
	            tl = rs ;
	            rs = sbuf_strw(sbp,tbuf,tl) ;
	        }
		va_end(ap) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? tl : rs ;
}
/* end subroutine (sbuf_termconseq) */


