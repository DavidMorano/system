/* strlcpy SUPPORT */
/* lang=C++20 */

/* buffer-size-conscious string operation */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-01, David A­D­ Morano
	This subroutine was written because I want to use these new
	(experimental?) subroutines on platforms other than Solaris!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	This subroutine is a knock off of the |strlcpy(3c)| that
	first appeared in the Solaris UNIX® system from Sun
	Microsystems.

******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstring>		/* |strlen(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>


/* local defines */


/* external subroutines */


/* exported variables */


/* exported subroutines */

size_t strlcpy(char *dst,cchar *src,size_t maxlen) noex {
	size_t		rsz = 0 ;
	if (dst && src) {
	    cint	ml = int(maxlen) ;
	    int		i{} ;
	    for (i = 0 ; (i < (ml - 1)) && *src ; i += 1) {
	        dst[i] = *src++ ;
	    }
	    dst[i] = '\0' ;
	    rsz = size_t((*src == '\0') ? i : (i + int(strlen(src)))) ;
	} /* end if (non-null) */
	return rsz ;
}
/* end subroutine (strlcpy) */


