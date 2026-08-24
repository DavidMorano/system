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
	int uc_openfs(passfname,oflags,timeout,opts)
	char	passfname[] ;
	int		oflags ;
	int		timeout ;
	int		opts ;

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
#include	<sys/mman.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<ucopen.h>
#include	<ucdesc.h>
#include	<permx.h>
#include	<localmisc.h>

#include	"ucopeninfo.h"

/* local defines */

#define	NENVS		100

#ifndef	SHMNAMELEN
#define	SHMNAMELEN	MAXNAMELEN
#endif

#ifndef	DEBFNAME
#define	DEBFNAME	"child.deb"
#endif

#ifndef	VARPATH
#define	VARPATH		"PATH"
#endif

#ifndef	VARPWD
#define	VARPWD		"PWD"
#endif

#define	DEFPATH		"/usr/bin:/bin"

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


/* exported variables */


/* exported subroutines */

int uc_openfs(cchar *passfname,int oflags,int to,int opts) noex {
	int		rs ;

#if	CF_DEBUG
	debugprintf("uc_openfs: passfname=%s\n",passfname) ;
#endif

	if (passfname == NULL) return SR_FAULT ;

	if (passfname[0] == '\0') return SR_INVALID ;

/* argument check */

	if ((rs = accmode(oflags)) >= 0) {
	    while ((passfname[0] == '/') && (passfname[1] == '/')) {
	        passfname += 1 ;
	    }
	    if ((rs = perm(passfname,-1,-1,NULL,(W_OK))) >= 0) {
	        rs = dialpass(passfname,to,opts) ;
	    }
	}

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


