/* bwritefile SUPPORT */
/* lang=C++20 */

/* copy a file to another file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	bwritefile

	Description:
	This subroutine copies the contents of a given file to the
	current file.

	Synospsis:
	int bwritefile(bfile *op,cchar *fname) noex

	Arguments:
	op		output file pointer to copy to
	fname		the file (which will be read) to copy to the current

	Returns:
	>=0		length of data copied
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<localmisc.h>

#include	"bfile.h"


/* local defines */


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int bwritefile(bfile *op,cchar *fname) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		wlen = 0 ;
	if ((rs = bfile_magic(op,fname)) > 0) {
	    bfile	ifile, *ifp = &ifile ;
	    cmode	om = 0664 ;
	    if ((rs = bopen(ifp,fname,"r",om)) >= 0) {
		{
		    rs = bwriteblock(op,ifp,-1) ;
		    wlen = rs ;
		}
		rs1 = bclose(ifp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (bfile) */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (bwritefile) */


