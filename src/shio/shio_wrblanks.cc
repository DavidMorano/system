/* shio_wrblanks SUPPORT */
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
	shio_wrblanks

	Description:
	Write a specified number of blanks to a SHIO object.

	Synopsis:
	int shio_wrblanks(shio *fp,int n) noex

	Arguments:
	fp	SHIO object pointer
	n	number of blanks to write

	Returns:
	>=0	nnumber bytes written
	<0	error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<strn.h>		/* LIBUC |strnset(3uc)| */
#include	<localmisc.h>		/* LIBU |MIN| */

#include	"shio.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/uconstants.ccm"

import libutil ;			/* |lenstr(3u)| */
import uconstants ;

/* local defines */


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	shio_wrblanks(shio *,int) noex ;
} /* end extern (C) */

/* external variables */


/* local structures */

namespace {
    struct blanker {
	cint	l = lenstr(sysword.w_blanks) ;
	ccharp	p = sysword.w_blanks ;
    } ; /* end struct (blanker) */
} /* end namespace */


/* forward references */


/* local variables */

static blanker		bo ;	/* "blank" object */


/* exported variables */


/* exported subroutines */

int shio_wrblanks(shio *fp,int n) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ; /* return-value */
	if (fp) ylikely {
	    rs = SR_INVALID ;
	    if (n >= 0) ylikely {
	        rs = SR_OK ;
	        while ((rs >= 0) && (wlen < n)) {
	            cint ml = min((n - wlen),bo.l) ;
	            rs = shio_write(fp,bo.p,ml) ;
	            wlen += rs ;
	        } /* end while */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (shio_wrblanks) */


