/* isasocket SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* test if a given file descriptor is a socket */
/* version %I% last-modified %G% */


/* revistion history:

	= 1998-11-06, David A­D­ Morano
	Here is a little ditty to see if the FD that we have is a
	socket or not.

	= 2017-01-14, David A­D­ Morano
	I greatly simplified this by only using |getsockopt(3socket)|
	as the test.  This still should properly work on systems
	that do not support S_ISSOCK for sockets (older Solaris®).

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This is my own hack attempt at figuring out if the given
	FD is a socket or not. Would it not be nice if a |stat(2)|
	returned a file mode for sockets so that |S_ISSOCK(mode)|
	was true? Oh, but NOOOOOOOOOOOOOOOOOOOOOO!!!


*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<sys/conf.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<isoneof.h>
#include	<localmisc.h>

#include	"isfiledesc.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static bool	isNotSock(int) noex ;


/* local variables */

static constexpr int	rsock[] = {
	SR_NOTSOCK,
	SR_OPNOTSUPP,
	SR_NOTSUP,
	0
} ;


/* exported subroutines */

int isasocket(int fd) noex {
	cint		slev = SOL_SOCKET ;
	cint		scmd = SO_TYPE ;
	int		rs ;
	int		len = szof(int) ;
	int		val = 0 ;
	int		f = false ;
	if ((rs = u_getsockopt(fd,slev,scmd,&val,&len)) >= 0) {
	    f = true ;
	} else if (isNotSock(rs)) {
	    rs = SR_OK ;
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (isasocket) */


/* local subroutines */

static bool isNotSock(int rs) noex {
	return isOneOf(rsock,rs) ;
}
/* end subroutine (isNotSock) */


