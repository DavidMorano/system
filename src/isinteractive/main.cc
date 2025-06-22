/* main (isinteractive) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* whole program for ISINTERACTIVE */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdio>
#include	<pwd.h>
#include	<exitcodes.h>
#include	<localmisc.h>


/* external subroutines */


/* exported subroutines */

int main(int argc,mainv argv,mainv envv) {
	int		ex = 0 ;
	int		f = TRUE ;


	f = f && (isatty(0) > 0) ;

	f = f && (isatty(1) > 0) ;

	ex = (f) ? EX_OK : EX_TEMPFAIL ;

	return ex ;
}
/* end subroutine (main) */



