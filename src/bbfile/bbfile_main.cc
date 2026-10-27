/* bbfile SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* convert arbitrary string to standard file name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

/*******************************************************************************

	Name:
	main

	Description:

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/param.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdio>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<isch.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"bbfile_config.h"


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
	int k ;
	k = 0 ;
	for (int i = 1 ; i < argc ; i += 1) {
	    int j = 0 ;
	    bool fdone = false ;
	    while (argv[i][j] != '\0') {
	        if (isalnumlatin(argv[i][j]) ||
	            argv[i][j] == '+' ||
	            argv[i][j] == '_' ||
	            argv[i][j] == '-' ||
	            argv[i][j] == '.' ) {
	            putc(argv[i][j],stdout) ;
	            if ((fdone = (++k == BB_MAXNAMELEN))) break ;
	        } /* end if */
	        j += 1 ;
		if (fdone) break ;
	    } /* end while */
	    if (fdone) break ;
	    if (++k == BB_MAXNAMELEN) break ;
	    if ((i + 1) == argc) break ;
	    putc('_',stdout) ;
	} /* end for */
	putc('\n',stdout) ;
	return ex ;
} /* end subroutine (main) */


