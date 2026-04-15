/* strlcpy SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* buffer-size-conscious string operation */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-01, David A­D­ Morano
	This subroutine was written because I want to use these
	new (experimental?) subroutines on platforms other than
	Solaris!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

  	Name:
	strlcpy

	Description:
	This subroutine is a knock off of the |strlcpy(3c)| that
	first appeared in the Solaris UNIX® system from Sun
	Microsystems.

******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
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

int strlcpy(char *dst,cchar *src,int maxlen) noex {
    	int		len = 0 ; /* return-value */
	if (dst && src) {
	    int		i{} ; /* used afterwards */
	    if (maxlen > 0) {
	        for (i = 0 ; (i < (maxlen - 1)) && *src ; i += 1) {
	            dst[i] = *src++ ;
	        }
	    }
	    dst[i] = '\0' ;
	    len = (*src == '\0') ? i : (i + int(strlen(src))) ;
	} /* end if (non-null) */
	return len ;
}
/* end subroutine (strlcpy) */


