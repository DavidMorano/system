/* strip SUPPORT */
/* lang=C++20 */

/* program to strip lines starting w/ ".mc" */
/* version %I% last-modified %G% */


/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

/*******************************************************************************

	This program strips certain characters from STDIN and writes
	the resuling lines to STDOUT.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdio>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<localmisc.h>


/* local defines */


/* exported variables */


/* exported subroutines */

int main(int,mainv,mainv) {
	int	c, c0, c1, c2 ;

top:
	c0 = getchar() ;
	if (c0 == EOF) return (0L) ;

	if (c0 != '.') { putchar(c0) ; goto finish ; }

	c1 = getchar() ;
	if (c1 == EOF) return (0L) ;

	if (c1 != 'm') { putchar(c0) ; putchar(c1) ; goto finish ; }

	c2 = getchar() ;
	if (c2 == EOF) return (0L) ;

	if (c2 != 'c') 
		{ putchar(c0) ; putchar(c1) ; putchar(c2) ; goto finish ; }



	while ((c = getchar()) != '\n') ;

	if (c == EOF) return 0 ;

	goto	top ;

finish:
	c = getchar() ;
	if (c == EOF) return 0 ;

	while (c != '\n') {

		putchar(c) ;

		c = getchar() ;
		if (c == EOF) return 0 ;
	} ;

	putchar('\n') ;

	goto	top ;
}
/* end of program */

