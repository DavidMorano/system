/* termcmd SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* standard (relatively) UNIX® preprocessor defiitions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-15, David A­D­ Morano
	This code is newly written but modeled after some code that
	was originally written to get some standardized variable
	serialization for the Ethernet-Controller (ETHCON) development
	work.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	termcmd

	Description:
	This contains miscellaneious support subroutines.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"termcmd.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

extern int termcmd_clear(termcmd *op) noex {
	TERMCMD		*hop = op ;
    	int		rs = SR_FAULT ;
	if (op) {
	    rs = memclear(hop) ;
	    for (int i = 0 ; i < TERMCMD_NP ; i += 1) {
	        op->p[i] = TERMCMD_PEOL ;
	    } /* end for */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (termcmd_clear) */


