/* cupper SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* convert characters to upper case */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	Originally written but adapted from assembly.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Description:
 	Thes are c-string library subroutines.
	This subroutine converts a string of characters to upper case.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
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

int cupper(cchar *src,cchar *dst,int len) noex {
	if (len <= 0) {
	    len = 0 ;
	    while (*src != '\0') {
	        *dst++ = 
	            ((*src >= 'a') && (*src <= 'z')) ? (*src++ - 32) : *src++ ;
	        len += 1 ;
	    } /* end for */
	} else {
	    for (int i = 0 ; i < len ; i += 1) {
	        *dst++ = 
	            ((*src >= 'a') && (*src <= 'z')) ? (*src++ - 32) : *src++ ;
	    } /* end for */
	} /* end if */
	return len ;
}
/* end subroutine (cupper) */


