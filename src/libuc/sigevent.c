/* sigevent SUPPORT */
/* lang=C++20 */

/* UNIX® signal-event initialization */
/* version %I% last-modified %G% */


/* revision history:

	= 2014-04-04, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2014 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sigevent_load

	Description:
	These subroutines manipulate SIGEVENT objects.

	Synopsis:
	int sigevent_load(SIGEVENT *sep,int notify,int signo,int val) noex

	Arguments:
	sep		pointer to SIGEVENT
	notify		"notify" element
	signo		signal number
	val		user defined value

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<csignal>
#include	<cstring>		/* <- for |memset(3c)| */
#include	<ctime>
#include	<usystem.h>

#include	"sigevent.h"


/* local defines */


/* imported namespaces */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sigevent_load(SIGEVENT *sep,int notify,int signo,int val) noex {
	int		rs = SR_FAULT ;
	if (sep) {
	    rs = memclear(sep) ;
	    sep->sigev_notify = notify ;
	    sep->sigev_signo = signo ;
	    sep->sigev_value.sival_int = val ;
	}
	return rs ;
}
/* end subroutine (sigevent_load) */


