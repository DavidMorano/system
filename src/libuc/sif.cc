/* sif SUPPORT */
/* lang=C++20 */

/* string-interate-find object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************* 
	These subroutines perform character case conversions
	(cheaply).  Yes, case-conversion is used a lot in embedded
	systems!

	Implementation note:

	The |CHAR(3dam)| translations (conversions) return an
	unsigned character ('uchar') type.  This means that when
	cajoled into becoming an 'int' type on return, it will not
	be sign-extended.

	= 2014-01-13
	We now use the |CHAR(3dam)| translation tables only (no
	computed conversion compile-time option).  Deal with it.
	See my note above under the revision history.  Look how
	trivial the code is now!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<usysrets.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<sfx.h>
#include	<localmisc.h>

#include	"sif.hh"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */


/* local subroutines */

int sif::next(cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		rl = sl ;
	cchar		*rp = nullptr ;
	if (sp && rpp) {
	    rs = SR_OK ;
	    if (sstr) {
		rl = 0 ;
	    } else if (sch) {
		rl = 0 ;
	    } else {
		if ((rl = sfnext(sp,sl,&rp)) > 0) {
		    sl -= ((rp + rl) - sp) ;
		    sp = (rp + rl) ;
		}
	    }
	    *rpp = rp ;
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end method (sif::next) */


