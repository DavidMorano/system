/* findfilepath SUPPORT */
/* lang=C++20 */

/* subroutine to try to find a file in the specified directory path */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	This code was originally written.  It is based loosely
	on sinilar functions that I had before but not in as nice
	an interface.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine searches through all of the directories in
	the supplied directory path.  If the given file with the
	given mode is found in a directory then the path to this
	file is returned.  If the directory path is specified as
	NUL:, then the current execution path (given by environment
	variable 'PATH') is used.

	Synopsis:
	int findfilepath(char *rbuf,cchar *path,cchar *fn,int am) noex

	Arguments:
	rbuf		resulting path to the file
	path		execution path or nullptr to use default 'PATH'
	fn		file to be searched for
	am		file mode like w/ |u_open(3u)| and |u_access(3u)|

	Returns:
	>0		program was found elsewhere and this is the path length
	==0		program not found
	<0		error (system-return)

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
#include	<isnot.h>
#include	<localmisc.h>

#include	"findfilepath.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	checkone(ids *,char *,cchar *,int,cchar *,int) noex ;
static int	fileperm(ids *,cchar *,int) noex ;
static int	mkourpath(char *,cc *,int,cc *) noex ;

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
	            if (fn[0] == '/') {
	                if ((rs = fileperm(&ids,fn,am)) > 0) {
			    rs = mkpath1(rbuf,fn) ;
	                    len = rs ;
	                }
	            } else {
	                if (path == nullptr) path = pathval ;
	                if (path && (path[0] != '\0')) {
	                    int		dnl = -1 ;
	            	    cchar	*tp ;
	                    cchar	*dnp = path ;
			    rs = SR_OK ;
	                    while ((tp = strchr(dnp,':')) != nullptr) {
			        {
	                            dnl = (tp - dnp) ;
	                            rs = checkone(&ids,rbuf,dnp,dnl,fn,am) ;
	                            len = rs ;
			        }
	                        dnp = (tp + 1) ;
	                        if (rs != SR_OK) break ;
	                    } /* end while */
	                } /* end if (non-nullptr path) */
	            } /* end if */
	            rs1 = ids_release(&ids) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (ids) */
	    } /* end if (valid) */
	    if ((rs < 0) || (len == 0)) rbuf[0] = '\0' ;
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (findfilepath) */


/* local subroutines */

static int checkone(ids *idp,char *pbuf,cc *dnp,int dnl,cc *fn,int am) noex {
	int		rs = SR_OK ;
	int		rl = 0 ;
	if (dnl != 0) {
	    if ((rs = mkourpath(pbuf,dnp,dnl,fn)) >= 0) {
		cint	tl = rs ;
	        if ((rs = fileperm(idp,pbuf,am)) > 0) {
		    rl = tl ;
	        } /* end if (fileperm) */
	    } /* end if (mkourpath) */
	} else {
	    if ((rs = fileperm(idp,fn,am)) > 0) {
	        rl = strlen(fn) ;
	    } /* end if (fileperm) */
	} /* end if */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (checkone) */

/* is it a file and are the permissions what we want? */
static int fileperm(ids *idp,cchar *fn,int am) noex {
	USTAT		sb ;
	int		rs ;
	int		f = false ;
	if ((rs = u_stat(fn,&sb)) >= 0) {
	    if (S_ISREG(sb.st_mode))  {
	        if ((rs = sperm(idp,&sb,am)) >= 0) {
		    f = true ;
		} else if (isNotAccess(rs)) {
		    rs = SR_OK ;
		}
	    } /* end if (regular file) */
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	} /* end if */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (fileperm) */

static int mkourpath(char *pbuf,cc *dnp,int dnl,cc *fn) noex {
	int		rs = SR_OK ;
	int		i = 0 ;
	if ((rs = maxpathlen) >= 0) {
	    cint	plen = rs ;
	    if (rs >= 0) {
	        rs = storebuf_strw(pbuf,plen,i,dnp,dnl) ;
	        i += rs ;
	    }
	    if ((rs >= 0) && (! isendslash(dnp,dnl))) {
	        rs = storebuf_chr(pbuf,plen,i,'/') ;
	        i += rs ;
	    }
	    if (rs >= 0) {
	        rs = storebuf_strw(pbuf,plen,i,fn,-1) ;
	        i += rs ;
	    }
	} /* end if (maxpathlen) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (mkourpath) */

static bool isendslash(cc *dp,int dl) noex {
	if (dl < 0) dl = strlen(dp) ;
	return ((dl > 0) && (dp[dl-1] == '/')) ;
}


