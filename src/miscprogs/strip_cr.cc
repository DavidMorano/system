/* strip SUPPORT */
/* lang=C++20 */

/* program to strip comments from an assemble language program */
/* version %I% last-modified %G% */


/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

/*******************************************************************************

	This program will remove all carriage return characters
	from the ends of all lines.

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

#define		EOL	'\n'		/* end of line mark */
#define		CR	'\r'


/* exported variables */


/* exported subroutines */

int main(int,mainv,mainv) {
	int	c ;

/* come to start a new line */
top:
	c = getchar()  ;

	if (c == EOF) return OK ;

	if (c != CR) putchar(c) ;

	goto top ;

}
/* end of program */

