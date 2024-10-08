/* fileisprint SUPPORT */
/* lang=C++20 */

/* is the specified file totally printable? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.


*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	fileisprint

	Description:
	This subroutine checks if the specified file is totally printable.

	Synopsis:
	int fileisprint(cchar *fname) noex

	Arguments:
	fname		file to check

	Returns:
	>=0		length of return organization string
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<linebuffer.h>
#include	<filer.h>
#include	<rmx.h>
#include	<char.h>
#include	<hasx.h>
#include	<localmisc.h>

#include	"fileisprint.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int fileisprint(cchar *fname) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		f = false ;
	if (fname) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
		if (linebuffer lb ; (rs = lb.start) >= 0) {
		    cint	to = -1 ;
		    cint	of = O_RDONLY ;
		    cmode	om = 0 ;
	            if ((rs = uc_open(fname,of,om)) >= 0) {
	    	        filer	b ;
	                int	fd = rs ;
	                if ((rs = b.start(fd,0z,512,0)) >= 0) {
	                    while ((rs = b.readln(lb.lbuf,lb.llen,to)) > 0) {
	                        int	ll = rmeol(lb.lbuf,rs) ;
		    		f = hasprintbad(lb.lbuf,ll) ;
		    		if (f) break ;
	        	    } /* end while (reading lines) */
	                    rs1 = b.finish ;
		            if (rs >= 0) rs = rs1 ;
	                } /* end if (filer) */
	                rs1 = u_close(fd) ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (open) */
		    rs1 = lb.finish ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (linebuffer) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? (!f) : rs ;
}
/* end subroutine (fileisprint) */


