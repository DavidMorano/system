/* hasDotDir */
/* lang=C20 */

/* is a directory entry a dot-directory? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	The subroutine was adapted from others programs that did
	similar types of functions.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine determines if a directory entry is a
	dot-directory.

*******************************************************************************/

#include	<envstandards.h>
#include	<utypedefs.h>
#include	<clanguage.h>


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

bool hasDotDir(cchar *dp,int) noex {
	bool		f = false ;
	if (dp[0] == '.') {
	    f = f || (dp[1] == '\0') ;
	    f = f || ((dp[1] == '.') && (sp[2] == '\0')) ;
	}
	return f ;
}
/* end subroutine (hasDotDir) */


