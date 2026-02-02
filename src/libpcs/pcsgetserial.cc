/* pcsgetserial SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* PCS Get-Serial number */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	pcsgetserial

	Description:
	This subroutine is used to get a unique serial number from
	a specified file.  These numbes are used for sequencing and
	other purposes in general code.  An attempt is made to lock
	the SERIAL file and if the lock fails, the subroutine returns
	an error (negative number).  Locking may indeed fail due
	to the very poorly written file locking code on the old
	SunOS 4.xxx version of the UNIX system. Remote file locking
	over NFS on the old SunOS 4.xxx systems **never** worked
	correctly!  Other errors, like "couldn't create the file"
	are reported as such.

	Synopsis:
	int pcsgetserial(cchar *pr) noex

	Arguments:
	pr		PCS program-root

	Returns:
	>0		the serial number
	==0		file was just created
	<0		could not get it!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<mkpathx.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"pcsgetserial.h"


/* local defines */

#ifndef	SERIALFNAME
#define	SERIALFNAME	"var/serial"
#endif

#define	NDF		"pcsgetserial.deb"


/* external subroutines */

extern "C" {
    extern int uc_unlink(cchar *) noex ;
    extern int uc_create(cchar *,mode_t) noex ;
    extern int uc_fminmod(int,mode_t) noex ;
    extern int uc_fpathconf(int,int,long *) noex ;
    extern int uc_fchown(int,uid_t,gid_t) noex ;
    extern int uc_fstat(int,ustat *) noex ;
    extern int uc_close(int) noex ;
} /* end extern */

extern "C" {
    extern int	getserial(cchar *) noex ;
}


/* external variables */


/* local structures */

namespace {
    struct serializer {
	cchar	*pr ;
	serializer(cchar *p) noex : pr(p) { } ;
	operator int () noex ;
    } ; /* end struct (serializer) */
} /* end namespace */


/* forwards references */


/* local variables */


/* exported variables */


/* exported subroutines */

int pcsgetserial(cchar *pr) noex {
	int		rs ;
	int		s = 0 ; /* return-value */
	if (pr) {
	    rs = SR_INVALID ;
	    if (pr[0]) {
		serializer so(pr) ;
		rs = so ;
		s = rs ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? s : rs ;
}
/* end subroutine (pcsgetserial) */


/* local subroutines */

serializer::operator int () noex {
    	cnullptr	np{} ;
	int		rs ;
	int		rs1 ;
	int		s = 0 ;
	if (char *sbuf ; (rs = lm_mp(&sbuf)) >= 0) {
	    if ((rs = mkpath(sbuf,pr,SERIALFNAME)) >= 0) {
	        if ((rs = getserial(sbuf)) >= 0) {
		    s = rs ;
	        } else if (isNotPresent(rs)) {
		    cmode	m = 0666 ;
		    s = 0 ;
		    uc_unlink(sbuf) ;
		    if ((rs = uc_create(sbuf,m)) >= 0) {
		        cint	fd = rs ;
		        if ((rs = uc_fminmod(fd,m)) >= 0) {
	    	            cint	n = _PC_CHOWN_RESTRICTED ;
	    	            if ((rs = uc_fpathconf(fd,n,np)) == 0) {
		                if (ustat sb ; (rs = uc_fstat(fd,&sb)) >= 0) {
			            rs = uc_fchown(fd,sb.st_uid,sb.st_gid) ;
			        }
			    } /* end if (u_pathconf) */
		        } /* end if (uc_minmod) */
		        rs1 = uc_close(fd) ;
	        	if (rs >= 0) rs = rs1 ;
		    } /* end if (uc_createfile) */
	        } /* end if */
	    } /* end if (mkpath) */
	} /* end if (m-a-f) */
	return (rs >= 0) ? rs : s ;
}
/* end method (serializer::operator) */


