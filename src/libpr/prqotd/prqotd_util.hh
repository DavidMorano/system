/* prqotd_util SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* open a channel (file-descriptor) to the quote-of-the-day (QOTD) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	prqotd_util

	Description:
	This subroutine sort of forms the back-end maintenance end
	of the QOTD mechansim.

	Synopsis:

	Arguments:

	Returns:
	>=0		FD of QOTD
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>

#include	"prqotd.h"


namespace prqotd {
    struct initer {
	operator int () noex ;
    } ; /* end struct (initer) */
    struct vars {
	mainv	envv ;
	int	pid ;
	int	maxnamelen ;
	int	maxpathlen ;
	int	nodenamelen ;
	int	hostnamelen ;
	int	ebuflen ;
	int	vbuflen ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace (prqotd) */

namespace prqotd {
    extern int	getdefmjd(time_t) noex ;
    extern int	mkqfname(char *,cchar *,int) noex ;
    extern int	mkourname(char *,cchar *,cchar *,cchar *,int) noex ;
} /* end namespace (prqotd) */

namespace prqotd {
    extern vars		var ;
    extern initer	init ;
} /* end namespace (prqotd) */


