/* shellunder SUPPORT */
/* charset=ISO8859-1 */
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
	shellunder_load

	Description:
	This subroutine will parse out the shell-under information
	from a given string (containing the shell-under information).

	Synopsis:
	int shellunder_load(shellunder_dat *op,char *under) noex

	Arguments:
	op		pointer to object
	under		given string containing shell-under information

	Returns:
	>=0		length of program-execution filename
	<0		error (system-return)

	= Implementation notes:
	The "shell-under" string takes the form:
		[*<ppid>*]<execname>
	where:
	ppid		PID of parent (shell) process
	progename	program exec-name of child process

	Enjoy.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* system-types */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strchr(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usupport.h>		/* LIBU |cfdec(3u)| */
#include	<localmisc.h>		/* LIBU */

#include	"shellunder.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/ureserve.ccm"

import libutil ;			/* |lenstr(3u)| */
import ureserve ;			/* |char{x}(3u)| */

/* local defines */

#define	ISWHT(ch)	char_iswhite(ch)


/* imported namespaces */

using libu::cfdec ;			/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int shellunder_load(shellunder_dat *op,cchar *under) noex {
	int		rs = SR_FAULT ;
	int		pl = 0 ;
	if (op && under) ylikely {
	    rs = SR_OK ;
	    op->pid = -1 ;
	    op->execname = nullptr ;
	    if (under[0] != '\0') ylikely {
	        if (under[0] == '*') {
	            cchar *dp = (under + 1) ;
	            if (cchar *tp = strchr(dp,'*') ; tp) {
	                cint dl = intconv(tp - dp) ;
	                under = (tp+1) ;
	                if (int v ; (rs = cfdec(dp,dl,&v)) >= 0) {
	                    op->pid = pid_t(v) ;
	                }
	            } else {
	                rs = SR_INVALID ;
		    }
	        } /* end if (bad a '*') */
	        if ((rs >= 0) && (under[0] != '\0')) {
	            while (ISWHT(*under)) {
			under += 1 ;
		    }
	            op->execname = under ;
	            pl = lenstr(under) ;
	        }
	    } /* end if (non-zero) */
	} /* end if (non-null) */
	return (rs >= 0) ? pl : rs ;
} /* end subroutine (shellunder_load) */


