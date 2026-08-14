/* prenvfile SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* process an environment file */
/* version %I% last modified %G% */


/* revision history:

	= 1994-09-10, David Morano
	This program was originally written.

*/

/* Copyright © 1994 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	prenvfile

  	Description:
	This subroutine will read (process) an environment file and
	put all of the environment variables into the string list
	(supplied).  New environment variables just get added to
	the list.  Old environment variables already on the list
	are deleted when a new definition is encountered.

	Synopsis:
	int prenvfile(cchar *pr,cchar *fname,vecstr *lp) noex

	Arguments:
	pr 		program root
	fname		filename to process
	lp		resulting list of environment variables

	Returns:
	>=0		number of environment variables
	<0		error (system-error)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG */

#include	"prenvfile.h"


/* local defines */


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* forward references */


/* local structures */


/* local variables */


/* exported variables */


/* exported subroutines */

int prenvfile(cc *pr,vecstr *lp,char *fn) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		n = 0 ; /* return-value */
	if (pr && lp && fn) ylikely {
	    rs = SR_INVALID ;
	    if (pr[0] && fn[0]) ylikely {
		if (char *pbuf ; (rs = mem.mp(&pbuf)) >= 0) ylikely {
		    if ((rs = mkpath(pbuf,pr,fn)) >= 0) ylikely {
		        rs = lp->envfile(pbuf) ;
		        n = rs ;
		    }
		    rs1 = mem.free(pbuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
} /* end subroutine (prenvfile) */


