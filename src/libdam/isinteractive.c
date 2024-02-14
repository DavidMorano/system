/* isinteractive */

/* test if we have a controlling terminal or not */


#define	CF_DEBUGS	0		/* compile-time debugging */


/* revision history:

	= 1998-03-07, David A�D� Morano
	This subroutine was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

        This subroutine tests if the calling program has a controlling terminal
        attached. This means that the program is usually being run
        interactively.


*******************************************************************************/


#include	<envstandards.h>

#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/socket.h>
#include	<unistd.h>
#include	<fcntl.h>

#include	<usystem.h>
#include	<localmisc.h>


/* local defines */

#ifndef	TTYFNAME
#define	TTYFNAME	"/dev/tty"
#endif


/* external subroutines */

extern int	isNotPresent(int) ;


/* external variables */


/* exported subroutines */


int isinteractive()
{
	int		rs ;
	int		f = FALSE ;
	if ((rs = u_open(TTYFNAME,O_RDONLY,0666)) >= 0) {
	    f = TRUE ;
	    u_close(rs) ;
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (isinteractive) */


