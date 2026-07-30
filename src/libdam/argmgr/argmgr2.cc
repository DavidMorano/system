/* argmgr2 MODULE */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* manage program arguments */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

	= 2020-08-13, David A­D­ Morano
	This code has been modularized (w/ C++20 modules).

*/

/* Copyright © 1998,2020 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	argmgr

	Description:
	This object manages program arguments.

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"argmgr.h"

#pragma		GCC dependency		"mod/deb.ccm"

module argmgr ;

import deb ;

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


/* local subroutines */

argmgr_iter argmgr::begin() noex {
    	argmgr_iter	res(this,0) ;
	int		rs ;
	DEBPR("ent\n") ;
	if (cchar *ap = nullptr ; (rs = get(1,&ap)) > 0) {
	    {
		cchar *fmt = "rs=%d ap=%s\n" ;
	        DEBPR(fmt,rs,((ap) ? "ok" : "null")) ;
	    }
	    if (ap) res.ai = rs ;
	} /* end if (get) */
	DEBPR("ret rs=%d\n",rs) ;
	return res ;
} /* end method (argmgr::begin) */

argmgr_iter argmgr::end() noex {
    	argmgr_iter	res(this,argc) ;
	return res ;
} /* end method (argmgr::end) */


