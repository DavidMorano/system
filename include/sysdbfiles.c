/* sysdbfiles SUPPORT */
/* encoding=ISO8859-1 */
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
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>

#include	"sysdbfiles.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */

cpcchar		sysdbfile[] = {
	[sysdbfile_pw] 		= "passwd",
	[sysdbfile_sp] 		= "shadow",
	[sysdbfile_ua] 		= "userattr",
	[sysdbfile_gr] 		= "group",
	[sysdbfile_pj] 		= "project",
	[sysdbfile_pr] 		= "protocols",
	[sysdbfile_nw] 		= "networks",
	[sysdbfile_nm] 		= "netmasks",
	[sysdbfile_ho] 		= "hosts",
	[sysdbfile_sv] 		= "services",
	[sysdbfile_sh] 		= "shells",
	[sysdbfile_overlast]	= nullptr
} ; /* end array (sysdbfile) */


/* exported subroutines */


