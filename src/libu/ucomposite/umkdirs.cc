/* umkdirs SUPPORT */
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
	int u_mkdirs(cchar *dname,mode_t dm) noex

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
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<umem.hh>
#include	<uids.hh>
#include	<localmisc.h>

#include	"umkdirs.h"

import ulibvals ;			/* |ulibval(3u)|  */
import umisc ;				/* |mknpath(3u)|  */

/* local defines */


/* imported namespace */

using libu::uids ;			/* type */
using libu::uperm ;			/* subroutine */
using libu::umem ;			/* variable */


/* external subroutines */


/* external variables */


/* local structues */

namespace {
    struct mker {
	cchar		*dname ;
	int		maxpath ;
	mode_t		dm ;
	mker(cchar *d,mode_t m) noex : dname(d), dm(m) { } ;
	operator int () noex ;
	int procdir(uids *,cchar *) noex ;
	int mkdirer(uids *,cchar *) noex ;
    } ; /* end struct (mker) */
} /* end namespace */


/* forward references */


/* local variables */

static cint	maxpathlen = ulibval.maxpathlen ;


/* external variables */


/* external subroutines */

sysret_t u_mkdirs(cchar *dname,mode_t dm) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	dm &= (~ S_IFMT) ;
	if (dname) ylikely {
	    rs = SR_INVALID ;
	    if (dname[0]) ylikely {
		if (mker mo(dname,dm) ; (rs = mo) >= 0) {
		    c = rs ;
		} /* end if (mker) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (u_mkdirs) */


/* local subroutines */

mker::operator int () noex {
    	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = maxpathlen) >= 0) {
	    maxpath = rs ;
	        if (uids id ; (rs = id.load) >= 0) ylikely {
	            if ((rs = procdir(&id,dname)) >= 0) {
	                c += rs ;
	            } else if (rs == SR_NOENT) {
			rs = mkdirer(&id,dname) ;
			c += rs ;
	            } /* end if (needed some creations) */
	            rs1 = id.release ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (ids) */
	} /* end if (maxpathlen) */
	return (rs >= 0) ? c : rs ;
} /* end method (mker::operator) */

int mker::procdir(uids *idp,cchar *dn) noex {
	int		rs ;
	if (ustat sb ; (rs = u_stat(dn,&sb)) >= 0) {
	    if (S_ISDIR(sb.st_mode)) {
	        if ((rs = idp->perm(&sb,X_OK)) == 0) {
		    rs = uperm(dn,X_OK) ;
		}
	    } else {
	        rs = SR_NOTDIR ;
	    }
	} else if (rs == SR_NOENT) {
	    if ((rs = u_mkdir(dn,dm)) >= 0) {
	        rs = 1 ;
	    }
	} /* end if */
	return rs ;
} /* end method (mker::procdir) */

int mker::mkdirer(uids *idp,cchar *dn) noex {
    	cnullptr	np{} ;
	cint		dsz = (maxpath + 1) ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (char *dbuf ; (rs = umem.mall(dsz,&dbuf)) >= 0) ylikely {
            if ((rs = mknpath(dbuf,maxpath,dn)) >= 0) ylikely {
                cchar       *dp = dbuf ;
                for (char *bp ; (bp = strchr(dp,'/')) != np ; ) {
                    bool    f = true ;
                    *bp = '\0' ;	/* <- set temporary termination */
                    f = f && ((bp - dp) > 0) ;
                    f = f && (strcmp(dp,".") != 0) ;
                    if (f) {
                        rs = procdir(idp,dbuf) ;
                        c += rs ;
                    } /* end if */
                    *bp = '/' ;		/* <- remove temporary termination */
                    dp = (bp + 1) ;
                    if (rs < 0) break ;
                } /* end for */
                if ((rs >= 0) && (*dp != '\0')) {
                    rs = procdir(idp,dbuf) ;
                    c += rs ;
                } /* end if */
            } /* end if (mkpath1) */
	    rs1 = umem.free(dbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
} /* end method (mker::mkdirer) */


