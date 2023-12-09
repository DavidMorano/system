/* uc_ztime */
/* lang=C20 */

/* interface component for UNIX® library-3c */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<usystem.h>


/* exported subroutines */

int uc_ztime(struct tm *tsp,const time_t *tp,int z) noex {
	int	rs ;
	if (z) {
	    rs = uc_localtime(tp,tsp) ;
	} else {
	    rs = uc_gmtime(tp,tsp) ;
	}
	return rs ;
}
/* end subroutine (uc_ztime) */


