/* ucwritefile SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* copy from one file descriptor to another */


/* revision history:

	= 1998-01-10, David A­D­ Morano
	The subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	uc_writefile

	Description:
	This subroutine copies data from the specified file to the
	output file descriptor.

	Synopsis:
	int uc_writefile(int ofd,cchar *fname) noex

	Arguments:
	ofd		source file descriptor
	fname		file to write to output file-descriptor

	Returns:
	>=0		length copied
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<fcntl.h>		/* |O_{xx}| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_writefile(int ofd,cchar *fname) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
	if (fname) ylikely {
	    rs = SR_BADF ;
	    if (ofd >= 0) ylikely {
	        rs = SR_INVALID ;
		if (fname[0]) ylikely {
		    cint	of = O_RDONLY ;
		    cmode	om = 0 ;
		    if ((rs = uc_open(fname,of,om)) >= 0) ylikely {
	    	        cint	ifd = rs ;
	    	        {
			    rs = uc_writedesc(ofd,ifd,-1) ;
			    len = rs ;
 	    	        }
	    	        rs1 = u_close(ifd) ;
		        if (rs >= 0) rs = rs1 ;
		    } /* end if (open-file) */
	        } /* end if (valid) */
	    } /* end if (valid-FD) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (uc_writefile) */


