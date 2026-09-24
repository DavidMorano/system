/* usupport_strnwcpy SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® kernel support subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-26, David A­D­ Morano
	This was first written to give a little bit to UNIX® 
	of what we have in our own circuit-pack OSes!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	strnwcpy
	strnwcpy{x}c

	Synopsis:
	char *strnwcpy(char *dp,int dl,cchar *sp,int sl) noex
	char *strnwcpy{x}c(char *dp,int dl,cchar *sp,int sl) noex

	Arguments:
	{x}		Base, Lower, Upper, Fold
	dp		destination character buffer pointer
	dl		destination character buffer length
	sp		source c-string pointer
	sl		source c-string length

	Returns:
	-		pointer to the end of the filled in part
			of the created destination string

	Notes:
	+ fixed-size destiantion buffer
	+ no mandatory NUL termination
	+ zero fills out the destiantion buffer
	+ acts very similarly to |strncpy(3c)|

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"usupport_toxc.hh"
#include	"usupport_strnwcpy.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */


/* imported namespaces */

using libu::tobc ;			/* subroutine */
using libu::tolc ;			/* subroutine */
using libu::touc ;			/* subroutine */
using libu::tofc ;			/* subroutine */


/* local typedefs */

extern "C" {
    typedef int (*toxc_f)(int) noex ;
} /* end extern (C) */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local char *strnwcpyxc(toxc_f toxc,char *dp,int dl,cchar *sp,int sl) noex {
	if (dp && sp) ylikely {
    	    while (dl && sl-- && *sp) {
	        *dp++ = char(toxc(*sp++)) ;
		dl -= 1 ;
	    }
	    if (dl > 0) {
	        memclear(dp,dl) ;
	    }
	} /* end if (non-null) */
	return dp ;
} /* end subroutine (strnwcpyxc) */


/* local variables */


/* exported variables */


/* exported subroutines */

namespace libu {
    char *strnwcpybc(char *dp,int dl,cchar *sp,int sl) noex {
    	return strnwcpyxc(tobc,dp,dl,sp,sl) ;
    } /* end subroutine */
    char *strnwcpylc(char *dp,int dl,cchar *sp,int sl) noex {
    	return strnwcpyxc(tolc,dp,dl,sp,sl) ;
    } /* end subroutine */
    char *strnwcpyuc(char *dp,int dl,cchar *sp,int sl) noex {
    	return strnwcpyxc(touc,dp,dl,sp,sl) ;
    } /* end subroutine */
    char *strnwcpyfc(char *dp,int dl,cchar *sp,int sl) noex {
    	return strnwcpyxc(tofc,dp,dl,sp,sl) ;
    } /* end subroutine */
} /* end namespace (libu) */


/* local subroutines */


