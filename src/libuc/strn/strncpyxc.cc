/* strncpyxc SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* copy a string to to *x* case */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strncpylc
	strncpyuc
	strncpyfc

	Description:
	This subroutine is like |strncpy(3c)| (with its non-NUL
	terminating behavior) except that the case of the characters
	are converted as desired.

	Synopsis:
	char *strncpy{x}c(char *dst,cchar *src,int n) noex

	Arguments:
	{x}		l, u, f
	dst		destination buffer
	src		source string
	n		length to copy

	Returns:
	last		pointer to one character past the end of destination

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |memset(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<toxc.h>
#include	<localmisc.h>

#include	"strcpyxc.h"
#include	"strncpyxc.h"


/* local defines */


/* imported namespaces */


/* local typedefs */

extern "C" {
    typedef int (*toxc_f)(int) noex ;
    typedef char *(*scxc_f)(char *,cchar *) noex ;
}


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<toxc_f toxc>
char *strncpyxc(scxc_f scxc,char *dst,cchar *src,int n) noex {
	if (dst && src) {
	    dst[0] = '\0' ;
	    if (n >= 0) {
	        while (n && *src) {
	            *dst++ = toxc(*src) ;
	            src += 1 ;
		    n -= 1 ;
	        } /* end while */
	        if (n > 0) {
	            memset(dst,0,n) ;
		}
	    } else {
	        dst = scxc(dst,src) ;
	    }
	} /* end if (non-null) */
	return dst ;
}
/* end subroutine-template (strncpyxc) */


/* local variables */


/* exported variables */


/* exported subroutines */

char *strncpybc(char *dst,cchar *src,int n) noex {
	return strncpyxc<tobc>(strcpybc,dst,src,n) ;
}

char *strncpylc(char *dst,cchar *src,int n) noex {
	return strncpyxc<tolc>(strcpylc,dst,src,n) ;
}

char *strncpyuc(char *dst,cchar *src,int n) noex {
	return strncpyxc<touc>(strcpyuc,dst,src,n) ;
}

char *strncpyfc(char *dst,cchar *src,int n) noex {
	return strncpyxc<tofc>(strcpyfc,dst,src,n) ;
}


