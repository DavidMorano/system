/* ucgetcwd SUPPORT */
/* lang=C++20 */

/* get the current working directory */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine returns the Current Working Directory (CWD).
	If you wanted the Present Working Directory (PWD), you
	should be calling |getpwd(3uc)|.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<cerrno>
#include	<cstring>		/* <- |strnlen(3c)| */
#include	<usystem.h>
#include	<usupport.h>
#include	<localmisc.h>


/* local defines */


/* imported namespaces */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct ucgetcwd ;
    typedef int (ucgetcwd::*mem_f)() noex ;
    struct ucgetcwd {
	mem_f		m ;
	char		*cwbuf ;
	int		cwlen ;
	ucgetcwd(char *b,int l) noex : cwbuf(b), cwlen(l) { } ;
	int stdgetcwd() noex ;
	int operator () () noex ;
    } ; /* end struct (ucgetcwd) */
}


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_getcwd(char *cwbuf,int cwlen) noex {
	int		rs = SR_FAULT ;
	if (cwbuf) {
	    ucgetcwd	aeo(cwbuf,cwlen) ;
	    aeo.m = &ucgetcwd::stdgetcwd ;
	    rs = aeo() ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_getcwd) */


/* local subroutines */

int ucgetcwd::operator () () noex {
	int		to_again = utimeout[uto_again] ;
	int		to_nomem = utimeout[uto_nomem] ;
	int		rs = SR_OK ;
	bool		f_exit = false ;
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
/* end subroutine (ucgetcwd::operator) */

int ucgetcwd::stdgetcwd() noex {
	int		rs ;
	if (getcwd(cwbuf,cwlen) != nullptr) {
	    rs = strnlen(cwbuf,cwlen) ;
	} else {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (ucgetcwd::stdgetcwd) */


