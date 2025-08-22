/* readn SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* read from a file into a buffer */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

/*******************************************************************************

  	Name:
	readn

	Description:
	This ruboutine reads data from a file into a specified buffer.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<usystem.h>
#include	<localmisc.h>

/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int readn(int fd,void *rbuf,int rlen) noex {
	return uc_readn(fd,rbuf,rlen) ;
} /* end subroutine (readn) */


