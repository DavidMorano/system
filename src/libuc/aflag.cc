/* aflag */
/* lang=C++20 */

/* atomic-flag */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
        This subroutine was originally written.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************* 
	Retrieve the numeric sign of a variable.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<atomic>
#include	<utypedefs.h>
#include	<clanguage.h>

#include	"aflag.hh"


/* local defines */


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */


/* exported subroutines */

aflag_co::operator bool () noex {
	bool		rf = false ;
	switch (w) {
	case aflagmem_set:
	    (void) op->af.test_and_set() ;
	    break ;
	case aflagmem_clear:
	    (void) op->af.clear() ;
	    break ;
	case aflagmem_test:
	    rf = op->af.test() ;
	    break ;
	case aflagmem_testandset:
	    rf = op->af.test_and_set() ;
	    break ;
	} /* end switch */
	return rf ;
}
/* end method (aflag_co::operator) */


