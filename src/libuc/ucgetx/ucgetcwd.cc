/* ucgetcwd SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
/* get the current working directory */
/* version %I% last-modified %G% */

#define	CF_UGETCWD	1		/* use |ugetcwd(3u)| */

/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	uc_getcwd

	Description:
	This subroutine returns the Current Working Directory (CWD).
	If you wanted the Present Working Directory (PWD), you
	should be calling |getcwd(3uc)|.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<usupport.h>
#include	<errtimer.hh>
#include	<localmisc.h>

import libutil ;
import usysbasic ;

/* local defines */

#ifndef	CF_UGETCWD
#define	CF_UGETCWD	1		/* use |ugetcwd(3u)| */
#endif


/* imported namespaces */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct ucgetcwd ;
    typedef int (ucgetcwd::*mem_m)() noex ;
    struct ucgetcwd {
	mem_m		m ;
	char		*cwbuf ;
	int		cwlen ;
	ucgetcwd(char *b,int l) noex : cwbuf(b), cwlen(l) { } ;
	int stdgetcwd() noex ;
	operator int () noex ;
    } ; /* end struct (ucgetcwd) */
}


/* forward references */


/* local variables */

cbool		f_ugetcwd = bool(CF_UGETCWD) ;


/* exported variables */


/* exported subroutines */

int uc_getcwd(char *cwbuf,int cwlen) noex {
	int		rs = SR_FAULT ;
	if (cwbuf) {
	    if_constexpr (f_ugetcwd) {
		rs = ugetcwd(cwbuf,cwlen) ;
	    } else {
	        ucgetcwd aeo(cwbuf,cwlen) ;
	        aeo.m = &ucgetcwd::stdgetcwd ;
	        rs = aeo ;
	    } /* end if_constexpr (f_ugetcwd) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_getcwd) */


/* local subroutines */

ucgetcwd::operator int () noex {
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
/* end subroutine (ucgetcwd::operator) */

int ucgetcwd::stdgetcwd() noex {
	int		rs ;
	if (cchar *rp ; (rp = getcwd(cwbuf,(cwlen+1))) != nullptr) {
	    rs = lenstr(rp,cwlen) ;
	} else {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (ucgetcwd::stdgetcwd) */


