/* ucopenfs SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* open a file-system (?) */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* compile-time debugging */

/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	uc_openfs

	Description:
	This subroutine opens a file-system (what does this mean?).

	Synopsis:
	int uc_openfs(passfn,oflags,timeout,opts)
	char	passfn[] ;
	int		oflags ;
	int		timeout ;
	int		opts ;

	Arguments:
	passfn	the pass filename
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
#include	<sys/mman.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<permx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<deb.hh>		/* LIBU (DPRINT(3u)| */

#include	"ucopeninfo.h"

import deb ;				/* debugging */

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* compile-time debugging */
#endif


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int dialpass(cchar *,int,int) noex ;
} /* end extern (C) */


/* external variables */


/* local structures */

enum accmodes {
	accmode_rdonly,
	accmode_wronly,
	accmode_rdwr,
	accmode_overlast
} ; /* end enum (accmodes) */


/* forward reference */

local int	accmode(int) noex ;


/* local variables */

cbool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int uc_openfs(cchar *passfn,int oflags,int to,int opts) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	DEBPRINTF("ent passfn=%s\n",passfn) ;
	if (passfn) ylikely {
	    rs = SR_INVALID ;
	    if (passfn[0]) ylikely {
	        /* argument check */
	        if ((rs = accmode(oflags)) >= 0) {
	            while ((passfn[0] == '/') && (passfn[1] == '/')) {
	                passfn += 1 ;
	            } /* end while */
	            if ((rs = perm(passfn,-1,-1,np,(W_OK))) >= 0) {
	                rs = dialpass(passfn,to,opts) ;
	            } /* end if (perm) */
	        } /* end if (accmode) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (uc_openfs) */


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


