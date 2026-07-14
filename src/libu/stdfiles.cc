/* stdfiles SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* standard file-names */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-03-24, David A­D­ Morano
	This code is newly written but modeled after some code that
	was originally written to get some standardized variable
	serialization for the Ethernet-Controller (ETHCON) development
	work.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Variable:
	stdfile

	Description:
	This module provides c-strings for the standard file names.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<stdfiles.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"stdfiles.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */

consteval stdfiler::stdfiler() noex {
	name[stdfile_in]	= "/dev/fd/0" ;
	name[stdfile_out]	= "/dev/fd/1" ;
	name[stdfile_err]	= "/dev/fd/2" ;
	name[stdfile_log]	= "/dev/fd/3" ;
	name[stdfile_null]	= "/dev/null" ;
	name[stdfile_zero]	= "/dev/zero" ;
	name[stdfile_minus]	= "/dev/stdin" ;
	name[stdfile_overlast]	= nullptr ;
} /* end ctor */

constexpr stdfiler	stdfile ;


/* exported subroutines */


