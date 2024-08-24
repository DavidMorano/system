/* tcgetlines */

/* UNIX® terminal-control "get-lines" */


/* revision history:

	= 1998-08-20, David A­D­ Morano
	This is new code.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This is an attempt at abstracting how to get the number of
	screen-lines that may have been set into the terminal driver
	device instance.

	Synopsis:

	int tcgetlines(fd)
	int	fd ;

	Arguments:

	fd		file-descriptor of terminal

	Returns:

	<0		error
	>=0		number of lines


*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<termios.h>
#include	<string.h>
#include	<usystem.h>
#include	<localmisc.h>


/* external subroutines */


/* exported subroutines */

int tcgetlines(int fd) noex {
	int		rs = SR_BADF ;
	if (fd >= 0) {
	    struct winsize	ws ;
	    memset(&ws,0,sizeof(struct winsize)) ;
	    if ((rs = u_ioctl(fd,TIOCGWINSZ,&ws)) >= 0) {
	        rs = ws.ws_row ;
	    }
	} 
	return rs ;
}
/* end subroutine (tcgetlines) */


