/* deprecated SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* check for the ONC private key stuff */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-08-17, David A­D­ Morano
	This subroutine was taken from the LOGDIR/LOGNAME program
	(fist written for the SunOS 4.xx environment in 1989).  This
	is severly hacked to work in the strange environment of
	PCSPOLL!

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	deprecated

	Description:
	This module contains subroutines that are not allowed to be
	called from our codebase.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |system(3c)| */
#include	<cstdio>		/* |gets(3stdio) */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<localmisc.h>


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


