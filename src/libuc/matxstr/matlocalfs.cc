/* matlocalfs SUPPORT */
/* lang=C++20 */

/* determine is the given counted c-string is a local-filesystem name */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	matlocalfs

	Description:
	This subroutine deterines if the given counted c-string
	matches up against (fixed at the moment) list of file-sytem
	name that are assume to be local file-systems.

	Synopsis:
	int matlocalfs(cchar *,sp,int sl) noex

	Arguments:
	sp		name pointer
	sl		name length

	Returns:
	>0		Yes, on a remote file-system
	==0		not on remote file-system
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<matstr.h>
#include	<localmisc.h>

#include	"matxstr.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward refernces */


/* local variables */

constexpr cpcchar	localfs[] = {
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

int matlocalfs(cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (sp) {
 	    rs = (matstr(localfs,sp,sl) >= 0) ;
	}
	return rs ;
}
/* end subroutine (matlocalfs) */


