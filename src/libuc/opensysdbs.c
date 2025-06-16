/* opensysdbs SUPPORT */
/* charset=ISO8859-1 */
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
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>

#include	"opensysdbs.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */

cpcchar		opensysdb[] = {
	[opensysdb_userhomes]		= "userhomes",
	[opensysdb_usernames]		= "usernames",
	[opensysdb_groupnames]		= "groupnames",
	[opensysdb_projectnames]	= "projectnames",
	[opensysdb_passwd]		= "passwd",
	[opensysdb_shadow]		= "shadow",
	[opensysdb_userattr]		= "userattr",
	[opensysdb_group]		= "group",
	[opensysdb_project]		= "project",
	[opensysdb_realname]		= "realname",
	[opensysdb_shells]		= "shells",
	[opensysdb_overlast]		= nullptr
} ; /* end array (opensysdb) */


/* exported subroutines */


