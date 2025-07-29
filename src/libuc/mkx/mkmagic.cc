/* mkmagic SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* create a c-string that constitutes a "magic" string for file-ID purposes */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-02-01, David A­D­ Morano
        This code was originally written.

	= 2017-07-17, David A­D­ Morano
	I added code to return overflow if the given string
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
	int mkmagic(char *rbuf,int rsz,cchar *ms,int ml = -1) noex

	Arguments:
	rbuf		result buffer pointer
	rsz		result buffer size (not NUL-terminator)
	ms		source c-string

	Returns:
	>=0		length of resulting string
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"mkmagic.h"

import libutil ;

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int mkmagic(char *rbuf,int rsz,cchar *ms,int ml) noex {
	int		rs = SR_FAULT ;
	if (rbuf && ms) ylikely {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if ((rsz >= 2) && ms[0]) ylikely {
	        if (cint mslen = lenstr(ms,ml) ; (mslen+1) <= rsz) ylikely {
	            char	*bp = strwcpy(rbuf,ms,ml) ;
	            rs = SR_OK ;
	            *bp++ = '\n' ;
	            if (cint zl = intconv((rbuf + rsz) - bp) ; zl > 0) {
	                memclear(bp,zl) ;
		    }
	        } else {
	            rs = SR_OVERFLOW ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rsz : rs ;
}
/* end subroutine (mkmagic) */


