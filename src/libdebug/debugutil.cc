/* debugutil SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* debugging stubs */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-15, David A­D­ Morano
	This was written to debug the REXEC program.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	debugutil

	Description:
	This modeule provides debugging support.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdarg>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<usupport.h>		/* |strwcpy(3u)| */
#include	<uclibmem.h>		/* LIBUC */
#include	<snflagsx.h>		/* LINUC */
#include	<localmisc.h>		/* LIBU |COLUMNS| */

#include	"debugutil.hh"
#include	"libdebug.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */

using libu::strwcpy ;			/* subroutine */


/* external subroutines */


/* external variables */


/* local structures */


/* forward subroutines */


/* local variables */


/* exported variables */


/* exported subroutines */

namespace libdebug {
    int checkbasebounds(cchar *bbuf,int blen,cvoid *vp) noex {
	int		rs = SR_FAULT ;
	if (bbuf && vp) {
	    rs = SR_OK ;
	    if (cchar *tp = ccharp(vp) ; tp >= bbuf) {
	        if (tp >= (bbuf + blen)) {
		    rs = SR_BADFMT ;
	        } /* end */
	    } else {
	        rs = SR_BADFMT ;
	    } /* end */
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (checkbasebounds) */
} /* end namespace (libdebug) */


