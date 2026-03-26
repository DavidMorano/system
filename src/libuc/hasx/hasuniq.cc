/* hasuniq SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++11 */

/* test whether the counted c-string consists of all unique characters */
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
	true		all unique
	false		all unique not

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<climits>		/* |UCHAR_MAX| */
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

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

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

bool hasuniq(cchar *bp,int µbl) noex {
    	bool		f = false ;
	if (int bl ; (bl = getlenstr(bp,µbl)) > 0) ylikely {
	    for (bitset<nchars> seen ; bl-- && *bp ; ) {
	        cint	ch = mkchar(*bp++) ;
		if ((f = seen[ch])) break ;
		seen[ch] = true ;
	    } /* end for */
	} /* end if (getlenstr) */
	return f ;
}
/* end subroutine (hasuniq) */


/* local subroutines */


