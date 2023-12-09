/* sysdbfnames */
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

	This modeule provides that c-strings for the standard file
	names.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>

#include	"sysdbfnames.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* subroutine templates */


/* exported variables */

cchar	*sysdbfnames[] = {
	[sysdbfile_passwd] 	= "passwd",
	[sysdbfile_shadow] 	= "shadow",
	[sysdbfile_userattr] 	= "userattr",
	[sysdbfile_group] 	= "group",
	[sysdbfile_project] 	= "project",
	[sysdbfile_protocols] 	= "protocols",
	[sysdbfile_networks] 	= "networks",
	[sysdbfile_hosts] 	= "hosts",
	[sysdbfile_services] 	= "services",
	[sysdbfile_shells] 	= "shells",
	nullptr
} ;


/* exported subroutines */


