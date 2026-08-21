/* usockaddr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Group:
	u_sockaddr{x}

	Description:
	This provides special control for devices.

	Synopsis:
	int u_sockaddr{x}(int fd,int cmd,...) noex

	Arguments:
	fd		file-descriptor
	cmd		command (request)
	...		any-argument

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<cerrno>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdint>		/* CSTD |uintptr_t| */
#include	<cstdarg>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<usysflag.h>		/* LIBU */
#include	<endian.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"ufiledescbase.hh"
#include	"usockaddr.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward refeferences */


/* local variables */


/* exported variables */


/* exported subroutines */

int u_sockaddrloc(int s,void *ap,int *alp) noex {
	return u_getsockname(s,ap,alp) ;
} /* end subroutine */

int u_sockaddrrem(int s,void *ap,int *alp) noex {
	return u_getpeername(s,ap,alp) ;
} /* end subroutine */


/* local subroutines */


