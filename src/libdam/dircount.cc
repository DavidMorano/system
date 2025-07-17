/* dircount SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* return the count of (non-overhead) files in the given directory */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	dircount

	Description:
	We return the count of non-overhead entries in the given
	directory.

	Synopsis:
	int dircount(cchar *dname) noex

	Arguments:
	dname		directory name (as a string)

	Returns:
	>=		count of files
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<fsdir.h>
#include	<hasx.h>
#include	<localmisc.h>

#include	"dircount.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int dircounts(cchar *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int dircount(cchar *dname) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (dname) {
	    rs = SR_FAULT ;
	    if (dname[0]) {
		rs = dircounts(dname) ;
		c = rs ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (dircount) */


/* local subroutines */

static int dircounts(cchar *dname) noex {
    	int		rs ;
	int		rs1 ;
	int		c = 0 ;
        if (char *nbuf ; (rs = malloc_mn(&nbuf)) >= 0) {
            cint        nlen = rs ;
            if (fsdir d ; (rs = d.open(dname)) >= 0) {
		for (fsdir_ent de ; (rs = d.read(&de,nbuf,nlen)) > 0 ; ) {
                    if (hasNotDots(nbuf,rs) > 0) {
                        c += 1 ;
                    } /* end (not dots) */
                } /* end for */
                rs1 = d.close ;
                if (rs >= 0) rs = rs1 ;
            } /* end if (fsdir) */
            rs1 = uc_free(nbuf) ;
            if (rs >= 0) rs = rs1 ;
        } /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (dircounts) */


