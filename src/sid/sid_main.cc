/* sid_main SUPPOERT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* this program prints out the current process Session-ID */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<iostream>		/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */


/* exported variables */


/* exported subroutines */

int main(int,con mainv,con mainv) {
	int	sid = getsid(0) ;
	std::cout << sid << '\n' ;
} /* end subroutine (main) */


