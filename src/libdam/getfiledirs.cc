/* getfiledirs SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* find all directories that contain a specified file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

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

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>
#include	<usystem.h>
#include	<uvariables.hh>
#include	<getpwd.h>
#include	<mallocxx.h>
#include	<strlibval.hh>
#include	<vecstr.h>
#include	<strwcpy.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"getfiledirs.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct getter {
	cchar		*path ;		/* subroutine argument */
	cchar		*fname ;	/* subroutine argument */
	vecstr		*dlp ;		/* subroutine argument */
	char		*pwd{} ;
	char		*pbuf{} ;
	int		am ;		/* derived argument */
	int		plen{} ;
	getter(cc *ap,cc *af,int aa,vecstr *vp) noex : path(ap), fname(af) {
	    if (path == nullptr) {
		static cchar *gpath = getenv(varname.path) ;
		path = gpath ;
	    }
	    am = aa ;
	    dlp = vp ;
	} ; /* end ctor */
	operator int () noex ;
    } ; /* end struct (getter) */
}


/* forward references */

static int	checkit(cchar *,int,cchar *,int,vecstr *) noex ;
static int	getmode(cchar *) noex ;


/* local variables */

static strlibval		envpath(strlibval_path) ;


/* exported variables */


/* exported subroutines */

int getfiledirs(cc *path,cc *fname,cc *modestr,vecstr *dlp) noex {
    	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (fname && dlp) {
	    cint	am = getmode(modestr) ;
	    rs = SR_INVALID ;
	    if (fname[0]) {
		if (getter go(path,fname,am,dlp) ; (rs = go) >= 0) {
		    c = rs ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (getfiledirs) */

#ifdef	COMMENT
	int		n, dirlen ;
	int		pwdlen ;
	int		mode ;
	int		f_pwd = FALSE ;
	cchar	*pp ;
	cchar	*tp ;
	char		pwd[MAXPATHLEN + 1] ;

	if (path == NULL)
	    path = getenv(varname.path) ;

	if (path == NULL)
	    return SR_INVALID ;

	mode = getmode(modestr) ;

	n = 0 ;
	pp = path ;
	while ((tp = strchr(pp,':')) != NULL) {

	    dirlen = (tp - pp) ;
	    if (dirlen == 0) {

	        if (! f_pwd) {
	            f_pwd = TRUE ;
	            pwdlen = getpwd(pwd,MAXPATHLEN) ;
	        }

	        if (pwdlen >= 0) {
	            n += checkit(pwd,pwdlen,fname,mode,slp) ;
		}

	    } else {
	        n += checkit(pp,dirlen,fname,mode,slp) ;
	    }

	    pp = (tp + 1) ;

	} /* end while */

	dirlen = strlen(pp) ;

	if (dirlen == 0) {

	    if (! f_pwd) {
	        f_pwd = TRUE ;
	        pwdlen = getpwd(pwd,MAXPATHLEN) ;
	    }

	    if (pwdlen >= 0)
	        n += checkit(pwd,pwdlen,fname,mode,slp) ;

	} else
	    n += checkit(pp,dirlen,fname,mode,slp) ;

	return n ;
}
/* end subroutine (getfiledirs) */
#endif /* COMMENT */


/* local subroutines */

getter::operator int () noex {
    	int		rs = SR_FAULT ;
	if (path) {
    	    rs = SR_INVALID ;
	    if (path[0]) {
	        if ((rs = malloc_mp(&pbuf)) >= 0) {
		    plen = rs ;
		    {
	                rs = SR_OK ;
		    }
		    rs = rsfree(rs,pbuf) ;
	        } /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (non-null) */
    	return rs ;
}
/* end method (getter::operator) */

static int checkit(cchar *dir,int dirlen,cc *fname,int mode,vecstr *slp) noex {
	int		rs = 0 ;
	char		pathbuf[MAXPATHLEN + 1], *pbp ;

	if (dirlen != 0) {

	    pbp = pathbuf ;
	    pbp = strwcpy(pathbuf,dir,dirlen) ;

/* recalculate the directory length if it was given as negative */

	    dirlen = pbp - pathbuf ;
	    if (u_access(pathbuf,X_OK) >= 0) {

	        pbp = strwcpy(pbp,"/",MAXPATHLEN - (pbp - pathbuf)) ;

	        pbp = strwcpy(pbp,fname,MAXPATHLEN - (pbp - pathbuf)) ;

	        if (u_access(pathbuf,mode) >= 0) {
	            rs = 1 ;
	            if (slp != NULL) {
	                vecstr_add(slp,pathbuf,dirlen) ;
		    }
	        }

	    } /* end if */

	} else {

	    if (u_access(fname,mode) >= 0) {
	        rs = 1 ;
	        if (slp) {
	            rs = vecstr_add(slp,pathbuf,dirlen) ;
		}
	    }

	} /* end if (NULL directory or not) */

	return rs ;
}
/* end subroutine (checkit) */

static int getmode(cchar *modestr) noex {
	int		am = (R_OK|X_OK) ;
	if (modestr) {
	    if (modestr[0]) {
	        cchar		*cp = modestr ;
	        am = 0 ;
	        for (int kc ; (kc = mkchar(*cp++)) != 0 ; ) {
	            switch (kc) {
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
	        } /* end while */
	    } /* end if (not-empty) */
	} /* end if (non-null) */
	return am ;
}
/* end subroutine (getmode) */


