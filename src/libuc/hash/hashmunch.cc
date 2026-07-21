/* hashmunch SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* create an index from a hash value */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
        This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************* 
  	Name:
	hashmunch

	Description:
	Munch data in a given buffer to an integer.

	Synopsis:
	int hashmunch(cvoid *bufp,int bufl,{x} *rp) noex

	Arguments:
	bufp		buffer to munch pointer
	bufl		buffer to munch length
	rp		result pointer

	Returns:
	>=0	OK
	<0	error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD |CHAR_BIT| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<concepts>		/* C++CSTD */
#include	<bit>			/* C++CSTD */
#include	<cstdintx>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* |MODP2| */

#include	"hashmunch.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */

using std::integral_unsigned ;		/* concept */
using std::rotl ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

template<integral_unsigned UT>
local int hashmunchx(cvoid *bufp,int bufl,UT *rp) noex {
	cint		sb = UCHAR_BIT ;
	int		rs = SR_FAULT ;
	if (bufp && rp) {
	    charp	gbp = charp(bufp) ;
	    rs = SR_OK ;
	    if (int bl = getlenstr(gbp,bufl) ; bl > 0) {
	        UT	hv = 0 ;
	    	ucharp	ubp = ucharp(bufp) ;
		for (int i = 0 ; i < bl ; i += 1) {
		    hv = rotl(hv,sb) ;
		    hv |= UT(ubp[i]) ;
		} /* end for */
		*rp = hv ;
	    } else {
		*rp = UT(0) ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine-template (hashmunchx) */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int hashmunchui		(cvoid *bufp,int bufl,uint *rp)		noex {
    	return hashmunchx(bufp,bufl,rp) ;
} /* end subroutine (hashmunchui) */

int hashmunchul		(cvoid *bufp,int bufl,ulong *rp)	noex {
    	return hashmunchx(bufp,bufl,rp) ;
} /* end subroutine (hashmunchui) */

int hashmunchuull	(cvoid *bufp,int bufl,ulonglong *rp)	noex {
    	return hashmunchx(bufp,bufl,rp) ;
} /* end subroutine (hashmunchui) */


