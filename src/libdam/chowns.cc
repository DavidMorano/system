/* chowns SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* change ownership on all components of a file path */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-10, David A­D­ Morano
	This code was originally written.  This subroutines is
	standard on some UNIXes but not on others so it is now
	provided.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	chowns

	Description:
	This subroutine will try to change the wonership (UID and
	GID) on all of the directories in the specified directory
	path.

	Synopsis:
	int chowns(cchar *dir,uid_t uid,gid_t gid) noex

	Arguments:
	- dir		direcrtory path to a new directory to create
	- uid
	- gid

	Returns:
	>=0		operation completed successfully
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® system types */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strchr(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"chowns.h"


/* local defines */


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct checkinfo {
	uid_t		uid ;
	gid_t		gid ;
	mode_t		mode ; /* not used here */
    } ; /* end struct (checkinfo) */
} /* end namespace */


/* forward references */

local int	checkdir(checkinfo *,cchar *) noex ;


/* local variables */

constexpr uid_t		uidend = -1 ;
constexpr gid_t		gidend = -1 ;


/* exported variables */


/* exported subroutines */

int chowns(cchar *dname,uid_t uid,gid_t gid) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	if (dname) ylikely {
	    rs = SR_INVALID ;
	    if (dname[0]) ylikely {
	        if (char *dbuf ; (rs = libmem.mp(&dbuf)) >= 0) ylikely {
	            checkinfo	ci{} ;
	            ci.uid = uid ;
	            ci.gid = gid ;
	            if ((rs = mkpath(dbuf,dname)) >= 0) ylikely {
	                cchar	*cp = dbuf ;
	                for (char *bp ; (bp = strchr(cp,'/')) != np ; ) {
	                    *bp = '\0' ;
	                    if (((bp - cp) > 0) && (strcmp(cp,".") != 0)) {
	                        rs = checkdir(&ci,dbuf) ;
			        c += 1 ;
	                    } /* end if */
	                    *bp = '/' ;
	                    cp = bp + 1 ;
	                    if (rs < 0) break ;
	                } /* end while */
	                if ((rs >= 0) && (*cp != '\0')) {
	                    rs = checkdir(&ci,dbuf) ;
			    c += 1 ;
	                } /* end if */
	            } /* end if (mkpath) */
		    rs1 = libmem.free(dbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (chowns) */


/* local subroutines */

local int checkdir(checkinfo *cip,cchar *dirbuf) noex {
	int		rs ;
	if (ustat sb ; (rs = u_stat(dirbuf,&sb)) >= 0) ylikely {
	    bool	f = false ;
	    f = f || ((cip->uid != uidend) && (sb.st_uid != cip->uid)) ;
	    f = f || ((cip->gid != gidend) && (sb.st_gid != cip->gid)) ;
	    if (f) {
		rs = u_chown(dirbuf,cip->uid,cip->gid) ;
	    }
	} /* end if */
	return rs ;
} /* end subroutine (checkdir) */


