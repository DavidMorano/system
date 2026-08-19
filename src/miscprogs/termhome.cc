/* termhome SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* clear the terminal screen */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

/*******************************************************************************

	Name:
	termhome

	Description:
	This program places the cursor of a terminal to the top of the
	screen.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdio>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ascii.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */


/* local defines */


/* exported variables */


/* exported subroutines */

int main(int,con mainv,con mainv) {
	printf("%1c[H",CH_ESC) ;
	return 0 ;
} /* end subroutine (main) */


