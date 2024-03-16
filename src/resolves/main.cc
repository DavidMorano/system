/* main (resolves) */

/* generic front-end for SHELL built-ins */
/* version %I% last-modified %G% */


#define	CF_DEBUGS	0		/* non-switchable debug print-outs */


/* revision history:

	= 1998-07-28, David A�D� Morano

	This subroutine was written for use as a front-end for Korn Shell
	(KSH) commands that are compiled as stand-alone programs.


*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/**************************************************************************

	Synopsis:

	$ resolves ...


*****************************************************************************/


#include	<envstandards.h>

#include	<sys/types.h>
#include	<sys/param.h>
#include	<climits>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstdlib>
#include	<string.h>

#include	<usystem.h>
#include	<localmisc.h>

#include	"b_resolves.h"
#include	"defs.h"


/* local defines */


/* external subroutines */

extern int	b_resolves(int,char **,void *) ;


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */


int main(argc,argv,envv)
int	argc ;
char	*argv[] ;
char	*envv[] ;
{
	int	ex ;


	ex = b_resolves(argc,argv,NULL) ;

	return ex ;
}
/* end subroutine (main) */



