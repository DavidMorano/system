/* modsnwcpy MODULE (implemenation) */
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

	Name:
	snwcpy

	Description:

	Synopsis:
    	int snwcpy(char *dp,int dl,cchar *sp,int sl) noex {

	Arguments:
	dp		destiation buffer pointer
	dp		destiation buffer length
	sp		source c-string pointer
	sl		source c-string length

	Returns:
	>=0		amount of data returned
	<0		error (system-return)

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

module modsnwcpy ;

import modsncpy ;
import modstrw ;

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int snwcpy(char *dp,int dl,cchar *sp,int sl) noex {
	int		rs ;
	if (dl >= 0) {
	    if (sl >= 0) {
	        if (sl > dl) {
	            rs = sncpy1(dp,dl,sp) ;
	        } else {
	            rs = intconv(strwcpy(dp,sp,sl) - dp) ;
		}
	    } else {
	        rs = sncpy1(dp,dl,sp) ;
	    }
	} else {
	    rs = intconv(strwcpy(dp,sp,sl) - dp) ;
	}
	return rs ;
} /* end subroutine (snwcpy) */


