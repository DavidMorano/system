/* mkdirs SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make all directories in a directory path */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-10, David A­D­ Morano
	This code was originally written.  This subroutines
	(or something similar to it) is standard on some UNIXes but
	not on others, so it is now provided.

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
			this permissions mode (subject to UMASK 
			restrictions)

	Returns:
	>0		number of directories that were created
	==0		all directories existed
	<0		represents a system error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strcmp(3c)| + |strchr(3c)| */
#include	<usystem.h>
#include	<ids.h>
#include	<libmallocxx.h>
#include	<mkpathx.h>
#include	<permx.h>
#include	<localmisc.h>

#include	"mkdirs.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structues */


/* forward references */

static int	procdir(ids *,cchar *,mode_t) noex ;
static int	mkdirer(ids *,cchar *,mode_t) noex ;


/* local variables */


/* external variables */


/* external subroutines */

int mkdirs(cchar *dname,mode_t dm) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	dm &= (~ S_IFMT) ;
	if (dname) ylikely {
	    rs = SR_INVALID ;
	    if (dname[0]) ylikely {
	        if (ids id ; (rs = id.load) >= 0) ylikely {
	            if ((rs = procdir(&id,dname,dm)) >= 0) {
	                c += rs ;
	            } else if (rs == SR_NOENT) {
			rs = mkdirer(&id,dname,dm) ;
			c += rs ;
	            } /* end if (needed some creations) */
	            rs1 = id.release ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (ids) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mkdirs) */


/* local subroutines */

static int procdir(ids *idp,cchar *dirbuf,mode_t dm) noex {
	int		rs ;
	if (ustat sb ; (rs = uc_stat(dirbuf,&sb)) >= 0) {
	    if (S_ISDIR(sb.st_mode)) {
	        rs = permid(idp,&sb,X_OK) ;
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

static int mkdirer(ids *idp,cchar *dname,mode_t dm) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (char *dirbuf ; (rs = libmalloc_mp(&dirbuf)) >= 0) ylikely {
            if ((rs = mkpath(dirbuf,dname)) >= 0) ylikely {
                cchar       *dp = dirbuf ;
                for (char *bp ; (bp = strchr(dp,'/')) != np ; ) {
                    bool    f = true ;
                    *bp = '\0' ;	/* <- set temporary termination */
                    f = f && ((bp - dp) > 0) ;
                    f = f && (strcmp(dp,".") != 0) ;
                    if (f) {
                        rs = procdir(idp,dirbuf,dm) ;
                        c += rs ;
                    } /* end if */
                    *bp = '/' ;		/* <- remove temporary termination */
                    dp = (bp + 1) ;
                    if (rs < 0) break ;
                } /* end for */
                if ((rs >= 0) && (*dp != '\0')) {
                    rs = procdir(idp,dirbuf,dm) ;
                    c += rs ;
                } /* end if */
            } /* end if (mkpath1) */
	    rs1 = uc_libfree(dirbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mkdirer) */


