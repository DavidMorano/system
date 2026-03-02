/* debugline SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* debugging stubs */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-15, David A­D­ Morano
	This was written to debug the REXEC program.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	debugline

	Description:
	This modeule provides debugging support.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdarg>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<usupport.h>		/* |strwcpy(3u)| */
#include	<uclibmem.h>
#include	<snflagsx.h>
#include	<localmisc.h>		/* |COLUMNS| */

#include	"debugline.h"
#include	"libdebug.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */

using libu::strwcpy ;			/* subroutine */


/* external subroutines */


/* external variables */


/* local structures */


/* forward subroutines */


/* local variables */


/* exported variables */


/* exported subroutines */


