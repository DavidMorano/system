/* dircount SUPPORT */
/* encoding=ISO8859-1 */
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

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<fsdir.h>
#include	<hasx.h>
#include	<localmisc.h>

#include	"dircount.h"


/* local defines */


/* external subroutines */


/* external variables */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int dircount(cchar *dname) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (dname) {
	    rs = SR_FAULT ;
	    if (dname[0]) {
		char	*nbuf{} ;
		if ((rs = malloc_mn(&nbuf)) >= 0) {
	            fsdir	d ;
	            fsdir_ent	de ;
		    cint	nlen = rs ;
	            if ((rs = fsdir_open(&d,dname)) >= 0) {
	                while ((rs = fsdir_read(&d,&de,nbuf,nlen)) > 0) {
	                   if (hasNotDots(de.name,rs) > 0) {
		                c += 1 ;
		            } /* end (not dots) */
	                } /* end while */
	                rs1 = fsdir_close(&d) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (fsdir) */
		    rs1 = uc_free(nbuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (dircount) */


