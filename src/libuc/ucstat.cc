/* ucstat SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* get status on a file */

#define	CF_DEBUGS	0		/* compile-time debugging */

/* revision history:

	= 1998-06-03, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	uc_stat

	Description:
	This subroutine provides an enchanced |stat(2)| function.
	It also removes trailing slash ('/') character from file-names,
	but it will preserve a lone pair of characters consisting
	of '//'.

	Notes:
	Some systems do not ignore trailing slash characters on
	file-names, so we take care of it for them (at least here
	we do).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/statvfs.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<libmallocxx.h>
#include	<nonpath.h>
#include	<mkx.h>
#include	<localmisc.h>

#pragma		GCC dependency	"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	FSTYPE_FLOAT	"float"

#ifdef	_ST_FSTYPSZ
define	FSTYPESZ	_ST_FSTYPSZ
#else
#define	FSTYPESZ	16
#endif


/* external subroutines */

extern "C" {
    extern int	mkuserpath(char *,cchar *,cchar *,int) noex ;
    extern int	mkcdpath(char *,cchar *,int) noex ;
}


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_stat(cchar *fname,ustat *sbp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		fl ;
	int		flen = 0 ;
	char		*fbuf = nullptr ;
	(void) rs1 ;
	if (fname == nullptr) return SR_FAULT ;
	if (sbp == nullptr) return SR_FAULT ;

	if (fname[0] == '\0') return SR_INVALID ;

	fl = lenstr(fname) ;

	if ((fl > 1) && (fname[0] != '/')) {
	    if (fname[fl-1] == '/') {
	        fl -= 1 ;
	        if ((rs = uc_libmallocstrw(fname,fl,&fbuf)) >= 0) {
		    fname = fbuf ;
		}
	    }
	}

	if (rs >= 0) {

	    if ((rs = nonpath(fname,fl)) == 0) {
	        memclear(sbp) ;
	        sbp->st_mode = S_IFNAM ;
	        sbp->st_uid = UID_NOBODY ;
	        sbp->st_gid = GID_NOBODY ;
	        strncpy(sbp->st_fstype,FSTYPE_FLOAT,FSTYPESZ) ;
	    } else if (rs >= 0) {
	        cint	elen = MAXPATHLEN ;
	        if (char *ebuf ; (rs = uc_libmalloc((elen+1),&ebuf)) >= 0) {
	            if ((rs = mkuserpath(ebuf,nullptr,fname,fl)) > 0) {
	                fname = ebuf ;
	            } else if (rs == 0) {
	                if ((rs = mkcdpath(ebuf,fname,fl)) > 0) {
	                    fname = ebuf ;
	                } else if (rs == 0) {
	                    if ((rs = mkvarpath(ebuf,fname,fl)) > 0) {
	                        fname = ebuf ;
		            }
		        }
	            }
	            if (rs >= 0) {
	                if ((rs = u_stat(fname,sbp)) >= 0) {
		            flen = (sbp->st_size & INT_MAX) ;
			}
	            }
		    uc_libfree(ebuf) ;
	        } /* end if (memory-allocation) */
	    } /* end if (nonpath) */

	    if (fbuf != nullptr) {
		uc_libfree(fbuf) ;
	    }

	} /* end if (ok) */

	return (rs >= 0) ? flen : rs ;
}
/* end subroutine (uc_stat) */


