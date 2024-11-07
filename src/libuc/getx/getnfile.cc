/* getnfile SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* get N-Files (available file-descriptors in UNIX®) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	getnfile

	Description:
	Here we get the number of file-descriptors that are available
	to the current process.  This will be the so-called "soft"
	limit of the number of descriptors.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/resource.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>		/* |NOFILE| */

#include	"getnfile.h"


/* local defines */

#ifndef	GETNFILE_MAXFD
#define	GETNFILE_MAXFD	2048	/* our fake maximum limit */
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int getnfile(int w) noex {
	cint		cmd = RLIMIT_NOFILE ;
	int		rs ;
	int		nf = 0 ;
	if (RLIMIT lim ; (rs = u_getrlimit(cmd,&lim)) >= 0) {
	    switch (w) {
	    case 0:
	        nf = int(lim.rlim_cur) ;
		break ;
	    case 1:
	        nf = int(lim.rlim_max) ;
		break ;
	    case 2:
		nf = NOFILE ;
		break ;
	    default:
		rs = SR_INVALID ;
		break ;
	    } /* end switch */
	    if (rs >= 0) {
  	        if (nf == RLIM_INFINITY) {
		    nf = GETNFILE_MAXFD ;
	        } else if (nf < 0) {
		    nf = NOFILE ;
	        }
	    } /* end if (ok) */
	} /* end if (u_getrlimit) */
	return (rs >= 0) ? nf : rs ;
}
/* end subroutine (getnfile) */


