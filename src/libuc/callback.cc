/* callback SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* surboutine call-back interface */
/* version %I% last-modified %G% */


/* revision history:

	= 2014-04-04, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2014 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	callback_load

	Description:
	This subroutine loads (initializes) a TIMEOUT object.

	Synopsis:
	int callback_load(callback *op,void *objp,callback_f *memf,int arg) noex

	Arguments:
	op		object pointer
	objp		target object-pointer
	memf		member-function pointer
	arg		argument

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<usystem.h>
#include	<localmisc.h>

#include	"callback.h"


/* local defines */


/* imported namespaces */


/* local typedefs */

typedef callback_f	t_f ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int callback_load(callback *op,void *objp,t_f m,int arg) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    op->objp = objp ;
	    op->memf = m ;
	    op->arg = arg ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (callback_load) */


