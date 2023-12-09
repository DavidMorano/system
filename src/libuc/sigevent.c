/* sigevent */
/* lang=C99 */

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
	int sigevent_init(SIGEVENT *sep,int notify,int signo,int val) noex

	Arguments:
	sep		pointer to SIGEVENT
	notify		"notify" element
	signo		signal number
	val		user defined value

	Returns:
	<0		error
	>=0		OK

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<signal.h>
#include	<time.h>
#include	<string.h>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int sigevent_load(SIGEVENT *sep,int notify,int signo,int val) noex {
	int		rs = SR_FAULT ;
	if (sep) {
	    memset(sep,0,sizeof(SIGEVENT)) ;
	    sep->sigev_notify = notify ;
	    sep->sigev_signo = signo ;
	    sep->sigev_value.sival_int = val ;
	}
	return rs ;
}
/* end subroutine (sigevent_init) */


