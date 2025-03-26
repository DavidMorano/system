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


enum opensysdbis {
	opensysdbi_userhomes,
	opensysdbi_usernames,
	opensysdbi_groupnames,
	opensysdbi_projectnames,
	opensysdbi_passwd,
	opensysdbi_group,
	opensysdbi_project,
	opensysdbi_realname,
	opensysdbi_shells,
	opensysdbi_shadow,
	opensysdbi_userattr,
	opensysdbi_overlast
} ;

extern cpcchar		opensysdbs[] ;


#endif /* OPENSYSDBS_INCLUDE */


