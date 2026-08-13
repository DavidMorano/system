/* getfiledirs SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* find all directories that contain a specified file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	This code was adopted from a previous function of mine (of
	some sort) that pretty much did the same thing.

	= 2024-12-08, David A­D­ Morano
	I added a comment note below (under "Notes").

*/

/* Copyright © 1998,2024 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	getfiledirs

	Description:
	This subroutine searches through the specified directory
	path for the given file with the given file mode.  A list
	of all directories that contain that file is optionally
	returned.

	Synopsis:
	int getfiledirs(cc *path,cc *filename,cc *mode,vecstr *dlp) noex

	Arguments:
	path		execution path or NULL to use default 'PATH'
	filename	file to be searched for
	mode		mode that file must have, one or more of:
			"x"	executable
			"r"	readable
			"w"	writeable
			"rx"	readable and executable
			et cetera
	dlp		pointer to VECSTR object or NULL

	Returns:
	>0	the number of directories the file was found in
	==0	file not found in any directory
	<0	error in finding file (system-return)

	Notes:
	This is a review comment (2024-12-08).  Looking through
	this code (now for a review of conformance w/ C++20), I can
	see (likely not obvious to anyone else) that this code
	originates from some very (very) old source (way predating
	the 1998 adoption).  I can tell by the style of coding that
	I used. 
	-- David A-D- Morano, 2024-12-08

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strchr(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<ucfileop.h>		/* LIBUC */
#include	<getpwd.h>		/* LIBUC */
#include	<ids.h>			/* LIBUC */
#include	<permx.h>		/* LIBUC */
#include	<strlibval.hh>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<mkpath.h>		/* LIBUC */
#include	<mknpath.h>		/* LIBUC */
#include	<pathadd.h>		/* LIBUC */
#include	<strwcpy.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<mkx.h>			/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"getfiledirs.h"

#pragma		GCC dependency		"mod/sif.ccm"

import sif ;

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

local cchar	*getdefpath() noex ;

namespace {
    struct getter {
	cchar		*path ;		/* passed argument */
	cchar		*fname ;	/* passed argument */
	vecstr		*dlp ;		/* passed argument */
	char		*pbuf{} ;
	ids		id ;
	int		am ;		/* derived argument */
	int		dlen ;
	int		plen{} ;
	bool		fpwd = false ;
	getter(cc *ap,cc *af,int aa,vecstr *vp) noex : path(ap), fname(af) {
	    if (path == nullptr) {
		local cchar *gpath = getdefpath() ;
		path = gpath ;
	    }
	    am = aa ;
	    dlp = vp ;
	} ; /* end ctor */
	operator int () noex ;
	int tryabs	() noex ;
	int tryrel	() noex ;
	int trypath	() noex ;
	int checkfile	(int) noex ;
	int checks	(cc *,int) noex ;
	int checker	(cc *,int) noex ;
	int checkname	(bool,int) noex ;
    } ; /* end struct (getter) */
} /* end namespace */

enum nametypes : uchar {
	nametype_dir,
	nametype_file,
	nametype_overlast
} ; /* end enum */


/* forward references */

local int	getmode(cchar *) noex ;


/* local variables */

static strlibval	defpath(strlibval_path) ;


/* exported variables */


/* exported subroutines */

