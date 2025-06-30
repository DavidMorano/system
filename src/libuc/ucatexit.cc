/* ucatexit SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface components for UNIX® library-3c */
/* process exit handling */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	ucatæxit

	Description:
	Happily, unlike some (several) other middleware calls to
	the UNIX® system, this one is quite straight-forward.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |atexit(3c)| */
#include	<usystem.h>
#include	<errtimer.hh>
#include	<localmisc.h>


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local stuctures */

namespace {
    struct ucatexit ;
    typedef int (ucatexit::*mem_m)() noex ;
    struct ucatexit {
	mem_m		m ;
	void_f	func ;
	ucatexit(void_f f) noex : func(f) { } ;
	int stdatexit() noex ;
	int operator () () noex ;
    } ; /* end struct (ucatexit) */
} /* end namespace */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_atexit(void_f func) noex {
    	int		rs = SR_FAULT ;
	if (func) {
	    ucatexit	aeo(func) ;
	    aeo.m = &ucatexit::stdatexit ;
	    rs = aeo() ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_atexit) */


/* local subroutines */

int ucatexit::operator () () noex {
	errtimer	to_again = utimeout[uto_again] ;
	errtimer	to_nomem = utimeout[uto_nomem] ;
	reterr		r ;
	int		rs ;
	repeat {
            if ((rs = (this->*m)()) < 0) {
		r(rs) ;
                switch (rs) {
                case SR_AGAIN:
		    r = to_again(rs) ;
                    break ;
                case SR_NOMEM:
		    r = to_nomem(rs) ;
                    break ;
                case SR_INTR:
		    r(false) ;
                    break ;
                } /* end switch */
            } /* end if (error) */
	} until ((rs >= 0) || r.fexit) ;
	return rs ;
}
/* end subroutine (ucatexit::operator) */

int ucatexit::stdatexit() noex {
	int		rs ;
	if ((rs = atexit(func)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (ucatexit::stdatexit) */


