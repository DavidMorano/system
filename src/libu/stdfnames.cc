/* stdfname SUPPORT */
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
	stdfname

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
#include	<usysbase.h>
#include	<usupport.h>
#include	<stdfiles.h>
#include	<localmisc.h>

#include	"stdfnames.h"


/* local defines */


/* imported namespaces */

using libu::matstr ;			/* subroutine */


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

ccharp		stdfname[stdfile_overlast + 1] ;

extern ccharp	stdfnames[stdfile_overlast + 1] ;

ccharp		stdfnames[stdfile_overlast + 1] = {
	"*STDIN*",
	"*STDOUT*",
	"*STDERR*",
	"*STDLOG*",
	"*STDNULL*",
	"*STDZERO*",
	"-",
	nullptr
} ; /* end array */


/* exported subroutines */

int getstdfname(cchar *sp,int sl) noex {
    	int		rs = SR_FAULT ;
	if (sp) {
	    rs = matstr(stdfname,sp,sl) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (getstddname) */


/* local subroutines */

stdfnamer::stdfnamer() noex {
    	ccharp *n = stdfname ;
	n[stdfile_in]		= "*STDIN*" ;
	n[stdfile_out]		= "*STDOUT*" ;
	n[stdfile_err]		= "*STDERR*" ;
	n[stdfile_log]		= "*STDLOG*" ;
	n[stdfile_null]		= "*STDNULL*" ;
	n[stdfile_zero]		= "*STDZERO*" ;
	n[stdfile_minus]	= "-" ;
	n[stdfile_overlast]	= nullptr ;
} /* end ctor (stdfnamer) */


