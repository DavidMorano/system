/* parts SUPPORT */
/* lang=C++20 */

/* E-Mail Address (parts) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This is an email address handling module object.  It can
	parse out and store hierarchically organized EMAs.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>		/* for |strlen(3c)| */
#include	<usystem.h>
#include	<char.h>
#include	<localmisc.h>

#include	"asstr.hh"
#include	"parts.hh"


/* local defines */


/* imported namespaces */

using namespace::emaobj ;		/* namespace */
using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* external subroutines */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

namespace emaobj {

    int partsbegin(asstr *asp) noex {
	int		rs = SR_FAULT ;
	if (asp) {
	    rs = SR_OK ;
	    for (int i = 0 ; i < si_overlast ; i += 1) {
	        ass_start(asp + i) ;
	    }
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (partsbegin) */

    int partslen(asstr *asp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (asp) {
	    rs = SR_OK ;
	    for (int i = 0 ; i < si_overlast ; i += 1) {
	        ass_backwhite(asp+i) ;
	        rs1 = ass_len(asp+i) ;
	        if (rs1 >= 0) len += rs1 ;
	    } /* end for */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
    } /* end subroutine (partslen) */

    int partsend(asstr *asp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (asp) {
	    rs = SR_OK ;
	    for (int i = 0 ; i < si_overlast ; i += 1) {
	        rs1 = ass_finish(asp + i) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (partsend) */

}


