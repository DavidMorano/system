/* findfilepath SUPPORT */
/* lang=C++20 */

/* subroutine to try to find a file in the specified directory path */
/* version %I% last-modified %G% */

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
	nullptr, then the current execution path (given by environment
	variable 'PATH') is used.

	Synopsis:
	int findfilepath(char *rbuf,cchar *path,cchar *fn,int am) noex

	Arguments:
	rbuf		resulting path to the file
	path		execution path or nullptr to use default 'PATH'
	fn		file to be searched for
	am		file mode like w/ u_open(2) and u_access(2)

	Returns:
	>0		program was found elsewhere and this is the path length
	0		program was found in present working directory (PWD)
	<0		program was not found or error (system-return)

	rbuf		resulting path if different than input

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<varnames.hh>
#include	<bufsizevar.hh>
#include	<mallocxx.h>
#include	<strlibval.hh>
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


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	checkone(ids *,char *,cchar *,int,cchar *,int) noex ;
static int	checkit(ids *,char *,cchar *,int,cchar *,int) noex ;
static int	fileperm(ids *,cchar *,int) noex ;
static bool	isendslash(cc *,int) noex ;


/* local variables */

static bufsizevar	maxpathlen(getbufsize_mp) ;

static strlibval	pathval(strlibval_path) ;


/* exported variables */


/* exported subroutines */

int findfilepath(char *rbuf,cchar *path,cchar *fn,int am) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (rbuf && fn) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (fn[0]) {
	        ids	ids ;
	        if ((rs = ids_load(&ids)) >= 0) {
	            bool	f_fpath = true ;
	            if (fn[0] == '/') {
	                if ((rs = fileperm(&ids,fn,am)) >= 0) {
	                    if (f_fpath) {
	                        len = mkpath1(rbuf,fn) ;
	                    } else {
	                        len = strlen(fn) ;
		            }
	                }
	            } else {
	                if (path == nullptr) path = pathval ;
	                rs = SR_NOTFOUND ;
	                if (path != nullptr) {
	                    int		dnl = -1 ;
	            	    cchar	*tp ;
	                    cchar	*dnp = path ;
	                    while ((tp = strchr(dnp,':')) != nullptr) {
			        {
	                            dnl = (tp - dnp) ;
	                            rs = checkone(&ids,rbuf,dnp,dnl,fn,am) ;
	                            len = rs ;
			        }
	                        dnp = (tp + 1) ;
	                        if (rs >= 0) break ;
	                    } /* end while */
	                    if (rs < 0) {
	                        dnl = strlen(dnp) ;
	                        rs = checkone(&ids,rbuf,dnp,dnl,fn,am) ;
	                        len = rs ;
	                    }

#if	CF_FILEPATH
	                    if ((rs <= 0) && f_fpath) {
	                        if (rs == 0) {
#if	CF_FILEPATHLEN
	                            rs = mkpath1(rbuf,fn) ;
	                            len = rs ;
#else
	                            rs = mkpath1(rbuf,fn) ;
#endif /* CF_FILEPATHLEN */
	                        } else {
	                            rbuf[0] = '\0' ;
			        }
	                    } /* end if */
#else
	                    if ((rs <= 0) && f_fpath) {
	                        rbuf[0] = '\0' ;
		            }
#endif /* CF_FILEPATH */
	                } /* end if (non-nullptr path) */
	            } /* end if */
	            rs1 = ids_release(&ids) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (ids) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (findfilepath) */


/* local subroutines */

static int checkone(ids *idp,char *rbuf,cc *dnp,int dnl,cc *fn,int am) noex {
	int		rs = SR_OK ;
	if (dnl == 0) {
	    if ((rs = fileperm(idp,fn,am)) >= 0) {
	        rs = 0 ;
	    }
	} else {
	    rs = checkit(idp,rbuf,dnp,dnl,fn,am) ;
	}
	return rs ;
}
/* end subroutine (checkone) */

static int checkit(ids *idp,char *pbuf,cc *dnp,int dnl,cc *fn,int am) noex {
	int		rs ;
	int		i = 0 ;
	if ((rs = maxpathlen) >= 0) {
	    cint	plen = rs ;
	    if (dnl != 0) {
	        if (rs >= 0) {
	            rs = storebuf_strw(pbuf,plen,i,dnp,dnl) ;
	            i += rs ;
	        }
	        if ((rs >= 0) && (! isendslash(dnp,dnl))) {
	            rs = storebuf_char(pbuf,plen,i,'/') ;
	            i += rs ;
	        }
	        if (rs >= 0) {
	            rs = storebuf_strw(pbuf,plen,i,fn,-1) ;
	            i += rs ;
	        }
	        if (rs >= 0) {
	            rs = fileperm(idp,pbuf,am) ;
	        }
	    } else {
	        if ((rs = fileperm(idp,fn,am)) >= 0) {
	            i = 0 ; /* <- ? */
	        }
	    } /* end if */
	} /* end if (maxpathlen) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (checkit) */

/* is it a file and are the permissions what we want? */
static int fileperm(ids *idp,cchar *fn,int am) noex {
	USTAT		sb ;
	int		rs ;
	if ((rs = u_stat(fn,&sb)) >= 0) {
	    rs = SR_NOTFOUND ;
	    if (S_ISREG(sb.st_mode))  {
	        rs = sperm(idp,&sb,am) ;
	    }
	} /* end if */
	return rs ;
}
/* end subroutine (fileperm) */

static bool isendslash(cc *dp,int dl) noex {
	return ((dl > 0) && (dp[dl-1] == '/')) ;
}


