/* stdfnames SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C20 */

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
	This modeule provides that c-strings for the standard file
	names.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>

#include	"stdfnames.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */

cchar	*const stdfnames[] = {
	[stdfile_in]	= "*STDIN*",
	[stdfile_out]	= "*STDOUT*",
	[stdfile_err]	= "*STDERR*",
	[stdfile_log]	= "*STDLOG*",
	[stdfile_null]	= "*STDNULL*",
	nullptr
} ;


/* exported subroutines */


