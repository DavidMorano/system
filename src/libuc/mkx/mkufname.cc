/* mkufname SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make a "user" file-name (whatever that is) */
/* version %I% last-modified %G% */


/* revision history:

	= 1995-04-01, David A­D­ Morano
	This is part of our cleanup-compatibility effort.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkufname

	Description:
	Make a "user" file-name (whatever that is -- see the code below).

	Synopsis:
	int mkufname(char *rbuf,int rlen,cchar *hn,cchar *un) noex

	Arguments:
	rbuf		result buffer pointer
	rlen		result buffer length
	hn		dirname of home directory
	un		username

	Results:
	>=0		number of bytes in result
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<mkpathx.h>
#include	<localmisc.h>

#include	"mkx.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* local variables */


/* exported variables */


/* exported subroutines */

int mkufname(char *rbuf,cchar *hn,cchar *rc) noex {
	int		rs = SR_FAULT ;
	if (rbuf && hn && rc) ylikely {
	    rs = SR_INVALID ;
	    if (hn[0] && rc[0]) ylikely {
		bool	f = true ;
		rs = SR_OK ;
		f = f && (rc[0] != '/') ;
		f = f && (! ((rc[0] == '.') && (rc[1] == '/'))) ;
		f = f && (! ((rc[0] == '.') && (rc[1] == '.'))) ;
		if (f) {
	            rs = mkpath(rbuf,hn,rc) ;
		} else {
		    rs = mkpath(rbuf,rc) ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mkufname) */



