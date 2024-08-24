/* strlcat SUPPORT */
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

/*******************************************************************************

	This subroutine is a knock off of the 'strlcat()' that first
	appeared in the Solaris UNIX system from Sun Microsystems.

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<clanguage.h>


/* external subroutines */

extern "C" {
    extern size_t strlen(const char *) noex ;
}


/* exported subroutines */

size_t strlcat(char *dst,const char *src,size_t maxlen) noex {
	size_t		rsz = 0 ;
	if (dst && src) {
	    cint	is = int(strlen(dst)) ;
	    cint	ml = int(maxlen) ;
	    int		i{} ;
	    for (i = is ; (i < (ml - 1)) && *src ; i += 1) {
	        dst[i] = *src++ ;
	    }
	    dst[i] = '\0' ;
	    rsz = size_t((*src == '\0') ? i : (i+int(strlen(src)))) ;
	}
	return rsz ;
}
/* end subroutine (strlcat) */


