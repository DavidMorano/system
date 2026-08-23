/* uckeepalive SUPPORT */
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
#include	<sys/types.h>		/* POSIX */
#include	<sys/uio.h>		/* POSIX */
#include	<sys/stat.h>		/* POSIX */
#include	<unistd.h>		/* POSIX */
#include	<fcntl.h>		/* POSIX */
#include	<poll.h>		/* POSIX */
#include	<usysbase.h>		/* LIBU */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ucdesc.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"ucdescsock.h"


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

int uc_keepalive(int fd,int f) noex {
	cint		slev = SOL_SOCKET ;
	cint		scmd = SO_KEEPALIVE ;
	int		rs ;
	mut int		keepalive{} ;
	int		optlen = szof(int) ;
	int		fprev = true ; /* return-value */
	if ((rs = u_sockoptget(fd,slev,scmd,&keepalive,&optlen)) >= 0) {
	    fprev = (keepalive != 0) ;
	    if (! LEQUIV(fprev,f)) {
	        keepalive = f ;
	        rs = u_setsockopt(fd,slev,scmd,&keepalive,szof(int)) ;
	    } /* end if (needed a change) */
	} /* end if */
	return (rs >= 0) ? fprev : rs ;
} /* end subroutine (uc_keepalive) */


