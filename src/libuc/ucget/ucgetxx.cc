/* ucgetxx SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* UNIX® C-language system database access (UCGET) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written for Rightcore Network
	Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ucgeter

	Description:
	This is a (error timer) support object for use by the UCGET
	sub-group of the U library.

	Synopsis:

	Arguments:

	Returns:

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<errtimer.hh>
#include	<localmisc.h>

#include	"ucgetxx.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* local variables */


/* exported variables */


/* exported subroutines */

int ucgeter::errhandle(int rs) noex {
        if (rs < 0) {
            reterr	r(rs) ; /* <- default causes exit */
	    fexit = false ;
            switch (rs) {
            case SR_AGAIN:
                r = to_again(rs) ;
                break ;
            case SR_BUSY:
                r = to_busy(rs) ;
                break ;
            case SR_NOMEM:
                r = to_nomem(rs) ;
                break ;
            case SR_NOSR:
                r = to_nosr(rs) ;
                break ;
            case SR_NOBUFS:
                r = to_nobufs(rs) ;
                break ;
            case SR_MFILE:
                r = to_mfile(rs) ;
                break ;
            case SR_NFILE:
                r = to_nfile(rs) ;
                break ;
            case SR_NOLCK:
                r = to_nolck(rs) ;
                break ;
            case SR_NOSPC:
                r = to_nospc(rs) ;
                break ;
            case SR_DQUOT:
                r = to_dquot(rs) ;
                break ;
            case SR_IO:
                r = to_io(rs) ;
                break ;
            case SR_INPROGRESS:
                break ;
            case SR_INTR:
                r(false) ;
                break ;
            } /* end switch */
	    rs = r ;
            fexit = r.fexit ;
        } /* end if (error) */
	return rs ;
}
/* end method (ucgeter::errhandle) */


