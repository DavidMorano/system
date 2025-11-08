/* uc_reuseaddr */
/* charset=ISO8859-1 */

/* interface component for UNIX® library-3c */
/* reuse an existing (perhaps) socket address */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<usystem.h>
#include	<localmisc.h>


/* exported subroutines */

int uc_reuseaddr(int fd) noex {
	cint		optlen = sizeof(int) ;
	cint		sol = SOL_SOCKET ;
	cint		so = SO_REUSEADDR ;
	int		rs ;
	int		one = 1 ;
	return u_setsockopt(fd,col,so,&one,optlen) ;
}
/* end subroutine (uc_reuseaddr) */


