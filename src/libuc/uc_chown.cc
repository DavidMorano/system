/* uc_chown SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* change owner (and group) on a file */

#define	CF_DEBUGS	0		/* compile-time debugging */

/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<cstring>
#include	<usystem.h>
#include	<nonpath.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */

extern "C" {
    extern int	mkuserpath(char *,cchar *,cchar *,int) noex ;
    extern int	mkcdpath(char *,cchar *,int) noex ;
    extern int	mkvarpath(char *,cchar *,int) noex ;
    extern int	hasvarpathprefix(cchar *,int) noex ;
}


/* forward references */


/* exported variables */


/* exported subroutines */

int uc_chown(cchar *fname,uid_t uid,gid_t gid) noex {
	int		rs = SR_OK ;
	int		fl ;

	if (fname == NULL) return SR_FAULT ;

	if (fname[0] == '\0') return SR_INVALID ;

	fl = lenstr(fname) ;

	if ((rs = nonpath(fname,fl)) > 0) {
	    char	efname[MAXPATHLEN + 1] ;

	    if ((rs = mkuserpath(efname,NULL,fname,fl)) > 0) {
	        fname = efname ;
	    } else if (rs == 0) {
	        if ((rs = mkcdpath(efname,fname,fl)) > 0) {
	            fname = efname ;
	        } else if (rs == 0) {
	            if ((rs = mkvarpath(efname,fname,fl)) > 0) {
	                fname = efname ;
		    }
		}
	    }
	    if (rs >= 0) {
	        rs = u_chown(fname,uid,gid) ;
	    }
	} else if (rs >= 0) {
	        rs = u_chown(fname,uid,gid) ;
	} /* end if (nonpath) */

	return rs ;
}
/* end subroutine (uc_chown) */


