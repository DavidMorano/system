/* fileliner SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* determine the number of lines in a file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	fileliner

	Description:
	Given a file-name we determine the number of lines the file
	has.  An optional comment character can be specified.  When
	a comment character is specified, only lines with some
	content located before the comment character will count as
	a line.  Note that a comment characters is indeed just a
	single characters, so things like specifying C-language
	comments are not possible.

	Synopsis:
	int fileliner(cchar *fname,int cc) noex

	Arguments:
	fname		file-path to check
	cc		optional comment character (when non-zero)

	Returns:
	<0		error
	>=0		number of lines in the file

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<bfile.h>
#include	<six.h>
#include	<rmx.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"fileliner.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int fileliner(cchar *fname,int cn) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		n = 0 ;
	if (fname) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	        if (char *lbuf{} ; (rs = malloc_ml(&lbuf)) >= 0) {
	    	    bfile	ifile, *ifp = &ifile ;
		    cint	llen = rs ;
	            if ((rs = bopen(ifp,fname,"r",0666)) >= 0) {
	                while ((rs = breadln(ifp,lbuf,llen)) > 0) {
		            if (cn > 0) {
		                if (cint ll = rmeol(lbuf,rs) ; ll > 0) {
				    auto sw = siskipwhite ;
			            if (int si ; (si = sw(lbuf,ll)) >= 0) {
				        if (ll > si) {
				            if (lbuf[si] != cn) n += 1 ;
				        }
			            }
			        }
		            } else {
		                n += 1 ;
		            }
	                } /* end while */
	                rs1 = bclose(ifp) ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (bfile) */
	            rs1 = uc_free(lbuf) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (filelines) */


