/* uc_fstat */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* interface component for UNIX® library-3c */
/* get status on a file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-03, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

        This subroutine provides an enchanced |fstat(2)| function.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<limits.h>
#include	<unistd.h>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */

#define	FSTYPE_FLOAT	"float"

#ifdef	_ST_FSTYPSZ
#define	FSTYPESZ	_ST_FSTYPSZ
#else
#define	FSTYPESZ	16
#endif


/* external subroutines */


/* forward references */


/* exported subroutines */

int uc_fstat(int fd,USTAT *sbp) noex {
	int		rs = SR_FAULT ;
	int		fsize = 0 ;
	if (sbp) {
	    if ((rs = u_fstat(fd,sbp)) >= 0) {
	        fsize = (sbp->st_size & INT_MAX) ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? fsize : rs ;
}
/* end subroutine (uc_fstat) */


