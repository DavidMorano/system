/* strncpy SUPPORT */
/* lang=C++20 */

/* routine to copy a string to a sized buffer (zeroed remainder) */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#include	<sys/types.h>
#include	<utypedefs.h>
#include	<clanguage.h>


/* exported subroutines */

char *strncpy(char *dst,const char *src,size_t msz) noex {
	cint		n = int(msz) ;
	int		i{} ;
	for (i = 0 ; (i < n) && src[i] ; i += 1) {
	    dst[i] = src[i] ;
	}
	while (i < n) {
	    dst[i] = '\0' ;
	}
	return dst ;
}
/* end subroutine (strncpy) */


