/* getfstype SUPPORT */
/* lang=C++20 */

/* get the type of the filesystem for an FD */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-09-27, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We return the file-system type associated with the file
	attached to the given file-descriptor.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/statvfs.h>
#include	<usystem.h>
#include	<snwcpy.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int getfstype(char *nbuf,int nlen,int fd) noex {
	int		rs = SR_FAULT ;
	if (nbuf) {
	    STATVFS	vsb ;
	    if ((rs = u_fstatvfs(fd,&vsb)) >= 0) {
	        cchar	*cp = vsb.f_basetype ;
	        int	cl = strnlen(vsb.f_basetype,FSTYPSZ) ;
	        rs = snwcpy(nbuf,nlen,cp,cl) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getfstype) */


