/* mkintfname SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make an open-intercept filename from components */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-12-03, David A­D­ Morano
	This code was pulled out of (like several other procedures
	like this) some encompassing code (which itself needed
	to create "open-intercept" file-names).

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkintfname

	Description:
	This subroutine constructs an open-intercept filename (from
	its constituent parts).  An open-intercept filename looks
	like:
		[<dn>/]<prn>ô<inter>

	Synopsis:
	int mkintfname(char *rbuf,cchar *dn,cchar *prn,cchar *inter) noex

	Arguments:
	rbuf		result buffer
	dn		directory name
	prn		program-root name
	inter		intercept service

	Returns:
	>0		result string length
	==		?
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<bufsizevar.hh>
#include	<storebuf.h>
#include	<localmisc.h>

#include	"mkx.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static bufsizevar	maxpathlen(getbufsize_mp) ;


/* exported variables */


/* exported subroutines */

int mkintfname(char *rbuf,cchar *dn,cchar *prn,cchar *inter) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ;
	if (rbuf && prn && inter) ylikely {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (prn[0] && inter[0]) ylikely {
	        if ((rs = maxpathlen) >= 0) ylikely {
		    storebuf	sb(rbuf,rs) ;
	            if (dn && dn[0]) ylikely {
	                rs = sb.str(dn) ;
		    }
		    if (rs >= 0) ylikely {
	                if (int i = sb.idx ; (i > 0) && (rbuf[i - 1] != '/')) {
	                    rs = sb.chr('/') ;
	                }
	            } /* end if (had a directory) */
	            if (rs >= 0) rs = sb.str(prn) ;
	            if (rs >= 0) rs = sb.chr('º') ;
	            if (rs >= 0) rs = sb.str(inter) ;
	            rl = sb.idx ;
	        } /* end if (maxpathlen) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (mkintfname) */


