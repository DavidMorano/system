/* cmpc SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* routine to compare two character strings for equallity */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Description:
 	Thes are c-string library subroutines.
	This subroutine converts a string of characters to upper case.

*******************************************************************************/

#include	<envstandards.h>	/* must be ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>


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

int cmpc(int len,cchar *src,cchar *dst) noex {
	int		rc = 0 ;
	for (int i = 0 ; i < len ; i += 1) {
	    rc = (*src++ - *dst++) ;
	    if (rc != 0) break ;
	}
	return rc ;
}
/* end subroutine (cmpc) */


