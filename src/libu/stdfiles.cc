/* stdfiles SUPPORT */
/* charset=ISO8859-1 */
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
	stdfile

	Description:
	This module provides that c-strings for the standard file
	names.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<stdfiles.h>
#include	<localmisc.h>

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


/* exported subroutines */

#ifdef	COMMENT

    struct stdfiler {
	cchar	*name[stdfile_overlast] = {} ;
	consteval stdfiler() noex {
	    name[stdfile_in]	= "/dev/fd/0" ;
	    name[stdfile_out]	= "/dev/fd/1" ;
	    name[stdfile_err]	= "/dev/fd/2" ;
	    name[stdfile_log]	= "/dev/fd/3" ;
	    name[stdfile_null]	= "/dev/null" ;
	    name[stdfile_zero]	= "/dev/zero" ;
	    name[stdfile_minus]	= "/dev/stdin" ;
	} ; /* end ctor */
    } ; /* end struct (devnames) */

#endif /* COMMENT */


