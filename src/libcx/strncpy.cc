/* strncpy SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* routine to copy a string to a sized buffer (zeroed remainder) */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	strncpy

	Description:
	Copy a source c-string to a destination buffer up to a maximum
	source string length.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
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

char *strncpy(char *dst,cchar *src,size_t msz) noex {
    	if (dst && src) {
	    cint	n = int(msz) ;
	    int		i{} ; /* used-afterwards */
	    for (i = 0 ; (i < n) && src[i] ; i += 1) {
	        dst[i] = src[i] ;
	    }
	    while (i < n) {
	        dst[i] = '\0' ;
	    }
	} /* end if (non-null) */
	return dst ;
}
/* end subroutine (strncpy) */


