/* echoargs_main SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* echo the program arguments to STDOUT */
/* version %I% last-modified %G% */

/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdio>		/* CSTD */
#include	<iostream>		/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"echoargs_config.h"


/* local defines */


/* local namespaces */

using std::cout ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int main(int argc,con mainv argv,con mainv) {
	int		rs = SR_OK ;
	int		ex = EXIT_SUCCESS ;
	if (argc > 1) {
	    for (int ai = 0 ; (rs >= 0) && (ai < argc) ; ai += 1) {
		if (ccharp ap = argv[ai] ; *ap) {
		    printf("%2u> %s\n",ai,ap) ;
		} /* end if */
	    } /* end for */
	} /* end if (arguments) */
	if ((ex == EXIT_SUCCESS) && (rs < 0)) {
	    ex = EXIT_FAILURE ;
	} /* end if (error) */
	return ex ;
} /* end subroutine (main) */


