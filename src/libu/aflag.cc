/* aflag SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* atomic-flag */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
        This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object provides an atomic flag that can be used for a 
	variety of purposes.  Two of the more populate purposes is
	to provide for:
	1, executing a sequence of code only once.
	2. implementing a lghtweight mutual-exclusion semaphore.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<atomic>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<usupport.h>
#include	<timewatch.hh>		/* |timewatch(3u)| */

#include	"aflag.hh"


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
	    rf = (*this) ;
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

int aflag_mx::operator () (int to) noex {
	int		rs = SR_BUGCHECK ;
	switch (w) {
	case aflagmx_lockbegin:
	    rs = op->ilockbegin(to) ;
	    break ;
	case aflagmx_lockend:
	    rs = op->ilockend() ;
	    break ;
	} /* end switch */
	return rs ;
}
/* end method (aflag_mx::operator) */

int aflag::ilockbegin(int to) noex {
	int		rs = SR_OK ;
	if (to >= 0) {
	    timewatch	tw(to) ;
	    auto lamb = [this] () -> int {
	        return int(! af.test_and_set(memord_acquire)) ;
	    } ; /* end lambda (lamb) */
	    rs = tw(lamb) ;		/* <- time-watching occurs in there */
	} else {
	    while (af.test_and_set(memord_acquire) == true) {
		af.wait(true,memord_relaxed) ;
	    }
	}
	return rs ;
}
/* end method (aflag::ilockbegin) */

int aflag::ilockend() noex {
	af.clear(memord_release) ;
	af.notify_one() ;
	return SR_OK ;
}
/* end method (aflag::ilockend) */


