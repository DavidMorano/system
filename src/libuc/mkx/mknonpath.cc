/* mknonpath SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make a file rooted from the present-working-directory */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mknonpath

	Description:
	This subroutine examines the source file or path given
	and returns either the original source-path as it is
	or an absolute-path version it, depending on whether the
	source as already absolute or not.

	Synopsis:
	int mknonpath(char *rbuf,cchar *sbuf) noex

	Arguments:
	rbuf		result buffer
	sbuf		source buffer

	Returns:
	>=0		length of resulting c-string
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
#include	<getpwd.h>
#include	<libmallocxx.h>
#include	<mkpath.h>
#include	<nonpath.h>
#include	<localmisc.h>

#include	"mkx.h"
#include	"mknonpath.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int mknonpath(char *rbuf,int rlen,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rl = 0 ;
	if (rbuf && sp) ylikely {
	    rs = SR_INVALID ;
	    if (sp[0]) ylikely {

	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end if (mknonpath) */


