/* main */
/* lang=C++20 */

/* generic front-end for SHELL built-ins */
/* version %I% last-modified %G% */


/* revision history:

	= 1989-03-01, David A.D. Morano
	This subroutine was originally written.

*/

/* Copyright © 1989 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Synopsis:

	$ userhome [<username(s)>|-] [-af <afile>]


*******************************************************************************/


#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<limits.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<string.h>
#include	<usystem.h>
#include	<exitcodes.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int main(int argc,const char **argv,const char **envv) noex {
	int	ex = 0 ;



	return ex ;
}
/* end subroutine (main) */


