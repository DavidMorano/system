/* stdfnames SUPPORT */
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
	stdfnames

	Description:
	This module provides that c-strings for the standard file
	names.

	Notes:
	1. This is written in the C-language because supposedly
	(actually really, according to the standard) the C++ language
	is not supposed to allow for aggregate array initialization.
	Ya, that is how dumb C++ can be.  But as you probably already
	know, I have aggregate array intialization elsewhere.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<stdfiles.h>
#include	<localmisc.h>

#include	"stdfnames.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct stdfnamer {
	stdfnamer() noex ;
    } ; /* end struct (stdfnamer) */
} /* end namespace */


/* forward references */


/* local variables */

const stdfnamer		stdfname_data ;


/* exported variables */

ccharp		stdfnames[stdfile_overlast + 1] ;


/* exported subroutines */


/* local subroutines */

stdfnamer::stdfnamer() noex {
	stdfnames[stdfile_in]		= "*STDIN*" ;
	stdfnames[stdfile_out]		= "*STDOUT*" ;
	stdfnames[stdfile_err]		= "*STDERR*" ;
	stdfnames[stdfile_log]		= "*STDLOG*" ;
	stdfnames[stdfile_null]		= "*STDNULL*" ;
	stdfnames[stdfile_zero]		= "*STDZERO*" ;
	stdfnames[stdfile_minus]	= "-" ;
	stdfnames[stdfile_overlast]	= nullptr ;
} /* end ctor (stdfnamer) */


