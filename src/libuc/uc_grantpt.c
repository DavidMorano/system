/* uc_grantpt */

/* interface component for UNIX�® library-3c */
/* grant a PTS-type pseudo-terminal the required permissions for use */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was adapted from a previously related subroutine.

	= 2018-10-03, David A.D. Morano
	Small pretty-up.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

        Grant the use of a pseudo-terminal on System V Release 3 (SVR3) UNIX�® OS
        systems.


*******************************************************************************/


#include	<envstandards.h>	/* MUST be first to configure */

#include	<sys/types.h>
#include	<stdlib.h>
#include	<errno.h>

#include	<usystem.h>


/* local defines */


/* forward references */


/* exported subroutines */


int uc_grantpt(int fd)
{
	int		rs ;

	errno = 0 ;
	if ((rs = grantpt(fd)) < 0) rs = (- errno) ;

	return rs ;
}
/* end subroutine (uc_grantpt) */

