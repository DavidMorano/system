/* mktmpfile SUPPORT */
/* lang=C++20 */

/* make a temporary file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mktmpfile

	Description:
	Make a temporary file.

	Synopsis:
	int mktmpfile(char *rbuf,cchar *template,mode_t fm) noex

	Arguments:
	- rbuf		result buffer pointer
	- template	input file name template string
	- fm		file-creation mode

	Returns:
	>=0	success and length of created file name
	<0	error (system-return)

	Notes:
	Q. What is with |uc_forklock(3uc)|?
	A. We try to minimize child processes getting an extra
	(unknown to it) file-descriptor.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstdlib>
#include	<cstring>		/* for |strlen(3c)| */
#include	<usystem.h>
#include	<localmisc.h>

#include	"mkfile.h"


/* local defines */


/* external subroutines */

extern "C" {
    extern int	opentmpfile(cchar *,int,mode_t,char *) noex ;
}


/* external variables */


/* forward reference */


/* local variables */


/* exported subroutines */

int mktmpfile(char *rbuf,cchar *inname,mode_t om) noex {
	cint		of = (O_WRONLY|O_CLOEXEC) ;
	int		rs ;
	if ((rs = opentmpfile(inname,of,om,rbuf)) >= 0) {
	    uc_close(rs) ;
	    rs = strlen(rbuf) ;
	} /* end if (opentmpfile) */
	return rs ;
}
/* end subroutine (mktmpfile) */


