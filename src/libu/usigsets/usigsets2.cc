/* ucsigsets2 MODULE (module-implementation-unit) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* version %I% last-modified %G% */


/* revision history:

	= 2020-05-07, David A-D- Morano
	I converted this (formerly a header-only file) to a module.

*/

/* Copyright © 2020 David A­D­ Morano.  All rights reserved. */

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<pthread.h>		/* POSIX |pthread_setmask(3c)| */
#include	<cerrno>		/* CSTD */
#include	<csignal>		/* CSTD */
#include	<cstddef>		/* CSTD |nullptr_t| */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

module usigsets ;

/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */


/* local subroutines */

usigset::usigset() noex {
    (void) usigseter.empty(&ss) ;
} /* end ctor) */

usigset::usigset(int sn) noex {
    (void) usigseter.empty(&ss) ;
    usigseter.add(&ss,sn) ;
} /* end ctor) */

int usigset::add(int sn) noex {
    return usigseter.add(&ss,sn) ;
} /* end method */

int usigset::del(int sn) noex {
    return usigseter.del(&ss,sn) ;
} /* end method */

int usigset::all() noex {
    return usigseter.fill(&ss) ;
} /* end method */

int usigset::none() noex {
    return usigseter.empty(&ss) ;
} /* end method */

int usigset::is(int sn) noex {
    return usigseter.ismem(&ss,sn) ;
} /* end method */

int usigset::operator +=	(int sn) noex {
    return usigseter.add(&ss,sn) ;
} /* end method */

int usigset::operator -=	(int sn) noex {
    return usigseter.del(&ss,sn) ;
} /* end method */

int usigset::operator <<	(int sn) noex {
    return usigseter.add(&ss,sn) ;
} /* end method */

int usigset::operator >>	(int sn) noex {
    return usigseter.del(&ss,sn) ;
} /* end method */


