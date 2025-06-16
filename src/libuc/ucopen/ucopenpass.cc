/* ucopenpass SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* connect to a local program */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debugging */

/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	uc_openpass

	Description:
	This is a dialer to connect to a local program.

	Synopsis:
	int uc_openpass(cc *passfname,int oflags,int timeout,int opts) noex

	Arguments:
	passfname	the pass filename
	oflags		options to specify read-only or write-only
	timeout		time-out
	opts		options

	Returns:
	>=0		file descriptor to program STDIN and STDOUT
	<0		error (system-error)

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
#include	<xperm.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */

extern "C" {
    extern int dialpass(cc *,int,int) noex ;
}


/* external variables */


/* local structures */

enum accmodes {
	accmode_rdonly,
	accmode_wronly,
	accmode_rdwr,
	accmode_overlast
} ;


/* forward reference */

static int	accmode(int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_openpass(cc *passfname,int oflags,int timeout,int opts) noex {
	int		rs = SR_FAULT ;
	if (passfname) {
	    rs = SR_INVALID ;
	    if (passfname[0]) {
	        if ((rs = accmode(oflags)) >= 0) {
	            while ((passfname[0] == '/') && (passfname[1] == '/')) {
	                passfname += 1 ;
	            }
	            if ((rs = perm(passfname,-1,-1,nullptr,(W_OK))) >= 0) {
	                rs = dialpass(passfname,timeout,opts) ;
	            }
	        } /* end if (accmode) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_openpass) */


/* local subroutines */

static int accmode(int oflags) noex {
	int		rs = SR_INVALID ;
	int		am = (oflags & (O_RDONLY | O_WRONLY | O_RDWR)) ;
	switch (am) {
	case (O_RDONLY):
	    rs = accmode_rdonly ;
	    break ;
	case (O_WRONLY):
	    rs = accmode_wronly ;
	    break ;
	case (O_RDWR):
	    rs = accmode_rdwr ;
	    break ;
	default:
	    rs = SR_INVALID ;
	    break ;
	} /* end switch */
	return rs ;
}
/* end subroutine (accmode) */


