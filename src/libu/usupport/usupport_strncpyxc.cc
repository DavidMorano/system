/* usupport_strncpyxc SUPPORT */
/* charset=ISO8859-1 */
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
	strncpybc
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
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usupport.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"usupport_strncpyxc.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| + |memcopy(3u)| */

/* local defines */


/* imported namespaces */

using libu::chtobc ;			/* subroutine */
using libu::chtolc ;			/* subroutine */
using libu::chtouc ;			/* subroutine */
using libu::chtofc ;			/* subroutine */


/* local typedefs */

extern "C" {
    typedef char (*chtoxc_f)(int) noex ;
} /* end extern */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local char *strncpyxc(chtoxc_f chtoxc,char *dst,cchar *src,int n) noex {
	if (dst && src) ylikely {
	    dst[0] = '\0' ;
	    while (n-- && *src) {
	        *dst++ = chtoxc(*src++) ;
	    } /* end while */
	} /* end if (non-null) */
	return dst ;
} /* end subroutine (strncpyxc) */


/* local variables */


/* exported variables */


/* exported subroutines */

char *strncpybc(char *dst,cchar *src,int n) noex {
	return strncpyxc(chtobc,dst,src,n) ;
} /* end */

char *strncpylc(char *dst,cchar *src,int n) noex {
	return strncpyxc(chtolc,dst,src,n) ;
} /* end */

char *strncpyuc(char *dst,cchar *src,int n) noex {
	return strncpyxc(chtouc,dst,src,n) ;
} /* end */

char *strncpyfc(char *dst,cchar *src,int n) noex {
	return strncpyxc(chtofc,dst,src,n) ;
} /* end */


