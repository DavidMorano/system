/* rmchr */
/* lang=C20 */

/* return the c-string index located at a character */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-17, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	rmchr

	Description:
	This subroutine returns the c-string index up to the given
	character or it returns the original length of the given
	c-string if no character is found.

	Synopsis:
	int rmchr(cchar *fp,int fl) noex

	Arguments:
	fp		file-name string
	fl		file-name length

	Returns:
	==0		does not have an approved extension
	>0		has an approved extension (and this is base-str length)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<string.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<strn.h>
#include	<localmisc.h>

#include	"rmx.h"


/* local defines */


/* external subroutines */


/* forward references */


/* local variables */


/* exported subroutines */

int rmchr(cchar *sp,int sl,int ch) noex {
	cchar		*tp ;
	if (sl < 0) sl = strlen(sp) ;
	if ((tp = strnchr(sp,sl,ch)) != nullptr) {
	    sl = (tp-sp) ;
	} /* end if */
	return sl ;
}
/* end subroutine (rmchr) */


