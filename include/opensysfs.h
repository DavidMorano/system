/* opensysfs HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* Open-System-File-System (OpenSysFS) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	OPENSYSFS_INCLUDE
#define	OPENSYSFS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<opensysdbs.h>		/* Open-system Data-Base names */


#define	OPENSYSFS_SYSDNAME	"/tmp/sysdb"
#define	OPENSYSFS_DEFTTL	(2*3600)

#define	OPENSYSFS_FUSERHOMES	opensysdbs[opensysdbi_userhomes]
#define	OPENSYSFS_FUSERNAMES	opensysdbs[opensysdbi_usernames]
#define	OPENSYSFS_FGROUPNAMES	opensysdbs[opensysdbi_groupnames]
#define	OPENSYSFS_FPROJECTNAMES	opensysdbs[opensysdbi_projectnames]
#define	OPENSYSFS_FPASSWD	opensysdbs[opensysdbi_passwd]
#define	OPENSYSFS_FGROUP	opensysdbs[opensysdbi_group]
#define	OPENSYSFS_FPROJECT	opensysdbs[opensysdbi_project]
#define	OPENSYSFS_FREALNAME	opensysdbs[opensysdbi_realname]
#define	OPENSYSFS_FSHELLS	opensysdbs[opensysdbi_shells]
#define	OPENSYSFS_FSHADOW	opensysdbs[opensysdbi_shadow]
#define	OPENSYSFS_FUSERATTR	opensysdbs[opensysdbi_userattr]

#define	OPENSYSFS_WUSERHOMES	opensysdbi_userhomes
#define	OPENSYSFS_WUSERNAMES	opensysdbi_usernames
#define	OPENSYSFS_WGROUPNAMES	opensysdbi_groupnames
#define	OPENSYSFS_WPROJECTNAMES	opensysdbi_projectnames
#define	OPENSYSFS_WPASSWD	opensysdbi_passwd
#define	OPENSYSFS_WGROUP	opensysdbi_group
#define	OPENSYSFS_WPROJECT	opensysdbi_project
#define	OPENSYSFS_WREALNAME	opensysdbi_realname
#define	OPENSYSFS_WSHELLS	opensysdbi_shells
#define	OPENSYSFS_WSHADOW	opensysdbi_shadow
#define	OPENSYSFS_WUSERATTR	opensysdbi_userattr
#define	OPENSYSFS_WOVERLAST	opensysdbi_overlast

EXTERNC_begin

extern int opensysfs(int,int,int) noex ;

EXTERNC_end


#endif /* OPENSYSFS_INCLUDE */


