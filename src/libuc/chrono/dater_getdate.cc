/* dater_getdate SUPPORT */
/* encoding=ISO8859-1 */
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
 
	Name:
	dater_getdate

	Description:
	This subroutine is a method of the DATER object.  It creates
	a DATE object from the DATER object.

	Synopsis:
	int dater_getdate(dater *dp,date *dop) noex

	Arguments:
	dp	DATER object pointer
	dop	pointer to result DATE object

	Returns:
	>=0	OK
	<0	error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<date.h>
#include	<localmisc.h>		/* |NYEARS_CENTURY| + |TIMEBUFLEN| */

#include	"dater.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int dater_getdate(dater *op,date *dop) noex {
	int		rs ;
	if ((rs = dater_magic(op,dop)) >= 0) {
	    custime	t = op->b.time ;
	    cint	zoff = op->b.timezone ;
	    cint	isdst = op->b.dstflag ;
	    cchar	*zp = op->zname ;
	    rs = date_start(dop,t,zoff,isdst,zp,-1) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_getdate) */


