/* strncpyxc SUPPORT */
/* lang=C++20 */

/* copy a string to to *x* case */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

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
#include	<cstring>		/* |memset(3c)| */
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<toxc.h>

#include	"strcpyxc.h"
#include	"strncpyxc.h"


/* local defines */


/* local typedefs */

extern "C" {
    typedef char *(*scxc_f)(char *,cchar *) noex ;
}


/* external subroutines */


/* external variables */


/* local variables */


/* subroutine-templates */

template<int (*toxc)(int)>
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


