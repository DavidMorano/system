/* getfname SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++21 */

/* get a file name according to rules */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-03, David A­D­ Morano
	This subroutine was originally written for Rightcore Network
	Services (RNS).

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
	program root area.  If the type is given as GETFNAME_TYPEROOT,
	the file is searched for in the program root area first and
	then locally.

	Synopsis:
	int getfname(cchar *pr,char *rbuf,getfnames type,cc *fn,int am) noex

	Arguments:
	pr		base directory path to check in
	rbuf		the output file buffer (user supplied)
	type		the type of the check to make
		0	search locally first
		1	search in the program root area first
	fn		the name of the input file to check for
	am		access-mode

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
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<utility>		/* C++STD |std::unreachable(c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucfileop.h>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<permx.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"getfname.h"


/* local defines */

#ifndef	GETFNAME_TYPELOCAL
#define	GETFNAME_TYPELOCAL	0	/* search locally first */
#define	GETFNAME_TYPEROOT	1	/* search PR area first */
#endif


/* imported namespaces */

using std::unreachable ;


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
	getfnames	ft ;
	subinfo(cc *p,char *r,getfnames t,cc *f,int a) noex : ft(t) {
	    pr = p ;
	    rbuf = r ;
	    fname = f ;
	    am = (a & O_ACCMODE) ;
	} ; /* end ctor */
	operator int () noex ;
	int round() noex ;
	int rem() noex ;
	int loc() noex ;
    } ; /* end struct (subinfo) */
} /* end namespace */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int getfname(cc *pr,char *rbuf,getfnames ft,cc *fn,int am) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (pr && rbuf && fn) ylikely {
	    rbuf[0] = '\0' ;
	    rs = SR_INVALID ;
	    if ((ft >= 0) && (ft < getfname_overlast) && fn[0]) ylikely {
		rs = SR_OK ;
	        if (fn[0] != '/') {
		    subinfo si(pr,rbuf,ft,fn,am) ;
		    rs = si ;
		    len = rs ;
		} /* end if (relative file-name) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (getrbuf) */


/* local subroutines */

subinfo::operator int () noex {
    	cint		n = getfname_overlast ;
	int		rs = SR_OK ;
	int		len = 0 ;
	for (int i = 0 ; (rs >= 0) && (i < n) ; i += 1) {
	    rs = round() ;
	    len = rs ;
	    if (len > 0) break ;
	} /* end for */
	return (rs >= 0) ? len : rs ;
} /* end method (subinfo::operator) */

int subinfo::round() noex {
	int		rs = 0 ;
	switch (ft) {
	case getfname_local:
	    rs = loc() ;
	    break ;
	case getfname_root:
	    rs = rem() ;
	    break ;
	default:
	    unreachable() ;
	} /* end switch */
	ft = getfnames(ft % getfname_overlast) ;
	return rs ;
} /* end method (subinfo::round) */

int subinfo::loc() noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = perm(fname,-1,-1,nullptr,am)) >= 0) {
	    if (ustat sb ; (rs = uc_stat(fname,&sb)) >= 0) {
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
} /* end method (subinfo::loc) */

int subinfo::rem() noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	if (pr) ylikely {
	    if ((rs = mkpath(rbuf,pr,fname)) >= 0) ylikely {
		len = rs ;
	        if ((rs = perm(rbuf,-1,-1,nullptr,am)) >= 0) {
	            if (ustat sb ; (rs = uc_stat(fname,&sb)) >= 0) {
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
} /* end method (subinfo::rem) */


