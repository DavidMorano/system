/* shellunder SUPPORT */
/* lang=C++20 */

/* parse out the shell-under information (as might be present) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	shellunder

	Description:
	This subroutine will parse out the shell-under information
	from a given string (containing the shell-under information).

	Synopsis:
	int shellunder_wr(shellunder_dat *op,char *under) noex

	Arguments:
	op		pointer to object
	under		given string containing shell-under information

	Returns:
	>=0	length of program-execution filename
	<0	error (system-return)

	= Implementation notes:
	The "shell-under" string takes the form:
		[*<ppid>*]<progename>
	where:
	ppid		PID of parent (shell) process
	progename	program exec-name of child process

	Enjoy.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* system-types */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstring>		/* <- |strchr(3c)| */
#include	<usystem.h>
#include	<cfdec.h>
#include	<char.h>
#include	<localmisc.h>

#include	"shellunder.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int shellunder_wr(shellunder_dat *op,cchar *under) noex {
	int		rs = SR_FAULT ;
	int		pl = 0 ;
	if (op && under) {
	    rs = SR_OK ;
	    op->pid = -1 ;
	    op->progename = nullptr ;
	    if (under[0] != '\0') {
	        if (under[0] == '*') {
	            int		dl = -1 ;
	            cchar	*dp = (under+1) ;
	            cchar	*tp = strchr(under,'*') ;
	            if (tp) {
	                dl = (tp-dp) ;
	                under = (tp+1) ;
	                if (int v ; (rs = cfdeci(dp,dl,&v)) >= 0) {
	                    op->pid = pid_t(v) ;
	                }
	            } else {
	                rs = SR_INVALID ;
		    }
	        } /* end if */
	        if ((rs >= 0) && (under[0] != '\0')) {
	            while (CHAR_ISWHITE(*under)) {
			under += 1 ;
		    }
	            op->progename = under ;
	            pl = strlen(under) ;
	        }
	    } /* end if (non-zero) */
	} /* end if (non-null) */
	return (rs >= 0) ? pl : rs ;
}
/* end subroutine (shellunder_load) */


