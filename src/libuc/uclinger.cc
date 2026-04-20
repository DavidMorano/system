/* uclinger */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* set (or unset) a LINGER time-out on a socket */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/socket.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<localmisc.h>


/* forward references */


/* exported variables */


/* exported subroutines */

int uc_linger(int fd,int to) noex {
	int		rs ;
	if (ustat sb ; (rs = u_fstat(fd,&sb)) >= 0) {
	    if (S_ISSOCK(sb.st_mode)) {
	        LINGER	ls{} ;
		cint	sol = SOL_SOCKET ;
		cint	cmd = SO_LINGER ;
		cint	llen = sizeof(LINGER) ;
		if (to >= 0) {
	            ls.l_onoff = TRUE ;
	            ls.l_linger = to ;
		}
		{
		    cchar	*obuf = charp(&ls) ;
	            rs = u_setsockopt(fd,sol,cmd,obuf,llen) ;
		}
	    } /* end if (socket) */
	} /* end if (stat) */
	return rs ;
}
/* end subroutine (uc_linger) */


