/* opensysdbs SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* database name for use w/ the |opensysfs(3uc)| subroutine */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	opensysdbs

	Description:
	This is a C-lang array of c-strings of database names for
	use w/ the |opensysfs(3uc)| subroutine.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<localmisc.h>

#include	"opensysfs.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */

cpcchar		opensysdbs[] = {
	[opensysdbi_userhomes]		= "userhomes",
	[opensysdbi_usernames]		= "usernames",
	[opensysdbi_groupnames]		= "groupnames",
	[opensysdbi_projectnames]	= "projectnames",
	[opensysdbi_passwd]		= "passwd",
	[opensysdbi_group]		= "group",
	[opensysdbi_project]		= "project",
	[opensysdbi_realname]		= "realname",
	[opensysdbi_shells]		= "shells",
	[opensysdbi_shadow]		= "shadow",
	[opensysdbi_userattr]		= "userattr",
	[opensysdbi_overlast]		= (cchar *) nullptr
} ;


/* exported subroutines */


