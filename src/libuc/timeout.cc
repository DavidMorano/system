/* timeout SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® signal event initialization */
/* version %I% last-modified %G% */


/* revision history:

	= 2014-04-04, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2014 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	timeout_load

	Description:
	This subroutine loads (initializes) a TIMEOUT object.

	Synopsis:
	int timeout_load(TIMEOUT *top,time_t sec,long nsec)

	Arguments:
	top		pointer to TIMEOUT
	sec		seconds
	nsec		nanoseconds

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<ctime>			/* |time_t| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<localmisc.h>

#include	"timeout.h"


/* local defines */


/* imported namespaces */


/* local typedefs */

typedef timeout_f	t_f ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int timeout_load(timeout *top,time_t v,void *o,t_f m,uint tag,int arg) noex {
	int		rs = SR_FAULT ;
	if (top) {
	    rs = SR_OK ;
	    top->id = -1 ;
	    top->val = v ;
	    top->objp = o ;
	    top->metf = m ;
	    top->tag = tag ;
	    top->arg = arg ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (timeout_load) */


