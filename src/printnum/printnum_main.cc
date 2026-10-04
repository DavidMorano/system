/* main SUPPORT (printnum) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

/*******************************************************************************

  	Name:
	main

	Descirpion:
	This is the whole of the PRINTNUM program.

*******************************************************************************/

#include	<envstandards.h>	/* must be ordered first to configure */
#include	<sys/types.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdio>
#include	<usystem.h>
#include	<cfdec.h>
#include	<ischarx.h>
#include	<localmisc.h>


/* external subroutines */


/* exported variables */


/* exported subroutines */

int main(int argc,mainv argv,mainv envv) {
	int	rs = SR_OK ;
	int	ex = EXIT_SUCCESS ;
	for (int i = 1 ; i < argc ; i += 1) {
	    cchar	*ap = argv[i] ;
	    if (ap  && (ap[0] != '\0')) {
	        if (int uc ; (rs = cfdecui(ap,-1,&c)) >= 0) {
		    if (isprintlatin(c)) {
	                fprintf(stdout,">%c<\n",c) ;
		    } else {
		        fprintf(stderr,"not printable (%u)\n",c) ;
		    }
		} else {
		    fprintf(stderr,"invalid number specified (%d)\n",rs) ;
		}
	    }
	} /* end for */
	if (rs < 0) ex = EXIT_FAILURE ;
	return ex ;
}
/* end subroutine (main) */


