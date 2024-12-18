/* mktmpdir SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* make the program-specific temporary directory */
/* version %I% last-modified %G% */


/* revision history:

	= 1991-09-01, David A­D­ Morano
	This subroutine was adopted from the DWD program.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*****************************************************************************

	Name:
	mktmpdir

	Description:
	Create a temporary directory (someplace).

	Synopsis:
	int mktmpdir(char *rbuf,cchar *dname,mode_t fm) noex

	Arguments:
	rbuf		result buffer pointer (should be MAXPATHLEN+1 sized)
	dname		directory to create directory in
	fm		file-mode to create

	Returns:
	>=0		length of result in bytes
	<0		error (system-return)

*****************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<ctime>
#include	<usystem.h>
#include	<mkfile.h>
#include	<localmisc.h>

#include	"mktmp.h"


/* local defines */


/* external subroutines */


/* forward references */


/* exported subroutines */

int mktmpdir(char *rbuf,cchar *dname,mode_t om) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ;
	if (rbuf && dname) {
	    om |= S_IFDIR ;
	    if ((rs = mktmpfile(rbuf,dname,om)) >= 0) {
		rl = rs ;
		rs = uc_minmod(rbuf,om) ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (mktmpdir) */


