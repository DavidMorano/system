/* getrunlevel SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* return the run-level of the system */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-18, David A­D­ Morano
	This subroutine was written to simplify getting the current
	"run-level" of the system.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getrunlevel

	Description:
	We use the UTMPX database to query for the "run-level" of
	the system.  If the UTMPX file is not found, we return
	SR_NOENT.  If we cannot access it we return SR_ACCESS.  If
	the file is found but there is no "run-level" record in it,
	we return zero (0).

	Synopsis:

	int getrunlevel(cchar *utmpxfname) noex

	Arguments:
	utmpxfname	UTMPX filename

	Returns:
	>=0	run-level (including '0' meaning no record found)
	<0	one of: SR_NOENT, SR_ACCESS, other means some bad happened

	Notes: Why some subroutine like this was not provided by
	the UNIX® developers themselves is a mystery.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<utmpacc.h>
#include	<tmpx.h>
#include	<localmisc.h>

#include	"getrunlevel.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	getrunlevel_tmpx(cchar *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int getrunlevel(cchar *utmpfname) noex {
	int		rs ;
	if (utmpfname) {
	    rs = getrunlevel_tmpx(utmpfname) ;
	} else {
	    rs = utmpacc_runlevel() ;
	}
	return rs ;
}
/* end subroutine (getrunlevel) */


/* local subroutines */

static int getrunlevel_tmpx(cchar *utmpfname) noex {
    	cint		of = O_RDONLY ;
	int		rs ;
	int		rs1 ;
	int		n = 0 ;
	if (tmpx ut ; (rs = tmpx_open(&ut,utmpfname,of)) >= 0) {
	    {
	        rs = tmpx_getrunlevel(&ut) ;
	        n = rs ;
	    }
	    rs1 = tmpx_close(&ut) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (tmpx) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (getrunlevel) */


