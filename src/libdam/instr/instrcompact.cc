/* instrcompact SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* compact the whitespace out of a string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-10, David A-D- Morano
	This was written from scratch.

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	instrcompact

	Descrption:
	This subroutine compacts a string -- in place -- by removing
	redundant whitespace.  Also non-blank whitespaces are
	replaced only by blanks.  Also any leading whitespace is
	removed.

	Synopsis:
	int instrcompact(char *rbuf,int rlen) noex

	Arguments:
	rbuf		destination c-string pointer
	rlen		designation c-string length

	Returns:
	>=0		resulting string length
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<estrings.h>
#include	<strwcpy.h>
#include	<hasx.h>
#include	<localmisc.h>

#include	"instrcompact.h"

#pragma		GCC dependency	"mod/libutil.ccm"

import libutil ;

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int instrcompact(char *sbuf,int slen) noex {
    	int		rs = SR_FAULT ;
	int		bl = 0 ;
	if (sbuf) ylikely {
	    rs = SR_OK ;
	    if (slen < 0) slen = lenstr(sbuf) ;
	    bl = slen ;
	    if ((slen > 0) && hasdoublewhite(sbuf,slen)) ylikely {
	        int	sl = slen ;
	        cchar	*sp = sbuf ;
	        cchar	*cp{} ;
	        char	tbuf[slen+1] ;
	        char	*bp ;
	        bl = 0 ;
	        tbuf[0] = '\0' ;
	        for (int cl ; (cl = sfnext(sp,sl,&cp)) > 0 ; ) {
		    if (bl > 0) {
		        tbuf[bl++] = ' ' ;
		    }
		    bp = (tbuf + bl) ;
		    bl += intconv(strwcpy(bp,cp,cl) - bp) ;
		    sl -= intconv((cp + cl) - sp) ;
		    sp = (cp + cl) ;
	        } /* end while */
	        if (bl > 0) ylikely {
		    strwcpy(sbuf,tbuf,bl) ;
	        }
	    } /* end if (hasdouble) */
	} /* end if (non-null) */
	return (rs >= 0) ? bl : rs ;
}
/* end subroutine (instrcompact) */


