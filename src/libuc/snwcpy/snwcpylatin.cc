/* snwcpylatin SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* special (excellent) string-copy type of subroutine! */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-08, David A­D­ Morano
	This was written to clean up some problems with printing
	garbage characters in a few place in some PCS utilities.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	snwcpylatin

	Description:
	This is essentially the same as the |snwcpy(3uc)| subroutine
	except that non-printable characters are replaced with a
	stuff-mark character.  Actually, this is identical to
	the |snwcpyclean(3uc)| subroutine.

	Synopsis:
	int snwcpylatin(char *dp,int dl,cchar *sp,int sl) noex

	Arguments:
	dp		destination string buffer
	dl		destination string buffer length
	sp		source string
	sl		source string length

	Returns:
	>=0		number of bytes in result
	<0		error (system-return)

	Notes:
	This subroutine just calls the |snwcpyclean(3uc)|
	subroutine w/ our substitution character.

	See-also:
	snwcpy(3uc),
	snwcpylatin(3uc), 
	snwcpyopaque(3uc), 
	snwcpycompact(3uc), 
	snwcpyclean(3uc), 
	snwcpyhyphen(3uc),

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD |UCHAR_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"snwcpyx.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int snwcpylatin(char *dbuf,int dlen,cchar *sp,int sl) noex {
    	cint	sch = ('-' + 128) ;
    	return snwcpyclean(dbuf,dlen,sch,sp,sl) ;
} /* end subroutine (snwcpylatin) */


