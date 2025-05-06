/* opensysdbs HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* database name for use w/ the |opensysfs(3uc)| subroutine */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	OPENSYSDBS_INCLUDE
#define	OPENSYSDBS_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<stddef.h>		/* ?? */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


enum opensysdbs {
	opensysdb_userhomes,
	opensysdb_usernames,
	opensysdb_groupnames,
	opensysdb_projectnames,
	opensysdb_passwd,
	opensysdb_group,
	opensysdb_project,
	opensysdb_realname,
	opensysdb_shells,
	opensysdb_shadow,
	opensysdb_userattr,
	opensysdb_overlast
} ; /* end enum (opensysdbs) */

extern cpcchar		opensysdb[] ;


#endif /* OPENSYSDBS_INCLUDE */


