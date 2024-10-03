/* strip SUPPORT */
/* lang=C++20 */

/* program to strip comments from an assemble language program */
/* version %I% last-modified %G% */


/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

/*******************************************************************************

	This program will remove all comment lines and all empty
	lines from the input file.  Comment lines have ' ' characters
	in them.

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

#define		SC	' '		/* character to remove */

#define		EOL	'\n'		/* end of line mark */


/* exported variables */


/* exported subroutines */

int main(int,mainv,mainv) {
	int	c ;


/* come to start a new line */
top:
	c = getchar() ;

	if (c == EOF) return 0 ;

	if (c == EOL) goto top ;

	if (c == SC) goto waste ;	/* jump if waste rest of line */


	putchar(c) ;

/* we have a non-zero length line */
loop:
	c = getchar() ;
	if (c == EOF) return 0 ;

	if (c == SC) goto lose ;	/* jump if want to lose rest of line */

	putchar(c) ;
	if (c == EOL) goto top ;

	goto	loop ;

/* lose the rest of a line */
lose:
	c = getchar() ;
	if (c == EOF) return 0 ;

	if (c != EOL) goto lose ;

	putchar(EOL) ;

	goto	top ;


/* waste the reset of a line */
waste:
	c = getchar() ;
	if (c == EOF) return 0 ;

	if (c != EOL) goto waste ;

	goto	top ;

}
/* end of program */

