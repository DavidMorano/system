/* bisterm SUPPORT */
/* lang=C++20 */

/* determine is the current I-O stream is a terminal */
/* version %I% last-modified %G% */

/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Determine is the current I-O stream refers to a terminal.

	Synopsis:
	int bisterm(bfile *fp)

	Arguments:
	fp		pointer to BFILE object

	Returns:
	==0		NO (not a terminal)
	>0		YES (a terminal)
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<usystem.h>

#include	"bfile.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int bisterm(bfile *fp) noex {
	return bcontrol(fp,BC_ISTERMINAL,0) ;
}
/* end subroutine (bisterm) */


