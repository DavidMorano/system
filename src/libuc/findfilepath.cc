/* findfilepath SUPPORT */
/* lang=C++20 */

/* subroutine to try to find a file in the specified directory path */
/* version %I% last-modified %G% */

#define	CF_PREPENDPWD	0	/* prepend PWD when encountered? */
#define	CF_FILEPATH	1	/* always return a file path */
#define	CF_FILEPATHLEN	0	/* always return a file path length */

/* revision history:

	= 1998-05-01, David A­D­ Morano
	This subroutine was originally written.  It is based loosely
	on sinilar functions that I had before but not in as nice
	an interface.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine searches through all of the directories in
	the supplied directory path.  If the given file with the
	given mode is found in a directory then the path to this
	file is returned.  If the directory path is specified as
	NULL, then the current execution path (given by environment
	variable 'PATH') is used.

	Synopsis:
	int findfilepath(char *fpath,cchar *path,cchar *fname,int am) noex

	Arguments:
	fpath		resulting path to the file
	path		execution path or NULL to use default 'PATH'
	fname		file to be searched for
	am		file mode like w/ u_open(2) and u_access(2)

	Returns:
	>0		program was found elsewhere and this is the path length
	0		program was found in present working directory (PWD)
	<0		program was not found or error (system-return)

	fpath		resulting path if different than input

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<varnames.hh>
#include	<storebuf.h>
#include	<mkpathx.h>
#include	<ids.h>
#include	<getpwd.h>
#include	<xperm.h>
#include	<localmisc.h>

#include	"findfilepath.h"


/* local defines */

#ifndef	VARPATH
#define	VARPATH		"PATH"
#endif

#define	FD		struct fdata


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct fdata {
	int		pwdlen ;
	char		pwd[MAXPATHLEN + 1] ;
} ;


/* forward references */

static int	checkone(ids *,FD *,char *,cchar *,int,cchar *,int) noex ;
static int	checkit(ids *,char *,cchar *,int,cchar *,int) noex ;
static int	fileperm(ids *,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int findfilepath(char *fpath,cchar *path,cchar *fname,int am) noex {
	FD		d ;
	ids		ids ;
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	int		dirlen ;
	int		f_fpath = TRUE ;
	cchar	*tp ;
	cchar	*pp ;
	char		fpathbuf[MAXPATHLEN + 1] ;

	if ((rs = ids_load(&ids)) >= 0) {

	    if (fpath == NULL) {
	        f_fpath = FALSE ;
	        fpath = fpathbuf ;
	    }

	    fpath[0] = '\0' ;
	    if (fname[0] == '/') {

	        if ((rs = fileperm(&ids,fname,am)) >= 0) {
	            if (f_fpath) {
	                len = mkpath1(fpath,fname) ;
	            } else {
	                len = strlen(fname) ;
		    }
	        }

	    } else {

	        d.pwd[0] = '\0' ;
	        d.pwdlen = -1 ;

	        if (path == NULL)
	            path = getenv(VARPATH) ;

	        rs = SR_NOTFOUND ;
	        if (path != NULL) {

	            pp = path ;
	            while ((tp = strchr(pp,':')) != NULL) {

	                dirlen = tp - pp ;
	                rs = checkone(&ids,&d,fpath,pp,dirlen,fname,am) ;
	                len = rs ;

	                pp = tp + 1 ;
	                if (rs >= 0) break ;
	            } /* end while */

	            if (rs < 0) {

	                dirlen = strlen(pp) ;

	                rs = checkone(&ids,&d,fpath,pp,dirlen,fname,am) ;
	                len = rs ;
	            }

#if	CF_FILEPATH
	            if ((rs <= 0) && f_fpath) {

	                if (rs == 0) {

#if	CF_FILEPATHLEN
	                    rs = mkpath1(fpath,fname) ;
	                    len = rs ;
#else
	                    mkpath1(fpath,fname) ;
#endif /* CF_FILEPATHLEN */

	                } else
	                    fpath[0] = '\0' ;

	            } /* end if */
#else
	            if ((rs <= 0) && f_fpath)
	                fpath[0] = '\0' ;
#endif /* CF_FILEPATH */

	        } /* end if (non-NULL path) */

	    } /* end if */

	    rs1 = ids_release(&ids) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ids) */

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (findfilepath) */


/* local subroutines */

static int checkone(ids *idp,FD *dp,char *fpath,cc *dirpath,int dirlen,
		cc *fname,int am) noex {
	int		rs = SR_OK ;
	(void) dp ;

	if (dirlen == 0) {

#if	CF_PREPENDPWD
	    if (dp->pwdlen < 0) {

	        rs = getpwd(dp->pwd,MAXPATHLEN) ;
	        dp->pwdlen = rs ;

	    } /* end if */

	    if (dp->pwdlen >= 0)
	        rs = checkit(idp,fpath,dp->pwd,dp->pwdlen,fname,am) ;
#else
	    rs = fileperm(idp,fname,am) ;
	    if (rs >= 0)
	        rs = 0 ;
#endif /* CF_PREPENDPWD */

	} else {
	    rs = checkit(idp,fpath,dirpath,dirlen,fname,am) ;
	}

	return rs ;
}
/* end subroutine (checkone) */

static int checkit(ids *idp,char *pbuf,cc *dnp,int dnl,cc *fn,int am) noex {
	cint		plen = MAXPATHLEN ;
	int		rs = SR_OK ;
	int		i = 0 ;
	int		len = 0 ;
	if (dnl != 0) {
	    if (rs >= 0) {
	        rs = storebuf_strw(pbuf,plen,i,dnp,dnl) ;
	        i += rs ;
	    }
	    if (rs >= 0) {
	        rs = storebuf_char(pbuf,plen,i,'/') ;
	        i += rs ;
	    }
	    if (rs >= 0) {
	        rs = storebuf_strw(pbuf,plen,i,fn,-1) ;
	        i += rs ;
	    }
	    if (rs >= 0) {
	        pbuf[i] = '\0' ;
	        len = i ;
	    }
	    if (rs >= 0) {
	        rs = fileperm(idp,pbuf,am) ;
	    }
	} else {
	    if ((rs = fileperm(idp,fn,am)) >= 0) {
	        len = 0 ; /* <- WTF? */
	    }
	} /* end if */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (checkit) */

/* is it a file and are the permissions what we want? */
static int fileperm(ids *idp,cchar *fname,int am) noex {
	USTAT		sb ;
	int		rs ;
	if ((rs = u_stat(fname,&sb)) >= 0) {
	    rs = SR_NOTFOUND ;
	    if (S_ISREG(sb.st_mode))  {
	        rs = sperm(idp,&sb,am) ;
	    }
	} /* end if */
	return rs ;
}
/* end subroutine (fileperm) */


