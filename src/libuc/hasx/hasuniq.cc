/* hasuniq SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++11 */

/* test whether a string consists of all unique characters */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	hasuniq

	Description:
	We modify the given sring in place to remove any duplicate
	characters found.

	Synopsis:
	int hasuniq(cchar *bp,int bl) noex

	Arguments:
	sp		test c-string pointer
	sl		test c-string length

	Returns:
	>=0		length of given string
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<bitset>		/* |bitset(3c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"hasuniq.h"


/* local defines */


/* imported namespaces */


/* local typedefs */

using std::bitset ;			/* type */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

cint		nchars = (UCHAR_MAX + 1) ;


/* exported variables */


/* exported subroutines */

bool hasuniq(cchar *bp,int bl) noex {
    	bool		f = false ;
	if (bp) {
	    if (bl > 1) ylikely {
	        for (bitset<nchars> seen ; bl-- && *bp ; ) {
	            cint	ch = mkchar(*bp++) ;
		    if ((f = seen[ch])) break ;
		    seen[ch] = true ;
	        } /* end for */
	    } /* end if */
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (hasuniq) */


/* local subroutines */


