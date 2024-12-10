/* calcite SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* calendar citation */
/* version %I% last-modified %G% */


/* revision history:

	- 2008-10-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	calcite

	Description:
	This small object just holds a citation for a calendar entry.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>

#include	"calcite.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* exported variables */


/* local variables */


/* exported variables */


/* exported subroutines */

int calcite_load(calcite *ep,int y,int m, int d) noex {
	int		rs = SR_FAULT ;
	if (ep) {
	    rs = memclear(ep) ;
	    ep->y = ushort(y) ;
	    ep->m = uchar(m) ;
	    ep->d = uchar(d) ;
	}
	return rs ;
}
/* end subroutine (calcite_load) */


