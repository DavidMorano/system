/* matlocalfs HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* determine is the given counted c-string is a local-filesystem name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MATLOCALFS_INCLUDE
#define	MATLOCALFS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


enum localfses {
	localfs_apfs,
	localfs_ext1,
	localfs_ext2,
	localfs_ext3,
	localfs_ext4,
	localfs_xfs,
	localfs_zfs,
	localfs_btrfs,
	localfs_tmpfs,
	localfs_autofs,
	localfs_nfs,
	localfs_devfs,
	localfs_lofs,
	localfs_fat,
	localfs_exfat,			/* ExFAT */
	localfs_vfat,			/* FAT32 */
	localfs_msdos,
	localfs_ntfs,
	localfs_cd9660,			/* ISO 9660 */
	localfs_udf,			/* Universal Disk Format */
	localfs_ufs,
	localfs_vxfs,
	localfs_pcfs,
	localfs_smbfs,
	localfs_hfs,
	localfs_sysv,
	localfs_overlast
} ; /* end enum (localfses) */

EXTERNC_begin

extern int	matlocalfs(cchar *sp,int sl) noex ;

EXTERNC_end


#endif /* MATLOCALFS_INCLUDE */


