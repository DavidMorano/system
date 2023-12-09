/* strnnlen */
/* lang=C20 */


/* calculate the length of a counted c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strnnlen

	Description:
	This subroutine computes the length of a supplied string
	but will not examine the string for more than a specified
	length.

	Synopsis:
	int strnnlen(cchar *sp,int sl,int max) noex

	Arguments:
	sp	string to be examined
	sl	length of string (or -1)
	nax	maximum length of string to be examined

	Returns:
	len	mimimum of length of string or MIN(slen,max)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<string.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<localmisc.h>


/* external subroutines */


/* exported subroutines */

int strnnlen(cchar *sp,int sl,int mlen) noex {
	int		len = 0 ;
	if (sp && (sl != 0) && (mlen != 0)) {
	    if (sl >= 0) {
	        if (mlen >= 0) sl = MIN(sl,mlen) ;
	    } else {
	        sl = mlen ;
	    }
	    len = strnlen(sp,sl) ;
	} /* end if */
	return len ;
}
/* end subroutine (strnnlen) */


