/* b_ustime SUPPORT (ustime) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* generic front-end for SHELL built-ins */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* non-switchable debug print-outs */

/* revision history:

	= 1998-07-28, David A­D­ Morano
	This subroutine was written for use as a front-end for Korn
	Shell (KSH) commands that are compiled as stand-alone
	programs.


*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Synopsis:
	$ utime

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"b_ustime.h"
#include	"defs.h"


/* local defines */


/* external subroutines */

extern int	p_utime(int,con mainv,con mainv,void *) noex ;


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int main(int argc,con mainv argv,con mainv envv) {
	int	ex ;
	{
	    ex = p_utime(argc,argv,envv,nullptr) ;
	}
	return ex ;
} /* end subroutine (main) */


