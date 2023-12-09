/* uc_getcwd */
/* lang=C20 */

/* get the current working directory */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine returns the Current Working Directory (CWD).
	If you wanted the Present Working Directory (PWD), you
	should be calling |getpwd(3uc)|.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<string.h>		/* <- |strnlen(3c)| */
#include	<errno.h>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */


/* local namespaces */


/* external subroutines */


/* exported subroutines */

int uc_getcwd(char *cwbuf,int cwlen) noex {
	int		rs = SR_FAULT ;
	if (cwbuf) {
	    if (getcwd(cwbuf,cwlen) != nullptr) {
	        rs = strnlen(cwbuf,cwlen) ;
	    } else {
	        rs = (- errno) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_getcwd) */


