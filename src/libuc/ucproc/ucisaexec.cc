/* ucisaexec SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	uc_isaexecve

	Description:
	This is a Solaris® specific hack that is used to find a
	more optimized program to execute than the one that is the
	standard one. Some ISAs may have more optimized versions
	of some programs.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/wait.h>
#include	<unistd.h>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<usysflag.h>
#include	<usupport.h>
#include	<ucproc.h>
#include	<utimeout.h>
#include	<localmisc.h>

#include	"ucexec.h"

/* local defines */


/* external subroutines */


/* exported variables */


/* exported subroutines */

int uc_isaexecve(cchar *pfn,con mainv argv,con mainv envv) noex {
	int		rs ;
	if (pfn && argv && envv) {
	    rs = SR_INVALID ;
	    if (pfn[0]) {
	        char *const *cav = (char *const *) argv ;
	        char *const *cev = (char *const *) envv ;
	        int	to_again = utimeout[uto_again] ;
	        bool	fexit = false ;
	        repeat {
	            if ((rs = isaexec(pfn,cav,cev)) < 0) {
		        rs = (neg errno) ;
	                switch (rs) {
	                case SR_AGAIN:
		            if (to_again-- > 0) {
	                        msleep(1000) ;
	 	            } else {
		                fexit = true ;
		            }
		            break ;
	                case SR_INTR:
	                    break ;
	                default:
		            fexit = true ;
		            break ;
	                } /* end switch */
	            } /* end if (error) */
	        } until ((rs >= 0) || fexit) ;
	        if ((rs == SR_NOENT) || (rs == SR_NOSYS)) {
	            rs = u_execve(pfn,argv,envv) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (uc_isaexecve) */


