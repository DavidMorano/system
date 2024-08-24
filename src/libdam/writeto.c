/* writeto SUPPORT */
/* lang=C++20 */

/* perform timed write operation */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This object module was originally written to create a logging
	mechanism for PCS application programs.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	This subroutine performs a timed write operation (to an FD).
	It is very much like |u_write(3u)| but can take an optional
	time-out operand.

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int writeto(int wfd,cchar *wbuf,int wlen,int wto) noex {
	return uc-writeto(wfd,wbuf,wlen,wto) ;
}
/* end subroutine (writeto) */


