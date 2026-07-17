/* chmods SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* change permissions on all components of a file path */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-10, David A­D­ Morano
	This code was originally written. This subroutines is
	standard on some UNIXes but not on others so it is now
	provided.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	chmods

  	Description:
	This subroutine will try to change the permissions mode on
	all of the directories in the specified directory path.

	Synopsis:
	int chmods(cchar *dname,mode_t mode) noex

	Arguments:
	- dname		direcrtory path to a new directory to create
	- mode		newly created directories are created with this

	Returns:
	>=0		operation completed successfully
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® |uid_t| */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strchr(3c)| + |strcmp(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<strwcpy.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"chmods.h"


/* local defines */

#define	CI		xcheckinfo


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct xcheckinfo {
	uid_t		uid ;
	mode_t		dm ;
} ; /* end struct */


/* forward references */

local int	checkinfo_start		(CI *,mode_t) noex ;
local int	checkinfo_dir		(CI *,cchar *) noex ;
local int	checkinfo_finish	(CI *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int chmods(cchar *dname,mode_t dm) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	if (dname) ylikely {
	    if (CI ci ; (rs = checkinfo_start(&ci,dm)) >= 0) {
	        if (char *dbuf ; (rs = libmem.mp(&dbuf)) >= 0) {
	            if ((rs = mkpath1(dbuf,dname)) >= 0) {
		        char	*cp, *cp2 ;
	                cp = dbuf ;
	                while ((cp2 = strchr(cp,'/')) != nullptr) {
	                    *cp2 = '\0' ;
	                    if (((cp2 - cp) > 0) && (strcmp(cp,".") != 0)) {
	                        rs = checkinfo_dir(&ci,dbuf) ;
			        c += rs ;
	                    } /* end if */
	                    *cp2 = '/' ;
	                    cp = (cp2 + 1) ;
	                    if (rs < 0) break ;
	                } /* end while */
	                if ((rs >= 0) && (*cp != '\0')) {
	                    rs = checkinfo_dir(&ci,dbuf) ;
			    c += rs ;
	                }
	            } /* end if (mkpath) */
	            rs1 = libmem.free(dbuf) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	        rs1 = checkinfo_finish(&ci) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (checkinfo) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (chmods) */


/* local subroutines */

local int checkinfo_start(CI *cip,mode_t dm) noex {
    	int		rs = SR_FAULT ;
	if (cip) ylikely {
	    cip->dm = dm ;
	    cip->uid = getuid() ;
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (checkinfo_start) */

local int checkinfo_dir(CI *cip,cchar *dirbuf) noex {
	int		rs ;
	int		c = 0 ; /* return-value */
	if (ustat sb ; (rs = u_stat(dirbuf,&sb)) >= 0) ylikely {
	    if (sb.st_uid == cip->uid) {
	        if ((sb.st_mode & cip->dm) != cip->dm) {
		    cmode	nm = (sb.st_mode | cip->dm) ;
	            rs = u_chmod(dirbuf,nm) ;
		    c = 1 ;
	        }
	    }
	} /* end if (u_stat) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (checkinfo_dir) */

local int checkinfo_finish(CI *) noex {
	return SR_OK ;
} /* end subroutine (checkinfo_finish) */


