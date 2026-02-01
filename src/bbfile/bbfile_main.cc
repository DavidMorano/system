/* bbfile SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* convert arbitrary string to standard file name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/*******************************************************************************

	Name:
	main

	Description:

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstdio>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<ischarx.h>
#include	<localmisc.h>


/* local defines */

#define	BB_MAXNAMELEN	14


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int main(int argc,mainv argv,mainv) {
    	int		ex = EXIT_SUCCESS ;
	int		rs = SR_OK ;
	int j, k ;

	k = 0 ;
	for (int i = 1 ; i < argc ; i += 1) {
	    j = 0 ;
	    while (argv[i][j] != '\0') {

	        if (isalnumlatin(argv[i][j]) ||
	            argv[i][j] == '+' ||
	            argv[i][j] == '_' ||
	            argv[i][j] == '-' ||
	            argv[i][j] == '.' ) {

	            putc(argv[i][j],stdout) ;

	            if (++k == BB_MAXNAMELEN) 
			goto done ;

	        }

	        j += 1 ;

	    } /* end while */

	    if (++k == BB_MAXNAMELEN) 
		break ;

	    if ((i + 1) == argc) 
		break ;

	    putc('_',stdout) ;

	} /* end for */

	putc('\n',stdout) ;

	return ex ;
}
/* end subroutine (main) */


