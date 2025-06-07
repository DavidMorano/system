/* usockaddr SUPPORT */
/* encoding=ISO8859-1 */
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

	Name:
	u_fcntl

	Description:
	This provides special control for devices.

	Synopsis:
	int uc_fcntl(int fd,int cmd,...) noex

	Arguments:
	fd		file-descriptor
	cmd		command (request)
	...		any-argument

	Notes:
	1. Because of the subroutine signature of |u_fcntl(3u)|
	allowing for a variable number of arguments and also
	importantly allowing for arguments with any types, passing
	those arguments down to another subroutine is potentially
	problematic.  For little-ending machines, this is not a
	problem as it is such a superior machine byte ordering
	convention over the stupid and idiotic big-endian (idiotic)
	convention.  See the code below for how I handle this
	problem for big-endian machines.
	2. Note the intentional spelling of the structure below
	("ufuncter").

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdint>		/* |uintptr_t| */
#include	<cstdarg>
#include	<usystem.h>
#include	<usysflag.h>
#include	<endian.h>
#include	<localmisc.h>

#include	"ufiledesc.h"


/* local defines */


/* imported namespaces */

using namespace	ufiledesc ;		/* namespace */


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
}

int u_sockaddrrem(int s,void *ap,int *alp) noex {
	return u_getpeername(s,ap,alp) ;
}


/* local subroutines */


