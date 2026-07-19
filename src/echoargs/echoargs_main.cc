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
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<bfile.h>
#include	<localmisc.h>		/* LIBU */


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int main(int argc,con mainv argv,con mainv envv) {
	int		rs ;
	int		ex = EXIT_SUCCESS ;
	int		rs1 ;
	cchar		*ofn = BFILE_STDOUT ;
	if (bfile of ; (rs = of.open(ofn,"dwct",0666)) >= 0) {
	    for (int i = 0 ; (rs >= 0) && (i < argc) ; i += 1) {
		of.printf("%u> %s\n",i,argv[i]) ;
	    }
	    rs1 = of.close(ofp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (o-file) */
	if ((ex == EXIT_SUCCESS) && (rs < 0)) {
	    ex = EXIT_FAILURE ;
	}
	return ex ;
}
/* end subroutine (main) */


