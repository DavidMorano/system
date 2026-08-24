/* ucmkdirp SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* make parent directories in a file path */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_mkdirp

	Description:
	Make a directory and all non-existent parents of that directory.

	Synopsis:
	uc_mkdirp(cchar *dname,mode_t dm) noex

	Arguments:
	dname		directory name to create (and any non-existent parents)
	dm		permission-mode of created directory

	Returns:
	>=0		OK
	<0		error (system-return)

	Notes:
	This subroutine requires linking with the |libgen(3gen)|
	library.  That library is not considered overly useful to
	the world and therefore has been deprecated.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<cerrno>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */


/* local defines */

#if	defined(SYSHAS_MKDIRP) && (SYSHAS_MKDIRP > 0)
#define	F_MKDIRP	1
#else
#define	F_MKDIRP	0
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
   extern int	mkdirp(cchar *,mode_t) noex ;
} /* end extern (C) */


/* external variables */


/* local structures */


/* forward references */

local int	ucmkdirp(cchar *,mode_t) noex ;


/* local variables */

constexpr bool		f_mkdirp = F_MKDIRP ;


/* exported variables */


/* exported subroutines */

int uc_mkdirp(cchar *fname,mode_t m) noex {
	int		rs ;
	if_constexpr (f_mkdirp) {
	    rs = ucmkdirp(fname,m) ;
	} else {
	    rs = SR_NOSYS ;
	}
	return rs ;
} /* end subroutine (uc_mkdirp) */


/* local subroutines */

local int ucmkdirp(cchar *fname,mode_t m) noex {
	int		rs ;
	int		to_io = utimeout[uto_io] ;
	int		to_dquot = utimeout[uto_dquot] ;
	int		to_nospc = utimeout[uto_nospc] ;
	bool		f_exit = false ;
	repeat {
	    rs = mkdirp(fname,m) ;
	    if (rs == -1) {
	        rs = (neg errno) ;
	    } else if (rs < -1) {
	        rs = SR_INVALID ;
	    }
	    if (rs < 0) {
	        switch (rs) {
	        case SR_IO:
	            if (to_io-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
	            break ;
	        case SR_DQUOT:
	            if (to_dquot-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
	            break ;
	        case SR_NOSPC:
	            if (to_nospc-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
	            break ;
	        case SR_AGAIN:
	        case SR_INTR:
	            break ;
		default:
		    f_exit = true ;
		    break ;
	        } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;
	return rs ;
} /* end subroutine (ucmkdirp) */


