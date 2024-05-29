/* ucsysauxinfo SUPPORT */
/* lang=C++20 */

/* UNIX® system-auxillary-information */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-02-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ucsysauxinfo

	Description:
	We (try to) retrieve auxillary informaton from the operating
	system.

	Synopsis:
	int uc_sysauxinfo(char *rbuf,int rlen,int req) noex

	Arguments:
	rbuf		result buffer pointer
	rlen		result buffer length
	req		system-auxillary-value to request

	Returns:
	-		length of returned string

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<usystem.h>

#include	<usysauxinfo.h>		/* <- from |libu| */


/* local defines */


/* imported namespaces */

using usys::usysauxinfo ;		/* subroutines */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_sysauxinfo(char *rbuf,int rlen,int req) noex {
	return usysauxinfo(rbuf,rlen,req) ;
}


/* local subroutines */


