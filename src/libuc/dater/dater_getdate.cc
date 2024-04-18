/* dater_getdate SUPPORT */
/* lang=C++20 */

/* get a DATE object out of a DATER object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	Originally created due to frustration with various other
	"fuzzy" date conversion subroutines.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************
 
	This subroutine is a method of the DATER object. It creates
	a DATE object from the DATER object.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>		/* |NYEARS_CENTURY| + |TIMEBUFLEN| */

#include	"dater.h"
#include	"date.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int dater_getdate(dater *dp,date *dop) noex {
	int		rs ;
	if ((rs = dater_magic(dp,dop)) >= 0) {
	    rs = SR_NOTOPEN ;
	    if (dp->magic == DATER_MAGIC) {
	        time_t	t = dp->b.time ;
	        int	zoff = dp->b.timezone ;
	        int	isdst = dp->b.dstflag ;
	        int	zl = DATER_ZNAMELEN ;
	        cchar	*zp = dp->zname ;
	        rs = date_start(dop,t,zoff,isdst,zp,zl) ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_getdate) */


