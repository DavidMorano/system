/* uckeepalive */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* set the KeepAlive mode on a (socket) file descriptor */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/uio.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<poll.h>
#include	<usysbase.h>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<localmisc.h>


/* forward references */


/* exported variables */


/* exported subroutines */

int uc_keepalive(int fd,int f) noex {
	cint		slev = SOL_SOCKET ;
	cint		scmd = SO_KEEPALIVE ;
	int		rs ;
	int		keepalive ;
	int		optlen = sizeof(int) ;
	int		fprev = true ;
	if ((rs = u_getsockopt(fd,slev,scmd,&keepalive,&optlen)) >= 0) {
	    fprev = (keepalive != 0) ;
	    if (! LEQUIV(fprev,f)) {
	        keepalive = f ;
	        rs = u_setsockopt(fd,slev,scmd,&keepalive,sizeof(int)) ;
	    } /* end if (needed a change) */
	} /* end if */
	return (rs >= 0) ? fprev : rs ;
}
/* end subroutine (uc_keepalive) */


