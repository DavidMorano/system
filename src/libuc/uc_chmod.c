/* uc_chmod SUPPORT */
/* lang=C++20 */

/* interface component for UNIX� library-3c */
/* change mode on a file */

#define	CF_DEBUGS	0		/* compile-time debugging */

/* revision history:

	= 1998-04-13, David A�D� Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<cstring>
#include	<usystem.h>
#include	<typenonpath.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */

extern int	mkuserpath(char *,const char *,const char *,int) ;
extern int	mkcdpath(char *,const char *,int) ;
extern int	mkvarpath(char *,const char *,int) ;
extern int	hasvarpathprefix(const char *,int) ;


/* forward references */


/* exported variables */


/* exported subroutines */

int uc_chmod(cchar *fname,mode_t fm) noex {
	int		rs = SR_OK ;
	int		fl ;

	if (fname == NULL) return SR_FAULT ;

	if (fname[0] == '\0') return SR_INVALID ;

	fl = strlen(fname) ;

	if (! typenonpath(fname,fl)) {
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
	        rs = u_chmod(fname,fm) ;
	    }
	} /* end if (nonpath) */

	return rs ;
}
/* end subroutine (uc_chmod) */


