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
	int		cl = sl ;
	if (sp && rpp) {
	    rs = SR_OK ;
	    if (sstr) {
		cl = 0 ;
	    } else if (sch) {
		cl = 0 ;
	    } else {
		cl = sfnext(sp,sl,rpp) ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? cl : rs ;
}
/* end method (sif::next) */


