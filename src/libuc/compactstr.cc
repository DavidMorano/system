/* compactstr SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* compact the whitespace out of a string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-10, David A.D. Morano
	This was written from scratch.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	compactstr

	Descrption:
	This subroutine compacts a string -- in place -- by removing
	redundant whitespace.  Also non-blank whitespaces are
	replaced only by blanks.  Also any leading whitespace is
	removed.

	Synopsis:
	int compactstr(char *rbuf,int rlen) noex

	Arguments:
	rbuf		destination c-string pointer
	rlen		designation c-string length

	Returns:
	>=0		resulting string length
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstring>		/* |strlen(3c)| */
#include	<estrings.h>
#include	<strwcpy.h>
#include	<hasx.h>
#include	<localmisc.h>

#include	"compactstr.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int compactstr(char *sbuf,int slen) noex {
	int		bl ;
	if (slen < 0) slen = strlen(sbuf) ;
	bl = slen ;
	if ((slen > 0) && hasdoublewhite(sbuf,slen)) {
	    int		sl = slen ;
	    int		cl ;
	    cchar	*sp = sbuf ;
	    cchar	*cp{} ;
	    char	tbuf[slen+1] ;
	    char	*bp ;
	    bl = 0 ;
	    tbuf[0] = '\0' ;
	    while ((cl = sfnext(sp,sl,&cp)) > 0) {
		if (bl > 0) {
		    tbuf[bl++] = ' ' ;
		}
		bp = (tbuf + bl) ;
		bl += (strwcpy(bp,cp,cl) - bp) ;
		sl -= ((cp + cl) - sp) ;
		sp = (cp + cl) ;
	    } /* end while */
	    if (bl > 0) {
		strwcpy(sbuf,tbuf,bl) ;
	    }
	} /* end if (hasdouble) */
	return bl ;
}
/* end subroutine (compactstr) */


