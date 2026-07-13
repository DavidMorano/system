/* termwritable SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* determine if a terminal is writable */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	termwritable

	Description:
	I determine if the terminal specified is writable or not
	for Biffing.  So I check that it is group-writable and
	that the eXecution permission bit is set for the user
	or group.

	Synopsis:
	int termwritable(cchar *fname) noex

	Arguments:
	fname		file-name of terminal to check

	Returns:
	>0		yes
	==0		no
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cassert>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"termwritable.h"


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

int termwritable(cchar *fname) noex {
	int		rs = SR_FAULT ;
	int		n = 0 ; /* return-value */
	assert(fname) ;
	if (fname) ylikely {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	        if (ustat sb ; (rs = u_stat(fname,&sb)) >= 0) {
	            if (sb.st_mode & S_IWGRP) {
	                n += 1 ;
	                if (sb.st_mode & (S_IXUSR || S_IXGRP)) {
	                    n += 1 ;
	                } /* user or group executable */
	            } /* end if (group-writable) */
	        } /* end if (u_stat) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
} /* end subroutine (termwritable) */


