/* ucgetpuid SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
/* get the UID of a given process by its PID */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-15, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_getpuid

	Description:
	This subroutine retrieves from the system the UID of a given
	process (if that process exists).

	Synopsis:
	int uc_getpuid(pid_t pid) noex

	Arguments:
	pid		PID of process to retrieve UID for

	Returns:
	>=0		UID of process (as an integer)
	<0		error (system-return)

	Notes:
	Specifying a PID of zero (0) will be assumed to represent
	the current process (rather than that process that has PID==0)
	and will return the UID of the caller.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* system types */
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<climits>		/* |INT_MAX| */
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<storebuf.h>
#include	<libmallocxx.h>


import uconstants ;

/* local defines */

#define	PROCDNAME	"/proc"


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	mkpidfname(char *,cchar *,pid_t) noex ;


/* local variables */

static bufsizevar	maxpathlen(getbufsize_mp) ;


/* exported variables */


/* exported subroutines */

int uc_getpuid(pid_t pid) noex {
	int		rs = SR_INVALID ;
	int		rs1 ;
	int		r = 0 ;
	if (pid >= 0) {
	    uid_t	uid = 0 ;
	    if (pid > 0) {
	        if (char *pidfname{} ; (rs = libmalloc_mp(&pidfname)) >= 0) {
	            cchar	*pd = sysword.w_procdir ;
	            if ((rs = mkpidfname(pidfname,pd,pid)) >= 0) {
	                USTAT	sb ;
		        cint	nrs = SR_NOENT ;
	                if ((rs = u_stat(pidfname,&sb)) == nrs) {
		            if ((rs = u_stat(pd,&sb)) >= 0) {
		                rs = SR_SRCH ;
		            } else if (rs == nrs) {
		    	        rs = SR_NOSYS ;
		            }
		        } else {
	                    uid = sb.st_uid ;
		        }
	            } /* end if (mkpidfname) */
		    rs1 = lm_free(pidfname) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (mallocxx) */
	    } else if (pid == 0) {
	        uid = getuid() ;
	    }
	    r = int(uid & INT_MAX) ;
	} /* end if (valid PID) */
	return (rs >= 0) ? r : rs ;
}
/* end subroutine (uc_getpuid) */


/* local subroutines */

static int mkpidfname(char *fname,cchar *dev,pid_t pid) noex {
	int		rs  ;
	int		i = 0 ;
	if ((rs = maxpathlen) >= 0) {
	    cint	flen = rs ;
	    if (rs >= 0) {
	        rs = storebuf_strw(fname,flen,i,dev,-1) ;
	        i += rs ;
	    }
	    if ((rs >= 0) && (i > 0) && (fname[i-1] != '/')) {
	        rs = storebuf_chr(fname,flen,i,'/') ;
	        i += rs ;
	    }
	    if (rs >= 0) {
	        cint	v = pid ;
	        rs = storebuf_deci(fname,flen,i,v) ;
	        i += rs ;
	    }
	} /* end if (maxpathlen) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (mkpidfname) */


