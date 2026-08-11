/* fileprintable SUPPORT */
/* charset=ISO8859-1 */
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
	fileprintable

	Description:
	This subroutine checks if the specified file is totally printable.

	Synopsis:
	int fileprintable(cchar *fname) noex

	Arguments:
	fname		file to check

	Returns:
	>=0		length of return organization string
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>		/* POSIX */
#include	<fcntl.h>		/* POSIX */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<filer.h>		/* LIBUC */
#include	<rmx.h>			/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<hasx.h>		/* LIBUC |hasprintbad(3uc)| */
#include	<localmisc.h>		/* LIBU */

#include	"fileprintable.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

cint		bsz		= 512 ; /* buffer size */


/* exported variables */


/* exported subroutines */

int fileprintable(cchar *fname) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		f = false ;
	if (fname) ylikely {
	    rs = SR_INVALID ;
	    if (fname[0]) ylikely {
		if (char *lbuf ; (rs = lm_ml(&lbuf)) >= 0) ylikely {
		    cint	llen = rs ;
		    cint	of = O_RDONLY ;
		    cmode	om = 0 ;
	            if ((rs = uc_open(fname,of,om)) >= 0) ylikely {
	                cint	fd = rs ;
	                if (filer b ; (rs = b.start(fd,0z,bsz)) >= 0) ylikely {
	                    while ((rs = b.readln(lbuf,llen)) > 0) {
	                        cint	ll = rmeol(lbuf,rs) ;
		    		f = hasprintbad(lbuf,ll) ;
		    		if (f) break ;
	        	    } /* end while (reading lines) */
	                    rs1 = b.finish ;
		            if (rs >= 0) rs = rs1 ;
	                } /* end if (filer) */
	                rs1 = uc_close(fd) ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (open) */
		    rs1 = lm_free(lbuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? int(!f) : rs ;
} /* end subroutine (fileprintable) */


