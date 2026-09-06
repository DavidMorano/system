/* ema_mag1 MODULE (implementation) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* E-Mail Address */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	ema

	Description:
	This is an email address handling module object.  It can
	parse out and store hierarchically organized EMAs.

*******************************************************************************/

module ;

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC |libmem(3uc)| */
#include	<vechand.h>		/* LIBUC */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"ema.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"ema_entry.ccm"
#pragma		GCC dependency		"ema_asstr.ccm"
#pragma		GCC dependency		"ema_parts.ccm"

module ema_mag ;

import libutil ;			/* |getlenstr(3u)| */

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

    int ema_starter(ema *op,cchar *sp,int sl) noex {
	int		rs ;
	if ((rs = ema_start(op)) >= 0) ylikely {
	    if (sp) {
	        rs = ema_parse(op,sp,sl) ;
	    } /* end if (optional data) */
	    if (rs < 0) {
	        ema_finish(op) ;
	    }
	} /* end if (ema_start) */
	return rs ;
   } /* end subroutine (ema_starter) */


