/* mkbasename SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* create a resuling c-string w/ the base-name of a given file-path */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A-D- Morano
	This subroutine was originally written for Rightcore Network
	Services (RNS).

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	mkbasename

	Description:
	This subroutine formulates (makes) the base-name part of a
	file-name into the result buffer.

	Synopsis:
	int mkbasename(char *rbuf,int rlen,cchar *pnp,int pnl) noex

	Arguments:
	rbuf		result buffer pointer
	rlen		result buffer length
	pnp		source path-name pointer
	pnl		source path-name length

	Returns:
	>=0		number of bytes put into result buffer
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
#include	<sfx.h>			/* |sfbasename(3uc)| */
#include	<snwcpy.h>		/* |snwcpy(3uc)| */
#include	<localmisc.h>

#include	"mkx.h"


/* local defines */


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int mkbasename(char *rbuf,int rlen,cchar *pnp,int pnl) noex {
	int		rs = SR_FAULT ;
	if (rbuf && pnp) ylikely {
	    cchar	*cp{} ;
	    rs = SR_INVALID ;
	    if (int cl ; (cl = sfbasename(pnp,pnl,&cp)) > 0) ylikely {
	        rs = snwcpy(rbuf,rlen,cp,cl) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mkbasename) */


