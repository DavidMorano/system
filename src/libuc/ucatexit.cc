/* ucatexit SUPPORT */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Happily, unlike some (several) other middleware calls to
	the UNIX® system, this one is quite straight-forward.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<cerrno>
#include	<climits>
#include	<cstdlib>		/* |atexit(3c)| */
#include	<usystem.h>


/* local typedefs */

extern "C" {
    typedef void	(*atexit_f)(void) noex ;
}


/* local stuctures */

struct ucatexit ;

typedef int (ucatexit::*mem_f)() noex ;

struct ucatexit {
	mem_f		m ;
	atexit_f	func ;
	ucatexit(atexit_f f) noex : func(f) { } ;
	int stdatexit() noex ;
	int operator () () noex ;
} ; /* end struct (ucatexit) */


/* exported subroutines */

int uc_atexit(atexit_f f) noex {
	ucatexit	aeo(f) ;
	aeo.m = &ucatexit::stdatexit ;
	return aeo() ;
}
/* end subroutine (uc_atexit) */


/* local subroutines */

int ucatexit::operator () () noex {
	int	to_again = utimeout[uto_again] ;
	int	to_nomem = utimeout[uto_nomem] ;
	int	rs = SR_OK ;
	bool	f_exit = false ;
	repeat {
            if ((rs = (this->*m)()) < 0) {
                switch (rs) {
                case SR_AGAIN:
                    if (to_again-- > 0) {
                        msleep(1000) ;
                    } else {
                        f_exit = true ;
                    }
                    break ;
                case SR_NOMEM:
                    if (to_nomem-- > 0) {
                        msleep(1000) ;
                    } else {
                        f_exit = true ;
                    }
                    break ;
                case SR_INTR:
                    break ;
                default:
                    f_exit = true ;
                    break ;
                } /* end switch */
            } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;
	return rs ;
}
/* end subroutine (ucatexit::operator) */

int ucatexit::stdatexit() noex {
	int		rs = SR_FAULT ;
	if (func) {
	    rs = SR_OK ;
	    if (atexit(func) < 0) rs = (- errno) ;
	}
	return rs ;
}
/* end subroutine (ucatexit::stdatexit) */


