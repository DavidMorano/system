/* shio_writeblanks SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* write blacks to a SHIO object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	shio_writeblanks

	Description:
	Write a specified number of blanks to a SHIO object.

	Synopsis:
	int shio_writeblanks(shio *fp,int n) noex

	Arguments:
	fp	SHIO object pointer
	n	number of blanks to write

	Returns:
	>=0	nnumber bytes written
	<0	error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |memset(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<strn.h>		/* |strnset(3uc)| */
#include	<localmisc.h>		/* |MIN| */

#include	"shio.h"

import uconstants ;

/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	shio_writeblanks(shio *,int) noex ;
}

/* external variables */


/* local structures */

namespace {
    struct blanker {
	cint	l = lenstr(sysword.w_blanks) ;
	cchar	*p = sysword.w_blanks ;
    } ; /* end struct (blanker) */
}


/* forward references */


/* local variables */

static blanker			bo ;	/* "blank" object */


/* exported variables */


/* exported subroutines */

int shio_writeblanks(shio *fp,int n) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (fp) {
	    rs = SR_INVALID ;
	    if (n >= 0) {
	        rs = SR_OK ;
	        while ((rs >= 0) && (wlen < n)) {
	            cint	ml = min((n - wlen),bo.l) ;
	            rs = shio_write(fp,bo.p,ml) ;
	            wlen += rs ;
	        } /* end while */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (shio_writeblanks) */


