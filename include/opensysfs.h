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
#ifdef	__cplusplus /* C++ only */


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<opensysdbs.h>		/* Open-System Data-Base names */


#define	OPENSYSFS_SYSDNAME	"/tmp/sysdb"
#define	OPENSYSFS_DEFTTL	(2*3600)

namespace libuc {
    extern int opensysfs(opensysdbs,int,int) noex ;
} /* end namespace (libuc) */

#define	OPENSYSFS_WUSERHOMES	opensysdb_userhomes
#define	OPENSYSFS_WUSERNAMES	opensysdb_usernames
#define	OPENSYSFS_WGROUPNAMES	opensysdb_groupnames
#define	OPENSYSFS_WPROJECTNAMES	opensysdb_projectnames
#define	OPENSYSFS_WPASSWD	opensysdb_passwd
#define	OPENSYSFS_WSHADOW	opensysdb_shadow
#define	OPENSYSFS_WUSERATTR	opensysdb_userattr
#define	OPENSYSFS_WGROUP	opensysdb_group
#define	OPENSYSFS_WPROJECT	opensysdb_project
#define	OPENSYSFS_WREALNAME	opensysdb_realname
#define	OPENSYSFS_WSHELLS	opensysdb_shells
#define	OPENSYSFS_WOVERLAST	opensysdb_overlast

#define	OPENSYSFS_FUSERHOMES	opensysdb[opensysdb_userhomes]
#define	OPENSYSFS_FUSERNAMES	opensysdb[opensysdb_usernames]
#define	OPENSYSFS_FGROUPNAMES	opensysdb[opensysdb_groupnames]
#define	OPENSYSFS_FPROJECTNAMES	opensysdb[opensysdb_projectnames]
#define	OPENSYSFS_FPASSWD	opensysdb[opensysdb_passwd]
#define	OPENSYSFS_FSHADOW	opensysdb[opensysdb_shadow]
#define	OPENSYSFS_FUSERATTR	opensysdb[opensysdb_userattr]
#define	OPENSYSFS_FGROUP	opensysdb[opensysdb_group]
#define	OPENSYSFS_FPROJECT	opensysdb[opensysdb_project]
#define	OPENSYSFS_FREALNAME	opensysdb[opensysdb_realname]
#define	OPENSYSFS_FSHELLS	opensysdb[opensysdb_shells]


#endif /* __cplusplus (C++ only) */
#endif /* OPENSYSFS_INCLUDE */


