/* ucfilemode SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* set (ensure) a minimum mode on a file */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	uc_chmod
	uc_minmod

	Description:
	Change the file-mode on files (or other filesystem objects).

	Synopses:
	int uc_chmod(cchar *fname,mode_t fm) noex
	int uc_minmod(cchar *fname,mode_t mm) noex

	Arguments:
	fname		file-name of file to possibly modify
	fm		file-mode to set on file

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<prefixfn.h>
#include	<localmisc.h>


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_chmod(cchar *fname,mode_t fm) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rv = 0 ; /* return-values */
	if (fname) ylikely {
	    cchar *fn ;
	    if (prefixfn pf ; (rs = pf.start(fname,-1,&fn)) > 0) {
		{
		    rs = u_chmod(fn,fm) ;
		    rv = rs ;
		}
		rs1 = pf.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (prefixfn) */
	} /* end if (non-null) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (uc_chmod) */

int uc_minmod(cchar *fname,mode_t mm) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		fchanged = false ; /* return-value */
	if (fname) ylikely {
	    cchar *fn ;
	    if (prefixfn pf ; (rs = pf.start(fname,-1,&fn)) > 0) {
	        mm &= (~ S_IFMT) ;
		if (ustat sb ; (rs = u_stat(fn,&sb)) >= 0) ylikely {
	            cmode	cm = (sb.st_mode & (~ S_IFMT)) ;
	            if ((cm & mm) != mm) {
	                fchanged = true ;
	                mm |= cm ;
	                rs = u_chmod(fn,mm) ;
	            } /* end if (needed a change) */
	        } /* end if (stat-able) */
		rs1 = pf.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (prefixfn) */
	} /* end if (non-null) */
	return (rs >= 0) ? fchanged : rs ;
}
/* end subroutine (uc_minmod) */

#ifdef	COMMENT
static int ensuremode(cchar *rbuf,mode_t nm) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		f = false ;
	if (rbuf) {
	    rs = SR_INVALID ;
	    if (rbuf[0]) {
		cmode	om = 0666 ;
		cint	of = O_RDONLY ;
	        if ((rs = u_open(rbuf,of,om)) >= 0) {
	            cint	fd = rs ;
	            if (ustat sb ; (rs = u_fstat(fd,&sb)) >= 0) {
	                cmode	cm = sb.st_mode & (~ S_IFMT) ;
	                nm &= (~ S_IFMT) ;
	                if ((cm & nm) != nm) {
		            f = true ;
	                    nm |= cm ;
		            rs = u_fchmod(fd,nm) ;
	                }
	            } /* end if (stat) */
	            rs1 = u_close(fd) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (open-close) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (ensuremode) */
#endif /* COMMENT */


