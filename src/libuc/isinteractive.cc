/* isinteractive SUPPORT */
/* lang=C++20 */

/* test if we have a controlling terminal or not */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-07, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	isinteractive

	Description:
	This subroutine tests if the calling program has a controlling
	terminal attached. This means that the program is usually
	being run interactively.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<usystem.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"isfiledesc.h"


/* local defines */

#ifndef	TTYFNAME
#define	TTYFNAME	"/dev/tty"
#endif


/* external subroutines */


/* external variables */


/* exported variables */


/* exported subroutines */

int isinteractive() noex {
	int		rs ;
	int		f = false ;
	if ((rs = u_open(TTYFNAME,O_RDONLY,0666)) >= 0) {
	    f = true ;
	    u_close(rs) ;
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (isinteractive) */


