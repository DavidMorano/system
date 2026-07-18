/* fmtsub2 MODULE (implementation) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutine to format string output */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	Of course, this subroutine was inspired by the UNIX®
	equivalent, but this is my own version for a) when I do not
	have the UNIX® libraries around, and b) to customize it to
	what I want!

	= 2020-02-15, David A­D­ Morano
	I modularized (w/ C++20 modules) this code.

*/

/* Copyright © 1998,2020 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	fmtsub

	Description:
	This (FMTSUB) is a helper object for the FMTSTR facility.

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdarg>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"fmtopts.h"
#include	"fmtutil.hh"
#include	"fmtsub.hh"

module fmtsub ;

import fmtutil ;

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* local structures */


/* forward references */


/* forward refernces */


/* local variables */

local constexpr fmtoptms	fopt ;


/* exported variables */


/* exported subroutines */

int fmtsub_start(fmtsub *op,char *ubuf,int ulen,int fm) noex {
	int		rs = SR_FAULT ;
	if (op && ubuf) ylikely {
	    op->ubuf = ubuf ;
	    op->ulen = ulen ;
	    op->mode = fm ;
	    op->numbase = 0 ;
	    {
	        op->fl.mclean	= !!(fm & fopt.clean) ;
	        op->fl.mnooverr	= !!(fm & fopt.nooverr) ;
	        op->fl.mminfill = !!(fm & fopt.minfill) ;
	    }
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (fmtsub_start) */

int fmtsub_finish(fmtsub *op) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op) ylikely {
	    rs = SR_NOTOPEN ;
	    if (op->ubuf) {
	        len = op->len ;
	        op->ubuf[len] = '\0' ;
	        if (op->fl.ov) {
	            if (! op->fl.mnooverr) rs = SR_OVERFLOW ;
	        }
		op->ubuf = nullptr ;
		rs = SR_OK ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (fmtsub_finish) */

int fmtsub_reserve(fmtsub *op,int n) noex {
	int		rs = SR_OVERFLOW ;
	if (! op->fl.ov) {
	    cint	rlen = (op->ulen - op->len) ;
	    rs = SR_OK ;
	    if (n > rlen) {
	        op->fl.ov = true ;
	    }
	} /* end if (not overflow) */
	return rs ;
} /* end subroutine (fmtsub_reserve) */

int fmtsub_audit(fmtsub *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	}
	return rs ;
} /* end subroutine (fmtsub_audit) */


