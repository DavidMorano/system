/* tcgetws */

/* UNIX® terminal-control "get-window-size" */


/* revision history:

	= 1998-08-20, David A­D­ Morano
	This is new code.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Get the window size from the terminal.

	Synopsis:

	int tcgetws(fd)
	int	fd ;

	Arguments:

	fd		file-descriptor of terminal

	Returns:
	>=0		number of lines
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<termios.h>
#include	<string.h>
#include	<usystem.h>
#include	<usupport.h>
#include	<localmisc.h>


/* external subroutines */


/* exported subroutines */

int tcgetws(int fd,WINSIZE *wsp) noex {
	int		rs = SR_FAULT ;
	if (wsp) {
	    rs = SR_BADF ;
	    if (fd >= 0) {
	        memclear(wsp) ;
	        rs = u_ioctl(fd,TIOCGWINSZ,wsp) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (tcgetws) */


