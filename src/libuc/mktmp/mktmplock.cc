/* mktmplock SUPPORT */
/* lang=C++20 */

/* make a temporary file which is "lockable" */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mktmplock

	Description:
	Make a temporary file that is also record lockable.  Why
	does this sort of function even have to exist (you might
	ask)?  Because with the introduction of the 'tmpfs' file-system
	in System V Release 4 (like Sun Solaris as primary example)
	in 1988, files put into that file-system are not lockable!
	Unfortunately, '/tmp' is mounted as a 'tmpfs' file-system!
	That is a place where file locks were often kept (sigh)!

	Synopsis:
	int mktmplock(char *rbuf,cchar *fn,mode_t fm) noex

	Arguments:
	rbuf		result buffer pointer
	fn		file-name
	fm		file creation mode

	Returns:
	>=0		length of created temporary file if successful
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<usystem.h>
#include	<strwcpy.h>
#include	<mkpathx.h>
#include	<dirseen.h>
#include	<isoneof.h>
#include	<localmisc.h>

#include	"mktmp.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* forward references */

static int		mktmptry(char *,cchar *,cchar *,mode_t) noex ;
static int		lockable(cchar *,mode_t) noex ;


/* local variables */

static constexpr cchar	*tmpdirs[] = {
	"/tmp",
	"/var/tmp",
	nullptr
} ;

static constexpr int	rslock[] = {
	SR_AGAIN,
	SR_BADF,
	SR_DEADLK,
	SR_INVAL,
	SR_NOLCK,
	SR_OPNOTSUPP,
	0
} ;


/* exported variables */


/* exported subroutines */

int mktmplock(char *rbuf,cchar *fn,mode_t fm) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ;
	if (rbuf && fn) {
	    rs = SR_INVALID ;
	    if (fn[0]) {
		for (int i = 0 ; (rs == 0) && tmpdirs[i] ; i += 1) {
		    cchar	*tmpdir = tmpdirs[i] ;
		    rs = mktmptry(rbuf,tmpdir,fn,fm) ;
		    rl = rs ;
		} /* end if (tmpdirs) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (mktmplock) */


/* local subroutines */

static int mktmptry(char *rbuf,cchar *tmpdir,cchar *fn,mode_t fm) noex {
	int		rs ;
	int		rl = 0 ;
	if ((rs = mkpath(rbuf,tmpdir,fn)) >= 0) {
	    rl = rs ;
	    if ((rs = lockable(rbuf,fm)) == 0) {
	        rl = 0 ;
	    }
	    u_unlink(rbuf) ;
	} /* end if (mkpath) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (mktmptry) */

static int lockable(cchar *fname,mode_t fm) noex {
	int		rs ;
	int		f = false ;
	if ((rs = u_open(fname,O_RDWR,fm)) >= 0) {
	    cint	fd = rs ;
	    if ((rs = uc_lockf(fd,F_TLOCK,0z)) >= 0) {
		f = true ;
	    } else if (isOneOf(rslock,rs)) {
		rs = SR_OK ;
	    }
	    u_close(fd) ;
	} /* end if (u_open) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (lockable) */


