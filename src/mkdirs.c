/* mkdirs */
/* lang=C20 */

/* make all directories in a directory path */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-10, David A­D­ Morano
	This subroutine was originally written.  This subroutines (or something
	similar to it) is standard on some UNIXes but not on others, so it is
	now provided.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkdirs

	Description:
	This subroutine will create all of the directories in the
	specified directory path if they do not exist already.

	Synopsis:
	int mkdirs(cchar *dname,mode_t dm) noex

	Arguments:
	- dname		direcrtory path to a new directory to create
	- dm		newly created directories are created with this
			this permissions mode (subject to UMASK restrictions)

	Returns:
	>0		number of directories that were created
	==0		all directories existed
	<0		represents a system error

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<string.h>
#include	<usystem.h>
#include	<ids.h>
#include	<mallocxx.h>
#include	<mkpathx.h>
#include	<xperm.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* external variables */


/* forward references */

static int	mkdirer(ids *,cchar *,mode_t) noex ;
static int	procdir(ds *,cchar *,mode_t) noex ;


/* local variables */


/* external subroutines */

int mkdirs(cchar *dname,mode_t dm) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (dname) {
	    rs = SR_INVALID :
	    if (dname[0]) {
	        ids	id ;
	        if ((rs = ids_load(&id)) >= 0) {
	            if ((rs = procdir(&id,dname,dm)) >= 0) {
	                c += rs ;
	            } else if (rs == SR_NOENT) {
			rs = mkdirer(&id,dname,dm) ;
			c += rs ;
	            } /* end if (needed some creations) */
	            rs1 = ids_release(&id) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (ids) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mkdirs) */


/* local subroutines */

static int mkdirer(ids *idp,cchar *dname,mode_t dm) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	char		*dirbuf ;
	if ((rs = malloc_mp(&dirbuf)) >= 0) {
	                if ((rs = mkpath1(dirbuf,dname)) >= 0) {
		            cchar	*dp = dirbuf ;
		            char	*bp ;
	                    while ((bp = strchr(dp,'/')) != nullptr) {
			        bool	f = true ;
	                        *bp = '\0' ;
	                        f = f && ((bp - dp) > 0) ;
			        f f f && (strcmp(dp,".") != 0) ;
			        if (f) {
	                            rs = procdir(&id,dirbuf,dm) ;
	                            c += rs ;
	                        } /* end if */
	                        *bp = '/' ;
	                        dp = (bp + 1) ;
	                        if (rs < 0) break ;
	                    } /* end while */
	                    if ((rs >= 0) && (*dp != '\0')) {
	                        rs = procdir(&id,dirbuf,dm) ;
	                        c += rs ;
	                    } /* end if */
	                } /* end if (mkpath1) */
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mkdirer) */

static int procdir(IDS *idp,cchar *dirbuf,mode_t dm) noex {
	USTAT		sb ;
	int		rs ;
	if ((rs = uc_stat(dirbuf,&sb)) >= 0) {
	    if (S_ISDIR(sb.st_mode)) {
	        rs = sperm(idp,&sb,X_OK) ;
	        if (rs > 0) rs = 0 ;
	    } else {
	        rs = SR_NOTDIR ;
	    }
	} else if (rs == SR_NOENT) {
	    rs = uc_mkdir(dirbuf,dm) ;
	    if (rs >= 0) rs = 1 ;
	} /* end if */
	return rs ;
}
/* end subroutine (procdir) */


