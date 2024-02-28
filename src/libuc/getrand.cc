/* getrand SUPPORT */
/* lang=C++20 */

/* get random data from the UNIX� kernel */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright � 2001 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getrand

	Synopsis:
	int getrand(char *rbuf,int rlen) noex

	Arguments:
	rbuf		result buffer
	rlen		length of supplied result buffer

	Returns:
	>0		returned number of bytes
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<usystem.h>
#include	<localmisc.h>

#include	"getrand.h"


/* local defines */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int getrand(void *rbuf,int rlen) noex {
	return uc_getrandom(rbuf,rlen,0) ;
}
/* end subroutine (getrand) */


