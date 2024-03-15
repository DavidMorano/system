/* mapex SUPPORT */
/* lang=C++20 */

/* map status-return (SR) values to program exit-codes (EX) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<exitcodes.h>
#include	<localmisc.h>

#include	"mapex.h"


/* local defines */


/* exported variables */


/* exported subroutines */

int mapex(const MAPEX *mapexs,int rs) noex {
	int		ex = EX_UNKNOWN ;
	if (mapexs) {
	    ex = EX_OK ;
	    if (rs < 0) {
	        int	i ;
	        for (i = 0 ; mapexs[i].rs ; i += 1) {
	            if (mapexs[i].rs == rs) break ;
	        } /* end for */
	        ex = (mapexs[i].rs) ? mapexs[i].ex : EX_UNKNOWN ;
	    } /* end if */
	} /* end if */
	return ex ;
}
/* end subroutine (mapex) */


