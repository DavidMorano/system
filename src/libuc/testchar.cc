/* testchar SUPPORT */
/* lang=C++20 */

/* test if |orintf(3c)| can print out ISO-Latin1 characters */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

#include	<envstandards.h>	/* ordered first to configure */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<iostream>
#include	<localmisc.h>

using namespace std ;

int main(int,mainv,mainv) {
	int	ch = 'À' ;
	printf("%c\n",ch) ;
}


