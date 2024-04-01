/* isfsremote SUPPORT */
/* lang=C++20 */

/* is the file on a local or remote filesystem? */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	isfsremote

	Description:
	This subroutine checks if the specified file-descriptor
	(FD) points to a file on a remote file-system.

	Synopsis:
	int isfsremote(int fd) noex

	Arguments:
	fd		file-descriptor to check

	Returns:
	>0		Yes, on a remote file-system
	==0		not on remote file-system
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<getbufsize.h>
#include	<matstr.h>
#include	<localmisc.h>

#include	"isfiledesc.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	getfstype(char *,int,int) noex ;
    extern int	islocalfs(cchar *,int) noex ;
}


/* external variables */


/* local structures */


/* forward refernces */


/* local variables */

static constexpr cchar	*localfs[] = {
	"sysv",
	"ufs",
	"ext1",
	"ext2",
	"ext3",
	"ext4",
	"xfs",
	"zfs",
	"btrfs",
	"apfs",
	"tmpfs",
	"devfs",
	"lofs",
	"vxfs",
	"pcfs",
	"hsfs",
	"smbfs",
	"autofs",
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int isfsremote(int fd) noex {
	int		rs ;
	int		f = false ;
	if ((rs = getbufsize(getbufsize_un)) >= 0) {
	    cint	fslen = rs ;
	    char	fstype[fslen+ 1] ;	/* <- VLA (yeh!) */
	    if ((rs = getfstype(fstype,fslen,fd)) >= 0) {
	        f = (matstr(localfs,fstype,rs) < 0) ;
	    }
	} /* end if (getbufsize) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (isfsremote) */

int islocalfs(cchar *sp,int sl) noex {
	return (matstr(localfs,sp,sl) >= 0) ;
}
/* end subroutine (islocalfs) */


