/* uc_minmod SUPPORT */
/* lang=C20 */

/* interface component for UNIX® library-3c */
/* set (ensure) a minimum mode on a file */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<poll.h>
#include	<usystem.h>
#include	<localmisc.h>


/* external subroutines */

extern int	snfilemode(char *,int,mode_t) ;


/* forward references */


/* exported subroutines */

int uc_minmod(cchar *fname,mode_t mm) noex {
	int		rs = SR_FAULT ;
	int		f_changed = FALSE ;
	if (fname) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
		USTAT		sb ;
	        mm &= (~ S_IFMT) ;
	        if ((rs = u_stat(fname,&sb)) >= 0) {
	            mode_t	cm = (sb.st_mode & (~ S_IFMT)) ;
	            if ((cm & mm) != mm) {
	                f_changed = TRUE ;
	                mm |= cm ;
	                rs = u_chmod(fname,mm) ;
	            } /* end if (needed a change) */
	        } /* end if (stat-able) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? f_changed : rs ;
}
/* end subroutine (uc_minmod) */


