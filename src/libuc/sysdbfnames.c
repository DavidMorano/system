/* sysdbfnames SUPPORT */
/* lang=C20 */

/* standard file-names */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-03-24, David A­D­ Morano
	This code is newly written but modeled after some code that
	was originally written for the Ethernet-Controller (ETHCON)
	development work.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This modeule provides that c-strings for the standard file
	names.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>

#include	"sysdbfnames.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* subroutine templates */


/* exported variables */

cchar	*sysdbfnames[] = {
	[sysdbfile_pw] 		= "passwd",
	[sysdbfile_sp] 		= "shadow",
	[sysdbfile_ua] 		= "userattr",
	[sysdbfile_gr] 		= "group",
	[sysdbfile_pj] 		= "project",
	[sysdbfile_pr] 		= "protocols",
	[sysdbfile_nw] 		= "networks",
	[sysdbfile_ho] 		= "hosts",
	[sysdbfile_sv] 		= "services",
	[sysdbfile_sh] 		= "shells",
	nullptr
} ;


/* exported subroutines */


