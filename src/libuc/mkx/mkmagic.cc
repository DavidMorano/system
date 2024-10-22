/* mkmagic SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* create a c-string that constitutes a "magic" string for file-ID purposes */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-02-01, David A­D­ Morano
        This code was originally written.

	= 2017-07-17, David A­D­ Morano
	I added the code to return overflow if the given string
	cannot fit into the result buffer.

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
#include	<cstring>		/* <- |strlen(3c)| + |memset(3c)| */
#include	<usystem.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"mkx.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int mkmagic(char *rbuf,int rlen,cchar *ms) noex {
	int		rs = SR_FAULT ;
	if (rbuf && ms) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if ((rlen > 2) && ms[0]) {
	        if (cint mslen = strlen(ms) ; (mslen+1) <= rlen) {
	            char	*bp = strwcpy(rbuf,ms,-1) ;
	            rs = SR_OK ;
	            *bp++ = '\n' ;
	            *bp++ = '\0' ;
	            if (cint zl = ((rbuf+rlen)-bp) ; zl > 0) {
	                memclear(bp,zl) ;
	            }
	        } else {
	            rs = SR_OVERFLOW ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rlen : rs ;
}
/* end subroutine (mkmagic) */


