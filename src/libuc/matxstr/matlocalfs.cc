/* matlocalfs SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* determine is the given counted c-string is a local-filesystem name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	matlocalfs

	Description:
	This subroutine determines if the given counted c-string
	matches up against (fixed at the moment) list of file-sytem
	name that are assume to be local file-systems.

	Synopsis:
	int matlocalfs(cchar *,sp,int sl) noex

	Arguments:
	sp		name pointer
	sl		name length

	Returns:
	>0		Yes, on a remote file-system
	==0		not on remote file-system
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* LIBU |UCHAR_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<algorithm>		/* C++STD |sort(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<strwcmp.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"matlocalfs.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| + |cstrcmp(3u)| */

/* local defines */


/* imported namespaces */

using std::sort ;			/* subroutine-template */
using std::partition_point ;		/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward refernces */


/* local variables */

constexpr int		ne_fs	= localfs_overlast ;

namespace {
    struct namemgr {
	cchar		*name	[ne_fs] ;
	uchar		tab_fs	[ne_fs] ;
	consteval void tabload_x() noex {
	    for (int i = 0 ; i < ne_fs ; i += 1) {
		tab_fs[i] = uchar(i) ;
	    } /* end for */
	} ; /* end method (tabload_x) */
	consteval void tabinit() noex {
	    cauto pred_sort = [this] (cuchar &ia,cuchar &ib) noex -> bool {
		cchar *s1 = name[ia] ;
		cchar *s2 = name[ib] ;
    		return (cstrcmp(s1,s2) < 0) ;
	    } ; /* end lambda */
	    tabload_x() ;
	    sort(tab_fs,(tab_fs+ne_fs),pred_sort) ;
	} ; /* end method (tabinit) */
	consteval void mknames() noex {
	    name[localfs_apfs]	= "apfs" ;
	    name[localfs_ext1]	= "ext1" ;
	    name[localfs_ext2]	= "ext2" ;
	    name[localfs_ext3]	= "ext3" ;
	    name[localfs_ext4]	= "ext4" ;
	    name[localfs_xfs]	= "xfs" ;
	    name[localfs_zfs]	= "zfs" ;
	    name[localfs_btrfs]	= "btrfs" ;
	    name[localfs_tmpfs]	= "tmpfs" ;
	    name[localfs_autofs] = "autofs" ;
	    name[localfs_nfs]	= "nfs" ;
	    name[localfs_devfs]	= "devfs" ;
	    name[localfs_lofs]	= "lofs" ;
	    name[localfs_fat]	= "fat" ;
	    name[localfs_exfat]	= "exfat" ;	/* ExFAT */
	    name[localfs_vfat]	= "vfat" ;	/* FAT32 */
	    name[localfs_msdos]	= "msdos" ;
	    name[localfs_ntfs]	= "ntfs" ;
	    name[localfs_cd9660] = "cd9660" ;	/* ISO 9660 */
	    name[localfs_udf]	= "udf" ;	/* Universal Disk Format */
	    name[localfs_ufs]	= "ufs" ;
	    name[localfs_vxfs]	= "vxfs" ;
	    name[localfs_pcfs]	= "pcfs" ;
	    name[localfs_smbfs]	= "smbfs" ;
	    name[localfs_hfs]	= "hfs" ;
	    name[localfs_sysv]	= "sysv" ;
	} ; /* end method */
	consteval namemgr() noex {
	    mknames() ;
	    tabinit() ;
	} /* end ctor (namemgr) */
	int operator () (cchar *sp,int sl) const noex {
            int     ei = -1 ; /* return-value */
            cauto pred_mat = [this,sp,sl] (uchar c) noex -> bool {
                cchar *an = name[c] ;
		bool f = false ;
		{
		    cuchar uch_a = uchar(an[0]) ;
		    cuchar uch_s = uchar(sp[0]) ;
		    if ((f = (uch_a < uch_s)) == false) {
			f = ((uch_a == uch_s) && (strwcmp(an,sp,sl) < 0)) ;
		    }
		}
                return f ;
            } ; /* end lambda (pred_mat) */
            con uchar *itf = (tab_fs + 0) ;
            con uchar *itl = (tab_fs + ne_fs) ;
            if (cauto it = partition_point(itf,itl,pred_mat) ; it != itl) {
                cauto mat = [this,sp,sl] (int c) noex -> bool {
                    cchar *an = name[c] ;
                    return (strwcmp(an,sp,sl) == 0) ;
                } ; /* end lambda (mat) */
                if (cint ii = *it ; mat(ii)) {
		    ei = ii ;
                } /* end if (got a match) */
            } /* end if (partition_point) */
            return ei ;
	} ; /* end method (operator) */
    } ; /* end struct (namemgr) */
} /* end namespace */

constexpr namemgr	nametab ;


/* exported variables */


/* exported subroutines */

int matlocalfs(cchar *sp,int µsl) noex {
	int		idx = -1 ;
	if (int sl ; sp && (sl = getlenstr(sp,µsl)) > 0) {
 	    idx = nametab(sp,sl) ;
	}
	return idx ;
} /* end subroutine (matlocalfs) */


