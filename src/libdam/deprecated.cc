/* deprecated SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* these are deprecated subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-16, David A­D­ Morano
	This subroutine was taken from the LOGDIR/LOGNAME program
	(fist written for the SunOS 4.xx environment in 1989).  This
	is severly hacked to work in the strange environment of
	PCSPOLL!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	deprecated

	Description:
	This module contains subroutines that are not allowed to be
	called from our codebase.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |system(3c)| + |abort(3c)| */
#include	<cstdio>		/* |gets(3stdio)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<localmisc.h>

#include	"deprecated.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int system(cchar *) noex {
    abort() ;
    return 0 ;
}
/* end subroutine (system) */

char *gets(char *) noex {
    abort() ;
    return 0 ;
}
/* end subroutine (gets) */


