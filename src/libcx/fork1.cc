/* fork1 SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* a |fork1(2)|if you do not already have it! */
/* version %I% last-modified %G% */


/* revision history:

	= 1989-03-01, David A-D- Morano
	This code was originally written.

*/

/* Copyright © 1989 David A-D- Morano.  All rights reserved. */

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<localmisc.h>


/* exported subroutines */

pid_t fork1() noex {
	return fork() ;
}


