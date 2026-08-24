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
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<permx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */


/* local defines */


/* external subroutines */

extern "C" {
    extern int dialpass(cc *,int,int) noex ;
} /* end extern (C) */


/* external variables */


/* local structures */

enum accmodes {
	accmode_rdonly,
	accmode_wronly,
	accmode_rdwr,
	accmode_overlast
} ; /* end enum */


/* forward reference */

local int	accmode(int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_openpass(cc *passfname,int oflags,int timeout,int opts) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (passfname) ylikely {
	    rs = SR_INVALID ;
	    if (passfname[0]) ylikely {
	        if ((rs = accmode(oflags)) >= 0) ylikely {
	            cint am = W_OK ;
	            while ((passfname[0] == '/') && (passfname[1] == '/')) {
	                passfname += 1 ;
	            } /* end while */
	            if ((rs = perm(passfname,-1,-1,np,am)) >= 0) ylikely {
	                rs = dialpass(passfname,timeout,opts) ;
	            }
	        } /* end if (accmode) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (uc_openpass) */


/* local subroutines */

local int accmode(int oflags) noex {
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
} /* end subroutine (accmode) */


