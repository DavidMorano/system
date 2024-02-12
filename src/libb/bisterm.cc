/* bisterm SUPPORT */
/* lang=C++20 */

/* determine is the current I-O stream is a terminal */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable debug print-outs */

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
	<		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<string.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	"bfile.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int bisterm(bfile *fp) noex {
	return bcontrol(fp,BC_ISTERMINAL,0) ;
}
/* end subroutine (bisterm) */