int getfiledirs(cc *path,cc *fname,cc *modestr,vecstr *dlp) noex {
    	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (fname && dlp) ylikely {
	    cint	am = getmode(modestr) ;
	    rs = SR_INVALID ;
	    if (fname[0]) ylikely {
		if (getter go(path,fname,am,dlp) ; (rs = go) >= 0) {
		    c = rs ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (getfiledirs) */


/* local subroutines */

getter::operator int () noex {
    	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (path) ylikely {
    	    rs = SR_INVALID ;
	    if (path[0]) ylikely {
                if ((rs = id.load) >= 0) ylikely {
                    if ((rs = lm_mp(&pbuf)) >= 0) ylikely {
                        plen = rs ;
		        if (fname[0] == '/') {
		            rs = tryabs() ;
		            c = rs ;
		        } else if (strchr(fname,'/') != nullptr) {
		            rs = tryrel() ;
		            c = rs ;
		        } else {
		            rs = trypath() ;
		            c = rs ;
		        }
                        rs1 = lm_free(pbuf) ;
			if (rs >= 0) rs = rs1 ;
		        pbuf = nullptr ;
                        plen = 0 ;
                    } /* end if (m-a-f) */
                    rs1 = id.release ;
                    if (rs >= 0) rs = rs1 ;
                } /* end if (ids) */
	    } /* end if (valid) */
	} /* end if (non-null) */
    	return (rs >= 0) ? c : rs ;
} /* end method (getter::operator) */

int getter::tryabs() noex {
    	int		rs ;
	int		c = 0 ;
	if ((rs = mknpath(pbuf,plen,fname)) >= 0) ylikely {
	    rs = checkfile(rs) ;
	    c = rs ;
	} /* end if (mknpath) */
	return (rs >= 0) ? c : rs ;
} /* end method (getter::tryabs) */

int getter::tryrel() noex {
    	int		rs ;
	int		c = 0 ;
	if ((rs = getpwd(pbuf,plen)) >= 0) ylikely {
	    if ((rs = pathnadd(pbuf,plen,rs,fname)) >= 0) ylikely {
		rs = checkfile(rs) ;
		c = rs ;
	    } /* end if (pathnadd) */
	} /* end if (getpwd) */
	return (rs >= 0) ? c : rs ;
} /* end method (getter::tryrel) */

int getter::checkfile(int pl) noex {
    	int		rs ;
	int		c = 0 ;
	cbool		ty = nametype_file ;
	if ((rs = checkname(ty,am)) > 0) {
	    cchar	*cp{} ;
	    if (int cl ; (cl = sfdirname(pbuf,pl,&cp)) > 0) {
		rs = dlp->adduniq(cp,cl) ;
		c += (rs < INT_MAX) ;
	    }
	} /* end if (checkname) */
	return (rs >= 0) ? c : rs ;
} /* end method (getter::checkfile) */

int getter::trypath() noex {
	sif		po(path,-1,":;") ;
    	int		rs = SR_OK ;
	int		c = 0 ;
	cchar		*pp ;
	for (int pl ; (pl = po(&pp)) >= 0 ; ) {
	    rs = checks(pp,pl) ;
	    c += rs ;
	    if (rs < 0) break ;
	} /* end for */
	return (rs >= 0) ? c : rs ;
} /* end method (getter::trypath) */

int getter::checks(cc *dp,int dl) noex {
    	int		rs ;
	int		c = 0 ;
	if ((rs = checker(dp,dl)) > 0) {
	    c = rs ;
	    rs = dlp->adduniq(pbuf,dlen) ;
	}
	return (rs >= 0) ? c : rs ;
} /* end method (getter::checks) */

int getter::checker(cc *dp,int dl) noex {
    	int		rs = SR_OK ;
	int		c = 0 ; /* return-value */
	bool		ty{} ; /* used-multiple */
	if (dl > 0) {
	    if ((rs = mknpathw(pbuf,plen,dp,dl)) >= 0) ylikely {
		cint	dm = (R_OK|X_OK) ;
		dlen = rs ;
		ty = nametype_dir ;
		if ((rs = checkname(ty,dm)) > 0) {
		    if ((rs = pathnadd(pbuf,plen,dlen,fname)) >= 0) {
			ty = nametype_file ;
		        rs = checkname(ty,am) ;
			c = rs ;
		    }
		} /* end if (checkname) */
	    } /* end if (mkpath) */
	} else if (! fpwd) {
	    dlen = 0 ;
	    if ((rs = mknpath(pbuf,plen,fname)) >= 0) ylikely {
	        ty = nametype_file ;
	        if ((rs = checkname(ty,am)) > 0) {
	            c = rs ;
	    	    fpwd = true ;
		    rs = getpwd(pbuf,plen) ;
		    dlen = rs ;
		} /* end if (checkname) */
	    } /* end if (mkpath) */
	} /* end if */
	return (rs >= 0) ? c : rs ;
} /* end method (getter::checker) */

int getter::checkname(bool fdir,int fam) noex {
    	int		rs ;
	int		c = 0 ;
	if (ustat sb ; (rs = uc_stat(pbuf,&sb)) >= 0) {
	    cmode	pm = sb.st_mode ;
	    if ((fdir && S_ISDIR(pm)) || ((!fdir) && S_ISREG(pm))) {
	        if ((rs = permid(&id,&sb,fam)) >= 0) {
	            c = 1 ;
	        } else if (isNotAccess(rs)) {
		    rs = SR_OK ;
		}
	    } /* end if (is-dir) */
	} else if (isNotPresent(rs)) { /* including EPERM for Apple-Darwin */
	    rs = SR_OK ;
	}
	return (rs >= 0) ? c : rs ;
} /* end method (getter::checkname) */

local int getmode(cchar *modestr) noex {
	int		am = (R_OK|X_OK) ;
	if (modestr) ylikely {
	    if (modestr[0]) ylikely {
	        cchar	*cp = modestr ;
	        am = 0 ;
	        for (int kch ; (kch = mkchar(*cp++)) != 0 ; ) {
	            switch (kch) {
	            case 'r':
	                am |= R_OK ;
	                break ;
	            case 'w':
	                am |= W_OK ;
	                break ;
	            case 'x':
	                am |= X_OK ;
	                break ;
	            } /* end switch */
	        } /* end for */
	    } /* end if (not-empty) */
	} /* end if (non-null) */
	return am ;
} /* end subroutine (getmode) */

local cchar *getdefpath() noex {
    	return defpath ;
} /* end subroutine */


