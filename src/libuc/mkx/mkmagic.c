/* mkmagic SUPPORT */
/* lang=C20 */

/* create a string with a (pretty much) standard magic string  */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-02-01, David A­D­ Morano
        This code was originally written.

	= 2017-07-17, David A­D­ Morano
	I added the code to return overflow if the given string cannot fit into
	the result buffer.

*/

/* Copyright © 2004,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkmagic

	Descrption:
	This subroutine creates a magic-string that goes at the front
	(usualy) of a file in order to identify it (its purpose).

	Synopsis:
	int mkmagic(char *rbuf,int rlen,cchar *ms) noex

	Arguments:
	rbuf		result buffer
	rlen		result buffer length
	ms		source string

	Returns:
	>=0		length of resulting string
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<string.h>		/* <- for |strlen(3c)| */
#include	<usystem.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"mkx.h"


/* external subroutines */


/* local subroutines */


/* exported subroutines */

int mkmagic(char *rbuf,int rlen,cchar *ms) noex {
	int		rs = SR_FAULT ;
	if (rbuf && ms) {
	    cint	mslen = strlen(ms) ;
	    rs = SR_OK ;
	    if ((mslen+1) <= rlen) {
	        char	*bp = strwcpy(rbuf,ms,-1) ;
	        *bp++ = '\n' ;
	        *bp++ = '\0' ;
	        if (((rbuf+rlen)-bp) > 0) {
	            memset(bp,0,(rbuf+rlen)-bp) ;
	        }
	    } else {
	        if (rlen > 0) rbuf[0] = '\0' ;
	        rs = SR_OVERFLOW ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? rlen : rs ;
}
/* end subroutine (mkmagic) */


