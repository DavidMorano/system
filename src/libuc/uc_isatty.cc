/* uc_isatty SUPPORT */
/* lang=C++20 */

/* determine is the given file-descriptor references a terminal device */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-28, David A­D­ Morano
	How did we get along without this for over 10 years?

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine determines if a supplied file-descriptor
	is associated with a terminal or not.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<cerrno>
#include	<usystem.h>


/* local defines */


/* forward references */


/* exported variables */


/* exported subroutines */

int uc_isatty(int fd) noex {
	int		rs ;
	errno = 0 ;
	if ((rs = isatty(fd)) == 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end subroutine (uc_isatty) */

int uc_isastream(int fd) noex {
	int		rs ;
	errno = 0 ;
	if ((rs = isastream(fd)) == 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end subroutine (uc_isastream) */


