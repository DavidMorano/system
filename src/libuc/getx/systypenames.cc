/* tsystypenams SUPPORT (System-Type-Nnames) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* system type numbers (and associated strings) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	systypenames

	Description:
	Currently this code does nothing more than initialize the
	'systypenames' array with its values.  This happens at
	load-time.  So this array (a global variable) cannot be
	used until the module containing this code is loaded.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<localmisc.h>

#include	"systypenames.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct systyper {
	systyper() noex ;
    } ; /* end struct (systyper) */
} /* end namespace */


/* forward references */


/* local variables */

const systyper	systype_data ;

ccharp		systypename[systypename_overlast + 1] ;


/* exported variables */


/* local subroutines */

systyper::systyper() noex {
       	systypename[systypename_darwin]		= "Darwin" ;
	systypename[systypename_linux]		= "Linux" ;
	systypename[systypename_sysv]		= "SysV" ;
	systypename[systypename_overlast]	= nullptr ;
} /* end ctor (systyper) */


