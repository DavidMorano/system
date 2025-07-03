/* itcontrol SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interval-timer control (block) */
/* version %I% last-modified %G% */


/* revision history:

	= 2014-04-04, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2014 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	itcontrol_load

	Description:
	This file provides the definition of the ITCONTROL object.
	This is the control-block for specifying how an interval-timer
	expiration is handled.

	Synopsis:
	typedef itcontrol	itc ;
	typedef itcontrol_f 	itc_f ;
	int itcontrol_load(itc *op,void *objp,itc_f *memf,int arg) noex

	Arguments:
	op		ITCONTROL object pointer
	objp		target object-pointer
	memf		call-back subroutine-function pointer
	arg		argument

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<usystem.h>
#include	<localmisc.h>

#include	"itcontrol.h"


/* local defines */


/* imported namespaces */


/* local typedefs */

typedef itcontrol	itc ;
typedef itcontrol_f	itc_f ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int itcontrol_load(itc *op,void *objp,psem *sp,itc_f f,int arg) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    op->objp = objp ;		/* object pointer */
	    op->psemp = sp ;		/* semæphore (POSIX®) pointer */
	    op->memf = f ;		/* itcontrol subroutine pointer */
	    op->arg = arg ;		/* optional argument */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (itcontrol_load) */


