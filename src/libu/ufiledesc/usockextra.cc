/* usockextra SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Group:
	u_sockextra{x}

	Description:
	This provides special control for devices.

	Synopsis:
	int u_sockextra{x}(int fd,int cmd,...) noex

	Arguments:
	fd		file-descriptor
	cmd		command (request)
	...		any-argument

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdint>		/* CSTD |uintptr_t| */
#include	<cstdarg>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<usysflag.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"ufiledescbase.hh"
#include	"usockextra.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward refeferences */


/* local variables */


/* exported variables */


/* exported subroutines */

int u_linger(int fd,int to) noex {
	int		rs ;
	if (ustat sb ; (rs = u_fstat(fd,&sb)) >= 0) {
	    if (S_ISSOCK(sb.st_mode)) {
	        LINGER	ls{} ;
		cint	sol = SOL_SOCKET ;
		cint	cmd = SO_LINGER ;
		cint	llen = szof(LINGER) ;
		if (to >= 0) {
	            ls.l_onoff = true ;
	            ls.l_linger = to ;
		}
		{
		    cchar	*obuf = charp(&ls) ;
	            rs = u_sockoptset(fd,sol,cmd,obuf,llen) ;
		}
	    } /* end if (socket) */
	} /* end if (stat) */
	return rs ;
} /* end subroutine (u_linger) */


/* local subroutines */


