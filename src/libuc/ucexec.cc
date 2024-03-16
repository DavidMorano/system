/* ucexec SUPPORT */
/* lang=C++20 */

/* interface component for UNIX� library-3c */
/* version %I% last-modified %G% */

#define	CF_ISAEXEC	1		/* try ISA-EXEC */

/* revision history:

	= 1998-11-28, David A�D� Morano
	How did we get along without this for over 10 years?

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	For now, this just is a regular (fairly so at any rate)
	|exec(2)|-like subroutine.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<libmallocxx.h>


/* local defines */


/* external subroutines */

extern "C" {
    extern int	mkexpandpath(char *,cchar *,int) noex ;
}


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_execve(cchar *fname,mainv av,mainv ev) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rv = 0 ;
	if (fname && av && ev) {
	    char	*ebuf{} ;
	    if ((rs = libmalloc_mp(&ebuf)) >= 0) {
	        if ((rs = mkexpandpath(ebuf,fname,-1)) > 0) {
		    rs = u_execve(ebuf,av,ev) ;
		    rv = rs ;
	        } else if (rs == 0) {
		    rs = u_execve(fname,av,ev) ;
		    rv = rs ;
	        }
	        rs1 = uc_libfree(ebuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (uc_execve) */


