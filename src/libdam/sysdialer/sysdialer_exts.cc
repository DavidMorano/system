/* sysdialer_exts SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* system-dialer storage object */
/* version %I% last-modified %G% */


/* revision history:

	- 2003-11-04, David A­D­ Morano
	This subroutine was adopted for use from the DWD program.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	sysdialer

	Description:
	This object manages what system-dialers (SYSDIALER) have
	been loaded so far.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<ctime>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<usupport.h>		/* |getustime(3u)| */
#include	<uclibmem.h>
#include	<localmisc.h>

#include	"sysdialer.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */

namespace sysdialer_util {
    cpcchar	exts[] = {
	"dylib",
	"so",
	"o",
	"",
	nullptr
    } ; /* end array */
} /* end namespace (sysdialer_util) */


/* exported subroutines */


