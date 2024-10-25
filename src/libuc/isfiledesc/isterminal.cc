/* isterminal SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* test if we have a terminal or not */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-07, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	isterminal

	Description:
	This subroutine tests if the calling program has a terminal
	associated with the given file-descriptor (FD).

	Synopsis:
	bool isterminal(int fd) noex

	Arguments:
	fd		file-descriptor to test for being a terminal

	Returns:
	FALSE		the file-descriptor is not a terminal
	TRUE		yrs, the file-descriptor is a terminal

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"isfiledesc.h"


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

int isterminal(int fd) noex {
    	int		rs = SR_BADFD ;
    	bool		f = false ;
	if (fd >= 0) {
	    if ((rs = isatty(fd)) > 0) {
	        f = true ;
	    } else if (rs < 0) {
		rs = (- errno) ;
	    }
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (isterminal) */


