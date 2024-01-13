/* mkbasename SUPPORT */
/* lang=C20 */

/* make the filename for a shared library (shared object) */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-07-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine formulates (makes) the base-name part of a
	file-name into the result buffer.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<stdlib.h>
#include	<string.h>
#include	<usystem.h>
#include	<mkpathxw.h>
#include	<sfx.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */


/* exported subroutines */

int mkbasename(char *rbuf,cchar *pnp,int pnl) noex {
	int		rs = SR_FAULT ;
	if (rbuf && pnp) {
	    int		cl ;
	    cchar	*cp ;
	    rs = SR_INVALID ;
	    if ((cl = sfbasename(pnp,pnl,&cp)) > 0) {
	        rs = mkpath1w(rbuf,cp,(cp - pnp)) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mkbasename) */


