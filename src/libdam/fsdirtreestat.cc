/* fsdirtreestat SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get status on a file */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debugging */

/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	fsdirtreestat

	Description
	This subroutine provides an independent stand-alone STAT function
	for the FSDIRTREE object.

	Synopsis:
	int fsdirtreestat(cchar *fname,int type,ustat *sbp) noex

	Arguments:
	fname		file-name to STAT
	type		type of STAT: 0=regular, 1=lstat
	sbp		ponter to a STAT block structure

	Returns:
	=>0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<uclibsubs.h>
#include	<localmisc.h>

#include	"fsdirtreestat.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int fsdirtreestat(cchar *fname,int type,ustat *sbp) noex {
	int		rs = SR_FAULT ;
	if (fname && sbp) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	        if (type == 0) {
	            rs = uc_stat(fname,sbp) ;
	        } else {
	            rs = uc_lstat(fname,sbp) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (fsdirtreestat) */


