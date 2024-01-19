/* aflag SUPPORT */
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
#include	<usystem.h>

#include	"aflag.hh"


/* local defines */


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */


/* exported subroutines */

bool aflag_co::operator () () noex {
	bool		rf = false ;
	switch (w) {
	case aflagmem_set:
	    rf = op->af.test(memord_relaxed) ;
	    (void) op->af.test_and_set() ;
	    break ;
	case aflagmem_clear:
	    rf = op->af.test(memord_relaxed) ;
	    (void) op->af.clear() ;
	    break ;
	case aflagmem_test:
	    rf = op->af.test(memord_relaxed) ;
	    break ;
	case aflagmem_testandset:
	    rf = op->af.test_and_set() ;
	    break ;
	case aflagmem_wait:
	    rf = op->af.test(memord_relaxed) ;
	    op->af.wait(false,memord_acquire) ;
	    break ;
	case aflagmem_notify:
	    rf = op->af.test(memord_relaxed) ;
	    op->af.notify_one() ;
	    break ;
	case aflagmem_notifyall:
	    rf = op->af.test(memord_relaxed) ;
	    op->af.notify_all() ;
	    break ;
	} /* end switch */
	return rf ;
}
/* end method (aflag_co::operator) */

bool aflag_co::operator () (bool a) noex {
	bool		rf = false ;
	switch (w) {
	case aflagmem_set:
	case aflagmem_clear:
	case aflagmem_test:
	case aflagmem_testandset:
	case aflagmem_wait:
	    rf = (*op) ;
	    break ;
	case aflagmem_notify:
	    rf = op->af.test(memord_relaxed) ;
	    (*op) = a ;
	    op->af.notify_one() ;
	    break ;
	case aflagmem_notifyall:
	    rf = op->af.test(memord_relaxed) ;
	    (*op) = a ;
	    op->af.notify_all() ;
	    break ;
	} /* end switch */
	return rf ;
}
/* end method (aflag_co::operator) */


