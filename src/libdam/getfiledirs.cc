/* getfiledirs SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* subroutine to try and find a file in the specified directory path */
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
	dlp		pointer to VECSTR structure or NULL

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
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<vecstr.h>
#include	<localmisc.h>


/* local defines */

#ifndef	VARPATH
#define	VARPATH		"PATH"
#endif


/* external subroutines */

extern int	getpwd(char *,int) ;

extern char	*strwcpy(char *,const char *,int) ;


/* external variables */


/* local structures */


/* forward references */

static int	checkit(const char *,int,const char *,int,vecstr *) ;
static int	getmode(const char *) ;


/* local variables */


/* exported subroutines */


int getfiledirs(path,fname,modestr,slp)
const char	path[] ;
const char	fname[] ;
const char	modestr[] ;
vecstr		*slp ;
{
	int		n, dirlen ;
	int		pwdlen ;
	int		mode ;
	int		f_pwd = FALSE ;
	const char	*pp ;
	const char	*tp ;
	char		pwd[MAXPATHLEN + 1] ;

	if (path == NULL)
	    path = getenv(VARPATH) ;

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


/* local subroutines */


static int checkit(dir,dirlen,fname,mode,slp)
const char	dir[] ;
int		dirlen ;
const char	fname[] ;
int		mode ;
vecstr		*slp ;
{
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
	        if (slp != NULL) {
	            vecstr_add(slp,pathbuf,dirlen) ;
		}
	    }

	} /* end if (NULL directory or not) */

	return rs ;
}
/* end subroutine (checkit) */


static int getmode(const char *modestr)
{
	int		mode = 0 ;
	const char	*cp = modestr ;
	while (*cp) {
	    int	kc = (*cp++ & 0xff) ;
	    switch (kc) {
	    case 'r':
	        mode |= R_OK ;
	        break ;
	    case 'w':
	        mode |= W_OK ;
	        break ;
	    case 'x':
	        mode |= X_OK ;
	        break ;
	    } /* end switch */
	} /* end while */
	return mode ;
}
/* end subroutine (getmode) */


