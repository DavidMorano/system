/* getfname SUPPORT */
/* lang=C++20 */

/* get a file name according to rules */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getfname

	Description:
	This subroutine will form a file name according to some rules.
	This subroutine is used (among others) to find files that
	are specified in configuration files and are supposed to
	be rooted at one place or another in the file system.
	If a type of GETFNAME_TYPELOCAL is given as an argument,
	the file is searched locally before being searched in the
	program root area. If the type is given as GETFNAME_TYPEROOT,
	the file is searched for in the program root area first and
	then locally.

	Synopsis:
	int getfname(cchar *pr,char *rbuf,int type,cchar *fname) noex

	Arguments:
	pr		base directory path to check in
	rbuf		the output file buffer (user supplied)
	type		the type of the check to make
		0	search locally first
		1	search in the program root area first
	fname		the name of the input file to check for

	Returns:
	>0		try file at path in 'rbuf'
	0		file-name is already absolute (but not tested)
	<0		error (system-return)

	Usage-note:
	The size of the receiving buffer (represented w/ the variable
	|rbuf| above) must be the length of the maximum file-path
	length plus one (for a terminating NUL character).

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<mkpathx.h>
#include	<xperm.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"getfname.h"


/* local defines */

#ifndef	GETFNAME_TYPELOCAL
#define	GETFNAME_TYPELOCAL	0	/* search locally first */
#define	GETFNAME_TYPEROOT	1	/* search pr area first */
#endif


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct subinfo {
	cchar		*pr ;
	cchar		*fname ;
	char		*rbuf ;
	int		am ;
	bool		ft ;
	subinfo(cc *p,char *r,int t,cc *f,int a) noex {
	    pr = p ;
	    rbuf = r ;
	    fname = f ;
	    am = (a & O_ACCMODE) ;
	    ft = bool(t) ;
	} ;
	operator int () noex ;
	int round() noex ;
	int rem() noex ;
	int loc() noex ;
    } ; /* end struct (subinfo) */
}


/* forward references */


/* local variables */

constexpr int		rsn = SR_NOTFOUND ;


/* exported variables */


/* exported subroutines */

int getfname(cchar *pr,char *rbuf,int type,cchar *fname,int am) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (pr && rbuf && fname) {
	    rbuf[0] = '\0' ;
	    rs = SR_INVALID ;
	    if ((type >= 0) && fname[0]) {
		rs = SR_OK ;
	        if (fname[0] != '/') {
		    subinfo	si(pr,rbuf,type,fname,am) ;
		    rs = si ;
		    len = rs ;
		} /* end if (relative file-name) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getrbuf) */


/* local subroutines */

subinfo::operator int () noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	for (int i = 0 ; (rs >= 0) && (i < 2) ; i += 1) {
	    rs = round() ;
	    len = rs ;
	    if (len > 0) break ;
	} /* end for */
	return (rs >= 0) ? len : rs ;
}
/* end method (subinfo::operator) */

int subinfo::round() noex {
	int		rs = 0 ;
	if (ft) {
	    rs = rem() ;
	} else {
	    rs = loc() ;
	}
	ft = (!ft) ;
	return rs ;
}
/* end method (subinfo::round) */

int subinfo::loc() noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = perm(fname,-1,-1,nullptr,am)) >= 0) {
	    USTAT	sb ;
	    if ((rs = uc_stat(fname,&sb)) >= 0) {
		if (! S_ISDIR(sb.st_mode)) {
		    rs = mkpath(rbuf,fname) ;
		    len = rs ;
		}
	    } else if (isNotAccess(rs)) {
	        rs = SR_OK ;
	    }
	} else if (isNotAccess(rs)) {
	    rs = SR_OK ;
	}
	return (rs >= 0) ? len : rs ;
}
/* end method (subinfo::loc) */

int subinfo::rem() noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	if (pr) {
	    if ((rs = mkpath(rbuf,pr,fname)) >= 0) {
		len = rs ;
	        if ((rs = perm(rbuf,-1,-1,nullptr,am)) >= 0) {
	            USTAT	sb ;
	            if ((rs = uc_stat(fname,&sb)) >= 0) {
		        if (S_ISDIR(sb.st_mode)) {
		            len = 0 ;
			}
		    } else if (isNotAccess(rs)) {
			len = 0 ;
			rs = SR_OK ;
		    }
	        } else if (isNotAccess(rs)) {
		    len = 0 ;
	            rs = SR_OK ;
	        }
	    } /* end if (mkpath) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end method (subinfo::loc) */


