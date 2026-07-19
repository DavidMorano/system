/* passfd SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* subroutine to pass a file-descriptor to a file */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Description:
	Pass a file-descritpr to a file in the file-system.  This
	subroutine passes the given file-descriptor to a file
	(specified by its file-path).

	Synopsis:
	int passfd(cchar *fname,int fd) noex

	Arguments:
	fname		path to UNIX® file (pipe-FIFO) to pass FD to
	fd		file-descriptor to pass

	Returns:
	<0		error in dialing
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<ucfileop.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"passfd.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int passfd(cchar *fname,int fd) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rv = 0 ;
	if (fname) ylikely {
	    rs = SR_INVALID ;
	    if (fname[0]) ylikely {
		rs = SR_BADFD ;
	        if (fd >= 0) ylikely {
		    cint	of = (O_WRONLY) ;
		    if ((rs = uc_open(fname,of,0666)) >= 0) ylikely {
	    	        cint	pfd = rs ;
			{
	    	            rs = uc_fpassfd(pfd,fd) ;
			    rv = rs ;
			}
	    	        rs1 = uc_close(pfd) ;
			if (rs >= 0) rs = rs1 ;
		    } /* end if (open) */
	        } /* end if (good-FD) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rv : rs ;
} /* end subroutine (passfd) */


